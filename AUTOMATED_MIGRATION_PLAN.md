# Automated Blueprint-to-C++ Migration Plan

## BREAKTHROUGH INSIGHT (December 28, 2025)

**The Problem We Solved:**
CoreRedirects work for asset loading/serialization but NOT for Blueprint graph compilation. Graph nodes (K2Node_MakeStruct, K2Node_BreakStruct, etc.) have hardcoded type references that don't go through the redirect system during compilation.

**The Solution:**
Use the JSON exports (which contain complete node/pin/connection data) to AUTOMATE fixing broken nodes after migration.

---

## Key Resources Available

1. **JSON Exports Location:** `C:\scripts\SLFConversion_Migration\Backups\SoulsClean\Exports\BlueprintDNA\`
   - Contains complete Blueprint DNA: nodes, pins, connections, GUIDs, type paths
   - 2231 occurrences of FStatInfo/FStatOverride/B_Stat across 91 files

2. **SLFAutomationLibrary** (C:\scripts\SLFConversion\Source\SLFConversion\SLFAutomationLibrary.h)
   - `AdvancedForceReplace()` - Caches pin connections, reconstructs nodes, restores wires
   - `MigrateTMapVariable()` - Handles wildcard pins for Map operations
   - `ReparentBlueprint()` - Changes parent class
   - `DeleteBlueprintVariable()` - Removes BP variables

3. **K2Node API** - Nodes have `ReconstructNode()` that refreshes pins after type changes

---

## JSON Structure (What We Can Parse)

```json
{
  "NodeClass": "K2Node_BreakStruct",
  "NodeGuid": "0BDCBB5B4C921C407C81298A28204C98",
  "NodeTitle": "Break FStat Info",
  "Pins": [
    {
      "Name": "FStatInfo",
      "Direction": "Input",
      "Type": {
        "SubCategoryObject": "FStatInfo",
        "SubCategoryObjectPath": "/Game/SoulslikeFramework/Structures/Stats/FStatInfo.FStatInfo"
      },
      "Connections": [
        { "NodeGuid": "...", "PinName": "..." }
      ]
    }
  ]
}
```

---

## The Automated Migration Flow

### Phase 1: Preparation
1. Parse all JSON exports
2. Build a map: `{ BlueprintPath -> [{ NodeGuid, NodeClass, OldTypePath, Connections }] }`
3. This tells us exactly which nodes in which Blueprints need fixing

### Phase 2: Type Migration
1. Ensure C++ structs exist (FStatInfo, FStatOverride, FRegen, FStatBehavior in SLFStatTypes.h) - DONE
2. Ensure C++ enum exists (E_ValueType in SLFStatTypes.h) - DONE
3. Delete Blueprint UserDefinedStructs from Content folder
4. Delete Blueprint Enum from Content folder
5. CoreRedirects in DefaultEngine.ini redirect old paths to new C++ types - DONE

### Phase 3: Automated Node Fixing (NEW CAPABILITY NEEDED)
1. Extend SLFAutomationLibrary with new function:
   ```cpp
   UFUNCTION(BlueprintCallable, Category = "SLF Automation")
   static int32 RefreshStructNodes(UObject* AssetObject, FString OldStructPath, FString NewStructPath);
   ```
2. This function:
   - Iterates all nodes in the Blueprint graph
   - Finds K2Node_MakeStruct and K2Node_BreakStruct nodes
   - For nodes referencing the old struct path, calls ReconstructNode()
   - ReconstructNode() picks up the new type via CoreRedirects
   - Returns count of fixed nodes

3. Python script using JSON exports:
   ```python
   for blueprint_path, nodes in affected_blueprints.items():
       bp = load_asset(blueprint_path)
       for node_info in nodes:
           automation_lib.RefreshStructNodes(bp, old_path, new_path)
       save_asset(bp)
   ```

### Phase 4: AC_StatManager Migration
1. Reparent AC_StatManager to USoulslikeStatComponent
2. C++ properties shadow Blueprint variables
3. Run AdvancedForceReplace for any remaining variable references
4. Delete Blueprint variables that are now in C++

### Phase 5: B_Stat Migration (if needed)
1. Create USLFStatObject C++ base class
2. Reparent B_Stat to USLFStatObject
3. Same node-fixing process

---

## Dependencies (Order Matters)

```
Layer 0 - No Dependencies (DONE):
├── E_ValueType (C++ enum)
├── FRegen (C++ struct)
├── FStatBehavior (C++ struct)
├── FStatInfo (C++ struct) - depends on FRegen, FStatBehavior
└── FStatOverride (C++ struct) - depends on FRegen

Layer 1 - Struct Migration:
└── Delete Blueprint structs/enum, CoreRedirects handle loading

Layer 2 - Node Fixing:
└── RefreshStructNodes() on all 91 affected Blueprints

Layer 3 - Component Migration:
├── B_Stat → USLFStatObject (has StatInfo property of type FStatInfo)
└── AC_StatManager → USoulslikeStatComponent (has TMaps using B_Stat, FStatOverride)
```

---

## Files to Create/Modify

1. **SLFAutomationLibrary.h/.cpp** - Add RefreshStructNodes() function
2. **refresh_struct_nodes.py** - Python script to parse JSONs and call RefreshStructNodes
3. **migrate_full.py** - Master migration script that orchestrates everything

---

## Why This Will Work

1. **JSON exports have everything** - Node GUIDs, type paths, pin connections
2. **K2Node::ReconstructNode()** - Engine function that refreshes node pins
3. **CoreRedirects work for loading** - When ReconstructNode() queries the type, redirect kicks in
4. **SLFAutomationLibrary proven** - Already has working node manipulation code

---

## Performance Consideration (Also Discussed)

For a Souls-like game, stat functions are called on events (dodge, hit, etc.), not every frame.
The overhead of Blueprint->C++ calls is negligible for event-driven code.
The real performance win is eliminating Event Tick and using Timers instead.

---

## Backup Before Testing

Always restore from: `C:\scripts\SLFConversion_Migration\Backups\blueprint_only\Content`
