# RALPH PROMPT: Fix AnimBP Migration with AAA-Quality C++ Implementation

## MISSION

Fix ALL AnimBlueprint issues caused by Blueprint-to-C++ migration. You MUST achieve **exact structural and functional equivalence** with the original Blueprint-only configuration (C:\scripts\bp_only).

**AAA GAME STUDIO STANDARDS:**
- NO fragile reflection hacks
- NO hardcoded property GUIDs  
- PROPER use of UE5 C++ APIs (Cast<>, typed accessors)
- PIN-LEVEL structural validation
- COMPILE-TIME error extraction and fixing

---

## CRITICAL ARCHITECTURE: LinkedAnimLayer Nodes

### Understanding the Problem

From UE5 source code (AnimGraphNode_LinkedAnimLayer.h and AnimNode_LinkedAnimLayer.h):

```cpp
// Editor node (UAnimGraphNode_LinkedAnimLayer)
UPROPERTY(EditAnywhere, Category = Settings)
FAnimNode_LinkedAnimLayer Node;  // Runtime struct

UPROPERTY()
FGuid InterfaceGuid;             // GUID of the implementing graph

// Methods:
FName GetLayerName() const;
void SetLayerName(FName InName);
void UpdateGuidForLayer();

// Runtime struct (FAnimNode_LinkedAnimLayer)
UPROPERTY()
TSubclassOf<UAnimLayerInterface> Interface;  // nullptr = "self layer"

UPROPERTY(EditAnywhere, Category = Settings)
FName Layer;  // e.g., "LL_Idle", "LL_Shield_Right"
```

**Self-Layer Pattern:**
When `Interface` is nullptr, the AnimBP implements the layer within itself. The `Layer` FName should match a FunctionGraph name in the AnimBP.

**Error "uses invalid layer 'LL_Idle'":**
1. The implementing graph can't be found in FunctionGraphs, OR
2. The InterfaceGuid points to wrong/missing graph GUID

---

## NEW C++ FUNCTIONS TO IMPLEMENT

Add to SLFAnimBPExport.inl with PROPER UE5 APIs.

### Required Include
```cpp
#include "AnimGraphNode_LinkedAnimLayer.h"
```

### 1. DiagnoseLinkedAnimLayerNodes
```cpp
FString USLFAutomationLibrary::DiagnoseLinkedAnimLayerNodes(UObject* AnimBlueprintAsset)
{
    UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
    if (!AnimBP) return TEXT("Error: Not an AnimBlueprint");

    FString Result;
    
    // Build map of FunctionGraph names to GUIDs
    TMap<FName, FGuid> GraphNameToGuid;
    for (UEdGraph* Graph : AnimBP->FunctionGraphs)
    {
        if (Graph)
            GraphNameToGuid.Add(FName(*Graph->GetName()), Graph->GraphGuid);
    }

    // Find LinkedAnimLayer nodes
    TArray<UEdGraph*> AllGraphs;
    AllGraphs.Append(AnimBP->UbergraphPages);
    AllGraphs.Append(AnimBP->FunctionGraphs);

    for (UEdGraph* Graph : AllGraphs)
    {
        for (UEdGraphNode* Node : Graph->Nodes)
        {
            // Use proper Cast<> - NOT string matching!
            UAnimGraphNode_LinkedAnimLayer* LayerNode = Cast<UAnimGraphNode_LinkedAnimLayer>(Node);
            if (!LayerNode) continue;

            // Access via public members and API
            const FAnimNode_LinkedAnimLayer& RuntimeNode = LayerNode->Node;
            bool bIsSelfLayer = (*RuntimeNode.Interface == nullptr);
            FName LayerName = RuntimeNode.Layer;
            
            Result += FString::Printf(TEXT("Node: %s, Layer: %s, SelfLayer: %s\n"),
                *LayerNode->GetName(),
                *LayerName.ToString(),
                bIsSelfLayer ? TEXT("YES") : TEXT("NO"));

            if (bIsSelfLayer)
            {
                FGuid* FoundGuid = GraphNameToGuid.Find(LayerName);
                if (FoundGuid)
                {
                    if (*FoundGuid == LayerNode->InterfaceGuid)
                        Result += TEXT("  [OK] GUID matches\n");
                    else
                        Result += FString::Printf(TEXT("  [FIX NEEDED] GUID mismatch: %s vs %s\n"),
                            *LayerNode->InterfaceGuid.ToString(), *FoundGuid->ToString());
                }
                else
                {
                    Result += FString::Printf(TEXT("  [ERROR] No graph for layer: %s\n"), *LayerName.ToString());
                }
            }
        }
    }
    return Result;
}
```

### 2. FixLinkedAnimLayerGuid
```cpp
int32 USLFAutomationLibrary::FixLinkedAnimLayerGuid(UObject* AnimBlueprintAsset)
{
    UAnimBlueprint* AnimBP = Cast<UAnimBlueprint>(AnimBlueprintAsset);
    if (!AnimBP) return 0;

    int32 FixCount = 0;
    TMap<FName, FGuid> GraphNameToGuid;
    for (UEdGraph* Graph : AnimBP->FunctionGraphs)
        if (Graph) GraphNameToGuid.Add(FName(*Graph->GetName()), Graph->GraphGuid);

    TArray<UEdGraph*> AllGraphs;
    AllGraphs.Append(AnimBP->UbergraphPages);
    AllGraphs.Append(AnimBP->FunctionGraphs);

    for (UEdGraph* Graph : AllGraphs)
    {
        for (UEdGraphNode* Node : Graph->Nodes)
        {
            UAnimGraphNode_LinkedAnimLayer* LayerNode = Cast<UAnimGraphNode_LinkedAnimLayer>(Node);
            if (!LayerNode) continue;

            const FAnimNode_LinkedAnimLayer& RuntimeNode = LayerNode->Node;
            if (*RuntimeNode.Interface != nullptr) continue; // Not self-layer

            FGuid* CorrectGuid = GraphNameToGuid.Find(RuntimeNode.Layer);
            if (CorrectGuid && *CorrectGuid != LayerNode->InterfaceGuid)
            {
                LayerNode->InterfaceGuid = *CorrectGuid;
                LayerNode->UpdateGuidForLayer(); // Use proper API!
                FixCount++;
            }
        }
    }

    if (FixCount > 0)
        FBlueprintEditorUtils::MarkBlueprintAsModified(AnimBP);

    return FixCount;
}
```

---

## ACCEPTANCE CRITERIA

Output `<promise>ANIMBP_FIX_COMPLETE</promise>` ONLY when:

1. **C++ builds with 0 errors**
2. **DiagnoseLinkedAnimLayerNodes shows all [OK]**
3. **Pin-level diff shows ZERO [CRITICAL]**
4. **GetBlueprintCompileErrors returns empty**
5. **PIE test starts and animations work**

---

## WORKFLOW

1. Add #include "AnimGraphNode_LinkedAnimLayer.h" 
2. Implement DiagnoseLinkedAnimLayerNodes() and FixLinkedAnimLayerGuid()
3. Build C++
4. Copy to bp_only, build
5. Diagnose backup (baseline)
6. Run migration
7. Diagnose current (identify issues)
8. Fix with FixLinkedAnimLayerGuid()
9. Fix Property Access paths
10. Run pin-level diff - ZERO [CRITICAL]
11. Verify 0 compile errors
12. PIE test
13. Output promise when ALL pass

**DO NOT output completion promise until pin-level diff shows ZERO [CRITICAL] and compile errors empty.**
