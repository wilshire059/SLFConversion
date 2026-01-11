# AnimBP C++ Native Migration Skill

## Overview

Migrate Animation Blueprints from Blueprint-driven updates to C++ native `NativeUpdateAnimation()`, eliminating Blueprint functions while preserving the AnimGraph.

**CRITICAL: NO REFLECTION NEEDED** - The proper approach is to rename Blueprint variables to match C++ property names, NOT to use reflection to set `_0` suffix variables. After renaming, AnimGraph reads directly from C++ UPROPERTY.

---

## When to Use This Skill

Use this skill when:
- AnimBP has Blueprint functions that set animation variables (EventGraph, BlueprintThreadSafeUpdateAnimation)
- You want to move animation variable updates to C++ for performance
- AnimBP was reparented to C++ AnimInstance but still has redundant Blueprint functions

---

## Key Concepts

### What Gets Preserved vs Cleared

| Preserve | Clear |
|----------|-------|
| AnimGraph (state machines, blends) | EventGraph |
| AnimGraph variable reads (K2Node_VariableGet) | BlueprintThreadSafeUpdateAnimation |
| BlendListByEnum nodes | Helper functions (GetVelocityData, etc.) |
| State machine transitions | Component getter functions |
| Linked Anim Layers | Data extraction functions |

### AnimGraph Variable Reads

AnimGraph nodes read variables via `K2Node_VariableGet`. These nodes reference variables by FName. After reparenting to C++:
- C++ UPROPERTY variables are accessible by the same name
- Blueprint variables with `_0` suffix (from rename during reparenting) need to be renamed back OR have their bindings fixed

---

## Migration Process

### Phase 1: Prepare C++ AnimInstance

Ensure your C++ AnimInstance class has all variables the AnimGraph needs:

```cpp
UCLASS()
class UABP_SoulslikeCharacter_Additive : public UAnimInstance
{
    GENERATED_BODY()

public:
    // Movement (set from CharacterMovementComponent)
    UPROPERTY(BlueprintReadWrite, Category = "Animation|Movement")
    float Speed = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Animation|Movement")
    float Direction = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Animation|Movement")
    bool bIsAccelerating = false;

    UPROPERTY(BlueprintReadWrite, Category = "Animation|Movement")
    bool bIsFalling = false;

    // State (set from components)
    UPROPERTY(BlueprintReadWrite, Category = "Animation|State")
    bool bIsBlocking = false;

    UPROPERTY(BlueprintReadWrite, Category = "Animation|State")
    bool IsResting = false;

    // Overlay states (for BlendListByEnum)
    UPROPERTY(BlueprintReadWrite, Category = "Animation|Overlay")
    ESLFOverlayState LeftHandOverlayState = ESLFOverlayState::Default;

    UPROPERTY(BlueprintReadWrite, Category = "Animation|Overlay")
    ESLFOverlayState RightHandOverlayState = ESLFOverlayState::Default;

    // IK data
    UPROPERTY(BlueprintReadWrite, Category = "Animation|IK")
    float IkWeight = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Animation|IK")
    FVector ActiveHitNormal = FVector::ZeroVector;

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
```

### Phase 2: Implement NativeUpdateAnimation

```cpp
void UABP_SoulslikeCharacter_Additive::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* OwnerPawn = TryGetPawnOwner();
    if (!OwnerPawn) return;

    ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerPawn);
    if (!OwnerCharacter) return;

    // Movement data from CharacterMovementComponent
    if (UCharacterMovementComponent* MovementComp = OwnerCharacter->GetCharacterMovement())
    {
        FVector Velocity2D = OwnerCharacter->GetVelocity();
        Velocity2D.Z = 0.0f;
        Speed = Velocity2D.Size();

        bIsFalling = MovementComp->IsFalling();

        FVector Acceleration2D = MovementComp->GetCurrentAcceleration();
        Acceleration2D.Z = 0.0f;
        bIsAccelerating = Acceleration2D.SizeSquared() > 1.0f;
    }

    // Component data (CombatManager, EquipmentManager, etc.)
    if (UAC_CombatManager* CombatMgr = OwnerCharacter->FindComponentByClass<UAC_CombatManager>())
    {
        bIsBlocking = CombatMgr->IsGuarding;
        IkWeight = CombatMgr->IKWeight;
        ActiveHitNormal = CombatMgr->CurrentHitNormal;
    }

    if (UAC_EquipmentManager* EquipMgr = OwnerCharacter->FindComponentByClass<UAC_EquipmentManager>())
    {
        LeftHandOverlayState = EquipMgr->LeftHandOverlayState;
        RightHandOverlayState = EquipMgr->RightHandOverlayState;
    }
}
```

### Phase 3: Reparent AnimBP to C++ Class

```python
# reparent_animbp.py
import unreal

def main():
    animbp_path = "/Game/Path/To/YourAnimBP"
    cpp_class_path = "/Script/YourModule.ABP_YourAnimInstance"

    bp = unreal.load_asset(animbp_path)
    cpp_class = unreal.load_class(None, cpp_class_path)

    success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
    if success:
        unreal.EditorAssetLibrary.save_asset(animbp_path)

if __name__ == "__main__":
    main()
```

### Phase 4: Rename Variables to Match C++ Properties

After reparenting, Blueprint variables that conflict with C++ properties get `_0` suffix. Rename them back:

```python
# rename_animbp_variables.py
import unreal

VARIABLE_RENAMES = {
    "LeftHandOverlayState_0": "LeftHandOverlayState",
    "RightHandOverlayState_0": "RightHandOverlayState",
}

def main():
    animbp_path = "/Game/Path/To/YourAnimBP"
    bp = unreal.load_asset(animbp_path)

    for old_name, new_name in VARIABLE_RENAMES.items():
        success = unreal.SLFAutomationLibrary.rename_variable(bp, old_name, new_name)
        if success:
            unreal.log_warning(f"Renamed {old_name} -> {new_name}")

    unreal.SLFAutomationLibrary.compile_and_save(bp)

if __name__ == "__main__":
    main()
```

### Phase 5: Export State Before Clearing

Always export AnimGraph state before making changes:

```python
# export_animbp_state.py
import unreal

def main():
    animbp_path = "/Game/Path/To/YourAnimBP"
    bp = unreal.load_asset(animbp_path)

    # Export to JSON for comparison
    state = unreal.SLFAutomationLibrary.export_anim_graph_state(
        bp, "C:/scripts/migration_cache/animbp_before.json"
    )

if __name__ == "__main__":
    main()
```

### Phase 6: Clear Redundant Functions

```python
# clear_animbp_functions.py
import unreal

CLEAR_FUNCTIONS = [
    "BlueprintThreadSafeUpdateAnimation",
    "GetLocationData",
    "GetRotationData",
    "GetAccelerationData",
    "GetVelocityData",
    "GetOverlayStates",
    "GetIsBlocking",
    "GetIsResting",
    "GetIsCrouched",
    "GetBlockSequenceForWeapon",
    "GetIkHitReactData",
    "GetCharacterMovementComponent",
    "GetEquipmentComponent",
    "GetCombatComponent",
    "GetActionComponent",
    "GetGrantedTags",
]

def main():
    animbp_path = "/Game/Path/To/YourAnimBP"
    bp = unreal.load_asset(animbp_path)

    current_funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)

    for func_name in CLEAR_FUNCTIONS:
        if func_name in current_funcs:
            success = unreal.SLFAutomationLibrary.remove_function(bp, func_name)
            if success:
                unreal.log_warning(f"Cleared: {func_name}")

    unreal.SLFAutomationLibrary.compile_and_save(bp)

if __name__ == "__main__":
    main()
```

### Phase 7: Validate AnimGraph Intact

```python
# validate_animbp.py
import unreal
import json

def main():
    animbp_path = "/Game/Path/To/YourAnimBP"
    bp = unreal.load_asset(animbp_path)

    # Export after state
    unreal.SLFAutomationLibrary.export_anim_graph_state(
        bp, "C:/scripts/migration_cache/animbp_after.json"
    )

    # Compare node counts
    with open("C:/scripts/migration_cache/animbp_before.json") as f:
        before = json.load(f)
    with open("C:/scripts/migration_cache/animbp_after.json") as f:
        after = json.load(f)

    before_nodes = 0
    after_nodes = 0

    for graph in before.get("Graphs", []):
        if graph.get("GraphName") == "AnimGraph":
            before_nodes = len(graph.get("Nodes", []))

    for graph in after.get("Graphs", []):
        if graph.get("GraphName") == "AnimGraph":
            after_nodes = len(graph.get("Nodes", []))

    if before_nodes == after_nodes:
        unreal.log_warning(f"[OK] AnimGraph intact: {before_nodes} nodes")
    else:
        unreal.log_error(f"[ERROR] AnimGraph changed: {before_nodes} -> {after_nodes}")

    # Check compile status
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if "Errors: 0" in errors:
        unreal.log_warning("[OK] No compile errors")
    else:
        unreal.log_error(errors)

if __name__ == "__main__":
    main()
```

---

## Common Issues and Solutions

### Issue: BlendListByEnum Not Responding to Overlay State Changes

**Cause:** BlendListByEnum nodes read from Blueprint variables with `_0` suffix, but C++ sets properties without suffix.

**Solution:** Rename Blueprint variables to match C++ property names:
```python
unreal.SLFAutomationLibrary.rename_variable(bp, "LeftHandOverlayState_0", "LeftHandOverlayState")
```

This updates all K2Node_VariableGet references automatically.

### Issue: Variables Not Being Set

**Cause:** NativeUpdateAnimation not finding components.

**Solution:** Cache component references and add null checks:
```cpp
if (!CombatManager)
{
    CombatManager = OwnerCharacter->FindComponentByClass<UAC_CombatManager>();
}
if (CombatManager)
{
    bIsBlocking = CombatManager->IsGuarding;
}
```

### Issue: Compile Errors After Clearing Functions

**Cause:** AnimGraph still references cleared functions.

**Solution:** Only clear helper functions that SET variables, not functions that are called by AnimGraph nodes. The AnimGraph reads variables via K2Node_VariableGet, not function calls.

---

## Verification Checklist

1. [ ] C++ AnimInstance has all needed UPROPERTY variables
2. [ ] NativeUpdateAnimation sets all variables from components
3. [ ] AnimBP reparented to C++ class
4. [ ] Blueprint variables renamed to match C++ properties (no `_0` suffix)
5. [ ] AnimGraph state exported before clearing
6. [ ] Redundant functions cleared (keep only AnimGraph)
7. [ ] AnimGraph state exported after clearing
8. [ ] Node counts match (before == after)
9. [ ] Compile status: 0 errors, 0 warnings
10. [ ] PIE test: animations play correctly

---

## Files Reference

| File | Purpose |
|------|---------|
| `ABP_SoulslikeCharacter_Additive.h` | C++ AnimInstance header |
| `ABP_SoulslikeCharacter_Additive.cpp` | NativeUpdateAnimation implementation |
| `reparent_animbp.py` | Reparent AnimBP to C++ |
| `rename_animbp_variables.py` | Rename `_0` suffix variables |
| `clear_animbp_functions.py` | Clear redundant Blueprint functions |
| `test_animbp_native.py` | Validate migration |

---

## Backup Location

`C:/scripts/SLFConversion/backups/animbp_native_complete/`

**See:** `C:/scripts/SLFConversion/backups/BACKUPS.md` for full index with restore commands.

Contains:
- `ABP_SoulslikeCharacter_Additive.uasset` - Migrated AnimBP
- `ABP_SoulslikeCharacter_Additive.h` - C++ header
- `ABP_SoulslikeCharacter_Additive.cpp` - C++ implementation
- Migration scripts
