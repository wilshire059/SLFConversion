---
name: animBP-fix
description: Fix Animation Blueprint issues after migration. Use when AnimBP variables aren't being set, animations not playing, or Property Access nodes returning wrong values. Triggers on "AnimBP", "animation not working", "animation blueprint fix", "Property Access". (project)
---

# AnimBP Fix - Post-Migration Animation Blueprint Troubleshooting

## WHEN TO USE THIS SKILL

Use this skill when:
- Animation not playing despite state changes (e.g., crouch animation not playing)
- AnimBP Property Access nodes returning NULL or wrong values
- AnimBP variables not being updated from C++
- AnimBP EventGraph was cleared during migration but AnimGraph still needs data

---

## ROOT CAUSE: Why AnimBP Breaks After Migration

During Blueprint-to-C++ migration, the AnimBP's **EventGraph is cleared** but the **AnimGraph is preserved**. This causes issues because:

1. **EventGraph used to set component references** - AnimBPs often have variables like `ActionManager`, `CombatManager` that were set in EventGraph
2. **Property Access nodes read from these references** - AnimGraph uses paths like `ActionManager.IsCrouched`
3. **After clearing, references are NULL** - The AnimGraph reads from NULL and gets default values
4. **AnimBP NOT reparented to C++** - Some AnimBPs (with Animation Layer Interfaces) can't be reparented, so NativeUpdateAnimation doesn't run

---

## DIAGNOSIS STEPS

### Step 1: Identify What AnimGraph Reads
Check the AnimBP's Property Access nodes in the AnimGraph:
```
Look for paths like:
- ActionManager.IsCrouched
- CombatManager.IsDead
- StatManager.CurrentHealth
```

### Step 2: Check If Reference Is NULL
Add debug logging to the owning character:
```cpp
if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
{
    FProperty* Prop = AnimInst->GetClass()->FindPropertyByName(TEXT("ActionManager"));
    if (Prop)
    {
        void* Ptr = Prop->ContainerPtrToValuePtr<void>(AnimInst);
        UObject** ObjPtr = static_cast<UObject**>(Ptr);
        UE_LOG(LogTemp, Warning, TEXT("AnimBP ActionManager: %p"), ObjPtr ? *ObjPtr : nullptr);
    }
}
```

### Step 3: Verify C++ Component Is Valid
```cpp
UE_LOG(LogTemp, Warning, TEXT("CachedActionManager: %p"), CachedActionManager);
```

---

## FIX PATTERNS

### Pattern 1: Initialize AnimBP References in OnStartCrouch/Similar Events

The AnimInstance may not be available in BeginPlay. Initialize references when first needed:

```cpp
void AMyCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

    // Initialize AnimBP's component reference if NULL
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
        {
            FObjectProperty* Prop = CastField<FObjectProperty>(
                AnimInst->GetClass()->FindPropertyByName(TEXT("ActionManager")));
            if (Prop && CachedActionManager)
            {
                UObject* CurrentValue = Prop->GetObjectPropertyValue_InContainer(AnimInst);
                if (!CurrentValue)
                {
                    // Initialize the reference
                    Prop->SetObjectPropertyValue_InContainer(AnimInst, CachedActionManager);
                    UE_LOG(LogTemp, Warning, TEXT("Initialized AnimBP ActionManager to %p"), CachedActionManager);
                }
            }
        }
    }

    // Then set the actual property the AnimGraph reads
    if (CachedActionManager)
    {
        CachedActionManager->IsCrouched = true;
    }
}
```

### Pattern 2: Update AnimBP Variables Every Frame via Tick

If the AnimBP can't be reparented to C++ (due to Animation Layer Interfaces), update variables in Tick:

```cpp
void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update AnimBP variables every frame (replaces cleared EventGraph logic)
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
        {
            // Set bool property
            if (FBoolProperty* Prop = CastField<FBoolProperty>(
                AnimInstance->GetClass()->FindPropertyByName(TEXT("IsCrouched"))))
            {
                Prop->SetPropertyValue_InContainer(AnimInstance, bIsCrouched);
            }

            // Set float property
            if (FFloatProperty* SpeedProp = CastField<FFloatProperty>(
                AnimInstance->GetClass()->FindPropertyByName(TEXT("Speed"))))
            {
                SpeedProp->SetPropertyValue_InContainer(AnimInstance, GetVelocity().Size());
            }
        }
    }
}
```

### Pattern 3: Direct Component Property Access

When the AnimGraph reads nested properties like `ActionManager.IsCrouched`:

```cpp
// In your component (e.g., AC_ActionManager)
UPROPERTY(BlueprintReadWrite, Category = "State")
bool IsCrouched = false;

// In your character
if (CachedActionManager)
{
    CachedActionManager->IsCrouched = bIsCrouched;
}
```

---

## COMMON ANIMGRAPH PROPERTY ACCESS PATHS

| Path | Component | Property | Where to Set |
|------|-----------|----------|--------------|
| `ActionManager.IsCrouched` | AC_ActionManager | IsCrouched | OnStartCrouch/OnEndCrouch |
| `ActionManager.IsSprinting` | AC_ActionManager | IsSprinting | HandleSprintStarted/Completed |
| `CombatManager.IsDead` | AC_CombatManager | IsDead | OnDeath |
| `StatManager.CurrentHealth` | AC_StatManager | CurrentHealth | OnStatChanged |

---

## WHY BEGINPLAY DOESN'T WORK

The AnimInstance may not be created yet when BeginPlay runs:
1. BeginPlay runs during actor initialization
2. SkeletalMeshComponent may not have initialized its AnimInstance yet
3. `GetAnimInstance()` returns NULL

**Solution:** Use lazy initialization - set references when first needed (e.g., in OnStartCrouch).

---

## DEBUGGING CHECKLIST

1. [ ] Check if AnimInstance exists: `GetMesh()->GetAnimInstance() != nullptr`
2. [ ] Check if property exists on AnimBP: `FindPropertyByName(TEXT("PropertyName"))`
3. [ ] Check if component reference is set: Log the pointer value
4. [ ] Check AnimGraph for Property Access paths
5. [ ] Verify C++ component has the property being accessed
6. [ ] Add logging at every step to trace the issue

---

## EXAMPLE: Full Crouch Animation Fix

```cpp
// Header
virtual void Tick(float DeltaTime) override;
virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

// Implementation
void AMyCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

    // 1. Set component property (what AnimGraph reads via Property Access)
    if (CachedActionManager)
    {
        CachedActionManager->IsCrouched = true;
    }

    // 2. Initialize AnimBP reference if needed
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
        {
            FObjectProperty* AMProp = CastField<FObjectProperty>(
                AnimInst->GetClass()->FindPropertyByName(TEXT("ActionManager")));
            if (AMProp && CachedActionManager)
            {
                UObject* CurrentValue = AMProp->GetObjectPropertyValue_InContainer(AnimInst);
                if (!CurrentValue)
                {
                    AMProp->SetObjectPropertyValue_InContainer(AnimInst, CachedActionManager);
                }
            }
        }
    }
}
```

---

## RELATED FILES

- `Source/SLFConversion/Blueprints/SLFSoulslikeCharacter.cpp` - Example of crouch fix
- `Source/SLFConversion/Components/AC_ActionManager.h` - IsCrouched property definition
- AnimBP exports in `Exports/BlueprintDNA_v2/` - Check EventGraph for original logic
