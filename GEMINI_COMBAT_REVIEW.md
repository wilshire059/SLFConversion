# Combat System Investigation - Peer Review Request

## Status: FIXES APPLIED (2026-01-14)

All critical issues identified below have been fixed:
- ✅ TraceTypes initialized with ECC_Pawn in both collision managers
- ✅ Knockback added to HandleHitReaction for living characters (both AC_CombatManager and AICombatManagerComponent)
- ⏭️ TargetMesh issue skipped (CollisionManagerComponent used in production has correct logic)

## Context
We're investigating why weapon swings don't always register hits on enemies/characters, and why knockback seems incorrect in the C++ migration compared to the original bp_only Blueprint project.

## CRITICAL FINDINGS

### Issue 1: TraceTypes Array is EMPTY (CRITICAL BUG)

**File:** `AC_CollisionManager.cpp` (Lines 16-32) and `CollisionManagerComponent.cpp` (Lines 19-36)

Both collision manager implementations have the **same critical bug**: the `TraceTypes` array is never initialized.

**C++ Implementation (AC_CollisionManager.cpp:16-32):**
```cpp
UAC_CollisionManager::UAC_CollisionManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;

    // Initialize defaults
    TraceSocketStart = FName("WeaponTraceStart");
    TraceSocketEnd = FName("WeaponTraceEnd");
    TraceRadius = 10.0;
    TargetMesh = nullptr;
    TraceDebugMode = EDrawDebugTrace::None;
    DamageMultiplier = 1.0;
    TraceSizeMultiplier = 1.0;
    // ... no TraceTypes initialization!
}
```

**Same issue in CollisionManagerComponent.cpp:19-36:**
```cpp
UCollisionManagerComponent::UCollisionManagerComponent()
{
    // Initialize config variables
    TraceSocketStart = FName("TraceStart");
    TraceSocketEnd = FName("TraceEnd");
    TraceRadius = 20.0;
    TraceDebugMode = EDrawDebugTrace::None;
    // ... no TraceTypes initialization!
}
```

**The problem:** `SphereTraceMultiForObjects` at line 140 uses `TraceTypes`:
```cpp
bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
    this,
    NewStartPosition,
    NewEndPosition,
    TraceRadius,
    TraceTypes,  // EMPTY ARRAY - WILL NEVER HIT ANYTHING!
    false,
    ActorsToIgnore,
    TraceDebugMode,
    HitResults,
    true
);
```

**Impact:** With an empty `TraceTypes` array, `SphereTraceMultiForObjects` will return **zero hits** because there are no object types to trace against. This means **ALL weapon attacks are completely broken**.

**Original Blueprint:** The Blueprint CDO had TraceTypes set to include `EObjectTypeQuery::Pawn` (and possibly others) to detect character pawns.

**Fix Required:**
```cpp
// In constructor - add Pawn object type
TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
// Or via EObjectTypeQuery directly:
TraceTypes.Add(EObjectTypeQuery::ObjectTypeQuery3); // Pawn
```

---

### Issue 2: TargetMesh Gets Character Mesh, Not Weapon Mesh

**File:** `AC_CollisionManager.cpp` (Lines 34-54)

```cpp
void UAC_CollisionManager::BeginPlay()
{
    AActor* Owner = GetOwner();
    if (IsValid(Owner))
    {
        // WRONG: Gets CHARACTER's mesh, not WEAPON's mesh!
        ACharacter* Character = Cast<ACharacter>(Owner);
        if (IsValid(Character))
        {
            TargetMesh = Character->GetMesh();  // This is wrong
        }
        else
        {
            TargetMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
        }
    }
}
```

**The problem:** The CollisionManager component is attached to the **WEAPON actor**, but BeginPlay tries to get the mesh from a Character. Since the weapon is the owner (not the character), it should get the weapon's own mesh component.

**Impact:** When `GetTraceLocations` runs:
```cpp
OutTraceStartLocation = MeshComp->GetSocketLocation(TraceSocketStart);
OutTraceEndLocation = MeshComp->GetSocketLocation(TraceSocketEnd);
```

If TargetMesh is the wrong mesh (or nullptr), socket locations will be wrong or zero vectors.

**Fix Required:**
```cpp
void UAC_CollisionManager::BeginPlay()
{
    Super::BeginPlay();

    // Get the WEAPON's mesh component (this component is on the weapon actor)
    AActor* Owner = GetOwner();
    if (IsValid(Owner))
    {
        // First try static mesh (common for weapons)
        TargetMesh = Owner->FindComponentByClass<UStaticMeshComponent>();

        // Fall back to skeletal mesh if no static mesh
        if (!TargetMesh)
        {
            TargetMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
        }
    }
}
```

---

### Issue 3: No Knockback Applied to Living Characters

**File:** `AC_CombatManager.cpp` (Lines 720-761)

**HandleHitReaction_Implementation** only does:
1. Sets `IKWeight = 1.0` (for procedural IK hit response)
2. Plays camera shake
3. Plays hit reaction montage

**What's MISSING:** No physical knockback/launch force is applied!

**C++ Implementation (no knockback):**
```cpp
void UAC_CombatManager::HandleHitReaction_Implementation(const FHitResult& HitInfo)
{
    // Activate IK hit react
    IKWeight = 1.0;
    CurrentHitNormal = HitInfo.ImpactNormal;

    // Play camera shake
    if (IsValid(GenericReactionShake))
    {
        UGameplayStatics::PlayWorldCameraShake(...);
    }

    // Play hit reaction animation
    if (IsValid(ReactionAnimset) && IsValid(Mesh))
    {
        // ... play montage
    }
    // NO KNOCKBACK APPLIED!
}
```

**HandleDeath (where knockback IS applied - line 796):**
```cpp
if (RagdollOnDeath && IsValid(Mesh))
{
    Mesh->SetSimulatePhysics(true);
    FVector Impulse = GetKnockbackAmountForDamage(HitInfo, 500.0, 1000.0);
    Mesh->AddImpulse(Impulse, NAME_None, true);  // Only on death!
}
```

**Original Blueprint Behavior (Expected):**
Living characters should receive knockback via `LaunchCharacter()` based on hit direction and damage amount.

**Fix Required:**
```cpp
void UAC_CombatManager::HandleHitReaction_Implementation(const FHitResult& HitInfo)
{
    AActor* Owner = GetOwner();

    // Apply knockback to character
    if (ACharacter* Character = Cast<ACharacter>(Owner))
    {
        FVector Knockback = GetKnockbackAmountForDamage(HitInfo, 100.0, 500.0);
        Character->LaunchCharacter(Knockback, false, false);
    }

    // ... rest of existing code (IK, camera shake, montage)
}
```

---

## Summary Table

| Issue | Severity | File | Line | Impact |
|-------|----------|------|------|--------|
| **Empty TraceTypes** | CRITICAL | AC_CollisionManager.cpp | 16-32 | NO hits detected at all |
| **Empty TraceTypes** | CRITICAL | CollisionManagerComponent.cpp | 19-36 | NO hits detected at all |
| **Wrong TargetMesh** | HIGH | AC_CollisionManager.cpp | 44-48 | Wrong socket positions |
| **No knockback on living chars** | MEDIUM | AC_CombatManager.cpp | 720-761 | No stagger/pushback |

---

## Validation Questions for Gemini

1. **TraceTypes:** What EObjectTypeQuery values should be in the array? Is it just Pawn, or also WorldDynamic/PhysicsBody?

2. **TargetMesh:** The weapon actor owns the CollisionManager - should we get the weapon's mesh, or should we traverse to the attached character and use their skeletal mesh?

3. **Knockback:** Should knockback be applied via:
   - `LaunchCharacter()` for controlled movement?
   - `AddImpulse()` on the mesh component?
   - Root motion via the hit reaction montage?

4. **Socket Names:** What are the expected socket names on weapon meshes?
   - `WeaponTraceStart` / `WeaponTraceEnd` (current default)
   - `TraceStart` / `TraceEnd` (CollisionManagerComponent default)
   - Something else?

---

## Files to Fix

| File | Changes Needed |
|------|---------------|
| `AC_CollisionManager.cpp` | Initialize TraceTypes with ECC_Pawn |
| `CollisionManagerComponent.cpp` | Initialize TraceTypes with ECC_Pawn |
| `AC_CollisionManager.cpp` | Fix BeginPlay to get weapon mesh, not character mesh |
| `AC_CombatManager.cpp` | Add LaunchCharacter knockback in HandleHitReaction |

---

## Recommended Fixes

### Fix 1: Initialize TraceTypes (CRITICAL)

```cpp
// In AC_CollisionManager constructor
UAC_CollisionManager::UAC_CollisionManager()
{
    // ... existing init ...

    // Initialize trace types to detect Pawns (characters)
    TraceTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
}
```

### Fix 2: Fix TargetMesh Assignment

```cpp
void UAC_CollisionManager::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (IsValid(Owner))
    {
        // This component is on a WEAPON actor - get weapon's mesh
        TargetMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
        if (!TargetMesh)
        {
            TargetMesh = Owner->FindComponentByClass<UStaticMeshComponent>();
        }

        UE_LOG(LogTemp, Log, TEXT("[CollisionManager] TargetMesh: %s"),
            TargetMesh ? *TargetMesh->GetName() : TEXT("NULL"));
    }
}
```

### Fix 3: Add Knockback to HandleHitReaction

```cpp
void UAC_CombatManager::HandleHitReaction_Implementation(const FHitResult& HitInfo)
{
    UE_LOG(LogTemp, Log, TEXT("UAC_CombatManager::HandleHitReaction"));

    AActor* Owner = GetOwner();
    if (!IsValid(Owner))
    {
        return;
    }

    // Apply knockback to living characters
    if (ACharacter* Character = Cast<ACharacter>(Owner))
    {
        // Calculate knockback based on hit (same as death knockback but smaller)
        FVector Knockback = GetKnockbackAmountForDamage(HitInfo, 100.0, 300.0);

        // Only apply if poise is broken or no hyper armor
        if (PoiseBroken || !HyperArmor)
        {
            Character->LaunchCharacter(Knockback, false, false);
            UE_LOG(LogTemp, Log, TEXT("  Applied knockback: %s"), *Knockback.ToString());
        }
    }

    // ... existing IK, camera shake, montage code ...
}
```

Please review and provide feedback on this analysis.
