# AnimGraph Migration Strategy

## Architecture Overview

The approach is to move **EventGraph logic to C++** while keeping the **AnimGraph in Blueprint**.

```
ABP_SoulslikeNPC
├── EventGraph (C++ - NativeUpdateAnimation)
│   └── Populates UPROPERTY variables
│
└── AnimGraph (Blueprint - unchanged)
    └── Reads UPROPERTY variables from AnimInstance
```

**Why this works:**
- AnimGraph nodes read variables by **name**
- When Blueprint is reparented to C++ AnimInstance, the C++ UPROPERTYs become available
- AnimGraph binds to C++ properties automatically (same names)

## Implementation Steps

### Step 1: C++ AnimInstance with UPROPERTY Variables

```cpp
UCLASS()
class UMyNPCAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // Exposed to Blueprint AnimGraph
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Data")
    FVector LookAtLocation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Data")
    bool HasLookAtTarget;

protected:
    // Replaces Blueprint EventGraph
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
```

### Step 2: Populate Variables in C++

```cpp
void UMyNPCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* OwnerPawn = TryGetPawnOwner();
    if (!OwnerPawn) return;

    if (ASoulslikeNPC* NPC = Cast<ASoulslikeNPC>(OwnerPawn))
    {
        // Call function on NPC, set variable for AnimGraph
        LookAtLocation = NPC->GetLookAtLocation();
        HasLookAtTarget = !LookAtLocation.IsZero();
    }
}
```

### Step 3: Reparent Blueprint to C++

1. Clear the Blueprint EventGraph (already done by migration)
2. Reparent to C++ AnimInstance class
3. AnimGraph automatically finds C++ properties by name

### Step 4: Wire AnimGraph Pins (The Hard Part)

**Problem:** Python API cannot create pin connections in AnimGraph.

**Solutions:**

#### Option A: Pre-wired Template (Recommended)
- Keep existing AnimGraph wiring intact
- Just reparent to C++ - wiring preserved if variable names match

#### Option B: C++ Function to Wire Pins
Create a Blueprint Function Library to expose `TryCreateConnection`:

```cpp
UCLASS()
class UAnimGraphWiringUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "AnimGraph Utils")
    static bool ConnectNodes(
        UEdGraphNode* OutputNode, FName OutputPinName,
        UEdGraphNode* InputNode, FName InputPinName);
};

// Implementation
bool UAnimGraphWiringUtils::ConnectNodes(...)
{
    UEdGraphPin* PinA = OutputNode->FindPin(OutputPinName);
    UEdGraphPin* PinB = InputNode->FindPin(InputPinName);

    const UEdGraphSchema* Schema = OutputNode->GetSchema();
    return Schema->TryCreateConnection(PinA, PinB);
}
```

Then call from Python:
```python
unreal.AnimGraphWiringUtils.connect_nodes(
    var_getter_node, "LookAtLocation",
    look_at_node, "LookAtLocation"
)
```

## Current Situation: ABP_SoulslikeNPC

### What Exists:
- `USLFNPCAnimInstance` C++ class with properties:
  - `LookAtLocation` (FVector)
  - `HasLookAtTarget` (bool)
  - `DistanceToLookAtTarget` (double)

### What's Missing:
- C++ `NativeUpdateAnimation()` doesn't populate look-at variables (has TODO comment)
- Need to call `ASLFSoulslikeNPC::GetLookAtLocation()` to populate them

### The Error:
```
In use pin Location no longer exists on node GetLookAtLocation
```

This is the **EventGraph** (not AnimGraph) calling `GetLookAtLocation()` on the NPC.
The function signature changed: Blueprint expected output param `Location`, C++ returns `FVector`.

### The Fix:
1. Complete C++ `NativeUpdateAnimation()` to populate look-at variables
2. Clear EventGraph entirely (logic now in C++)
3. AnimGraph continues to work - reads variables by name

## Thread Safety Note (UE 5.7+)

For better performance, consider `NativeThreadSafeUpdateAnimation`:
- Runs on worker thread (faster)
- Must ensure `GetLookAtLocation()` is thread-safe
- If not thread-safe, use regular `NativeUpdateAnimation`

## Testing Checklist

- [ ] Complete `USLFNPCAnimInstance::NativeUpdateAnimation()` with look-at logic
- [ ] Build C++ successfully
- [ ] Clear ABP_SoulslikeNPC EventGraph
- [ ] Reparent to C++ AnimInstance
- [ ] Verify AnimGraph finds C++ properties
- [ ] Test in PIE - NPC look-at behavior works
