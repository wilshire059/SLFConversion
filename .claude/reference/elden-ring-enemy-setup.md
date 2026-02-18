# Elden Ring Enemy Setup Guide

Complete step-by-step reference for importing an Elden Ring character and setting it up as a working Soulslike enemy in UE5. Based on the c3100 (Godrick Soldier) guard enemy implementation, February 2026.

---

## Table of Contents

1. [Overview & Architecture](#1-overview--architecture)
2. [Prerequisites](#2-prerequisites)
3. [Phase 1: Extract Animations (Blender)](#3-phase-1-extract-animations-blender)
4. [Phase 2: Import to UE5 (Commandlet)](#4-phase-2-import-to-ue5-commandlet)
5. [Phase 3: SetupCommandlet - All Steps Explained](#5-phase-3-setupcommandlet---all-steps-explained)
6. [Phase 4: Runtime C++ Enemy Class](#6-phase-4-runtime-c-enemy-class)
7. [Critical Bugs & Workarounds](#7-critical-bugs--workarounds)
8. [TAE (Time Act Events) - Hitbox Timing](#8-tae-time-act-events---hitbox-timing)
9. [Damage & Combat System Integration](#9-damage--combat-system-integration)
10. [Checklist for New Enemies](#10-checklist-for-new-enemies)
11. [File Reference](#11-file-reference)

---

## 1. Overview & Architecture

### What We Built
A fully functional Soulslike enemy using an Elden Ring character (c3100 Godrick Soldier) with:
- FLVER mesh with built-in weapon geometry
- 112 animations imported from HKX via FBX
- 17 montages (4 attack, 3 reaction, 3 death, 4 dodge, 3 locomotion)
- AnimBP (duplicated from ABP_SoulslikeEnemy, retargeted to c3100 skeleton)
- 4 AI abilities with TAE-accurate hitbox timing
- Poise, hit reactions, death, dodge, locomotion
- Bleed status effect on attacks

### Architecture Summary
```
SetupC3100Commandlet (editor-time, run once)
  ├── Step 0:  DDC compression + root lock on all 112 AnimSequences
  ├── Step 1:  Add sockets (weapon_start, weapon_end, hand_r/l, foot_l/r)
  ├── Step 2:  Create 17 montages from AnimSequences
  ├── Step 3:  Create 2D BlendSpace (unused at runtime, but required by AnimBP)
  ├── Step 4:  Duplicate AnimBP + replace anim references + remove IK/ControlRig
  ├── Step 5:  Create PDA data assets (CombatReaction, PoiseBreak)
  ├── Step 6:  Create AI Ability data assets (4 attacks)
  ├── Step 6b: Verify montage lengths
  ├── Step 6c: Add ANS_WeaponTrace notifies to attack montages (TAE timings)
  ├── Step 7:  AnimBP diagnostic dump
  ├── Steps 8-9b: Verification & diagnostics
  └── All assets saved to /Game/EldenRingAnimations/c3100_guard/

ASLFEnemyGuard (runtime, per-instance)
  ├── BeginPlay → ApplyC3100Config()
  │   ├── Set FLVER mesh (scale 1.0, offset -90 Z)
  │   ├── Remove weapon child actor (mesh has built-in weapon)
  │   ├── Set combat data assets (reaction + poise break)
  │   ├── Configure 4 AI abilities
  │   ├── Cache AnimBP class (applied in Tick due to deferred reinit)
  │   └── Cache locomotion montages (idle/walk/run)
  └── Tick
      ├── Re-apply AnimBP until it sticks (Bug #8: SetSkeletalMeshAsset deferred reinit)
      ├── Refresh AI state machine's cached AnimInstance
      ├── Fix montage durations (Bug #13: CalculateSequenceLength doesn't set)
      └── Montage-based locomotion (Bug: BlendSpacePlayer bone collapse)
```

---

## 2. Prerequisites

### Required Tools
- **Blender 5.0+** with `extract_animations.py` script
- **UE5.7** with SLFConversion project compiled
- **Elden Ring** game files (ANIBND, CHRBND, FLVER)
- **tae_parser.py** for TAE JSON export

### Required UE5 C++ Classes
- `SLFAutomationLibrary` - All automation functions
- `SetupC3100Commandlet` - Editor-time asset setup (template for new enemies)
- `SLFEnemyGuard` - Runtime enemy class (template for new enemies)
- `SLFAnimNotifyStateWeaponTrace` - Hitbox detection via sphere traces
- `AICombatManagerComponent` - Damage dealing/receiving, poise, abilities
- `SLFAIStateMachineComponent` - AI behavior (combat, patrol, idle)

### Required Assets in Backup
- Source AnimBP: `/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy`
- Source BlendSpace: `/Game/SoulslikeFramework/Demo/_Animations/Locomotion/Blendspaces/ABS_SoulslikeEnemy`
- Guard Blueprint: `/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard`
- Parent Enemy Blueprint: `/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy`

---

## 3. Phase 1: Extract Animations (Blender)

### Step 1: Extract animations from ANIBND
```bash
cd C:\scripts\elden_ring_tools
python extract_animations.py c3100
```
**Output:** `output/c3100/c3100/fbx/` containing one FBX per animation (e.g., `a000_003000.fbx`)

### Step 2: Export TAE data
```bash
python tae_parser.py c3100
```
**Output:** `output/c3100/c3100/tae/c3100_tae.json` - JSON with all animation events

### Step 3: Import FLVER mesh (if needed)
The FLVER mesh is the character's 3D model from Elden Ring. Import via Blender using FLVER import addon, then export as FBX.

### Key Rules
- Use FBX from `extract_animations.py` output, NOT reskinned versions
- FBX axis convention: `axis_forward='-Y', axis_up='Z', primary_bone_axis='Y', secondary_bone_axis='X'`
- Animation FBX MUST include mesh (Interchange needs it for import)
- GC every 5 animations in Blender to prevent crash

---

## 4. Phase 2: Import to UE5 (Commandlet)

### Import animations via SLF.ImportAnims
```bash
UnrealEditor-Cmd.exe SLFConversion.uproject ^
  -ExecCmds="SLF.ImportAnims C:/scripts/elden_ring_tools/output/c3100/c3100/fbx /Game/EldenRingAnimations/c3100_guard/Animations" ^
  -unattended -nosplash
```

This calls `USLFAutomationLibrary::BatchImportAnimationsFromFBX()` which:
1. Imports each FBX file using Interchange
2. Creates UAnimSequence per animation
3. Auto-creates USkeleton (saved separately - Bug #12)
4. Runs DDC compression (`CacheDerivedDataForCurrentPlatform` + `FinishAllCompilation`)
5. Saves all packages

### Import FLVER mesh
```bash
UnrealEditor-Cmd.exe SLFConversion.uproject ^
  -ExecCmds="SLF.ImportMesh C:/path/to/c3100_mesh.fbx /Game/EldenRingAnimations/c3100_guard/c3100_mesh" ^
  -unattended -nosplash
```

### CRITICAL: DDC Compression (Bug #11)
After import, ALL AnimSequences MUST have DDC compression applied:
```cpp
AnimSeq->CacheDerivedDataForCurrentPlatform();
// ... do all sequences ...
FAssetCompilingManager::Get().FinishAllCompilation();  // WAIT for async
// ... then SavePackage() each one
```
Without this, `GetBoneTransform()` returns identity (A-pose) at runtime.

---

## 5. Phase 3: SetupCommandlet - All Steps Explained

Run via: `UnrealEditor-Cmd.exe SLFConversion.uproject -run=SetupC3100 -unattended -nosplash`

### Step 0: DDC Re-compression + Root Lock

**What:** Iterates all 112 AnimSequences, sets `bForceRootLock = true` with `ERootMotionRootLock::AnimFirstFrame`, then re-runs DDC compression and saves.

**Why Root Lock:**
Elden Ring animations have root/Master bone translation baked in. Without root lock, the mesh visually drifts during animation then snaps back to start position when the animation loops. Root lock prevents this.

**CRITICAL: Use `AnimFirstFrame`, NOT `RefPose`!**
- `RefPose` = lock to skeleton reference pose → BREAKS EVERYTHING (ref pose is meter-scale 1x, animations are cm-scale 100x). Entire skeleton collapses to 1/100 size.
- `AnimFirstFrame` = lock to frame 0 position → CORRECT (frame 0 has the correct cm-scale root transform, preventing drift without losing scale)

```cpp
AnimSeq->bForceRootLock = true;
AnimSeq->RootMotionRootLock = ERootMotionRootLock::AnimFirstFrame;
AnimSeq->CacheDerivedDataForCurrentPlatform();
// ... FinishAllCompilation() ... SavePackage()
```

### Step 1: Add Sockets

**What:** Adds sockets to the skeleton for weapon traces and attachment points.

**Sockets added:**
| Socket | Bone | Offset | Purpose |
|--------|------|--------|---------|
| weapon_start | R_Sword | (0, 0, 50) | Weapon trace start (blade tip) |
| weapon_end | R_Sword | (0, 0, -50) | Weapon trace end (hilt) |
| hand_r | R_Hand | (0,0,0) | Right hand attachment |
| hand_l | L_Hand | (0,0,0) | Left hand attachment |
| foot_l | L_Foot | (0,0,0) | Left foot |
| foot_r | R_Foot | (0,0,0) | Right foot |

**Key:** Socket offsets are in bone-local space. At mesh scale 1.0, ±50 = ±50 world units = 100-unit weapon trace length. This is correct for meter-scale animations.

### Step 2: Create Montages

**What:** Creates 17 UAnimMontage assets from AnimSequences using `CreateMontageFromSequence()`.

**Montage categories:**
- **Attack (4):** AM_c3100_Attack01 (a000_003000), Attack02 (a000_003001), Attack03_Fast (a000_003017), HeavyAttack (a000_004100)
- **Reaction (3):** AM_c3100_HitReact (a000_010000), HitReact_Light (a000_010001), GuardHit (a000_008030)
- **Death (3):** AM_c3100_Death_Front (a000_011010), Death_Back (a000_011060), Death_Left (a000_011070)
- **Dodge (4):** AM_c3100_Dodge_Fwd/Bwd/Left/Right (a000_005000-005003)
- **Locomotion (3):** AM_c3100_Idle (a000_000020), Walk (a000_002000), Run (a000_002100)

All montages use `DefaultSlot` and have a single `Default` composite section.

### Step 3: Create 2D BlendSpace

**What:** Creates `BS_c3100_Locomotion` - a 2D BlendSpace with Direction on X (-180 to 180) and Speed on Y (0 to 600).

**Samples:** Idle at (0, 0), Walk at (0, 200), Run at (0, 450).

**Note:** This BlendSpace exists because the AnimBP references it, but at runtime the BlendSpacePlayer evaluation has a known bug (bone collapse to meter-scale). Locomotion is driven by montages instead (see Bug section).

### Step 4: Duplicate AnimBP

**What:** Duplicates `ABP_SoulslikeEnemy` to `ABP_c3100_Guard`, retargeted to the c3100 skeleton.

**Sub-steps:**
- **4a:** `DuplicateAnimBPForSkeleton()` - copies AnimBP, changes target skeleton, compiles
- **4b:** `ReplaceAnimReferencesInAnimBP()` - remaps blend space and idle animation references to c3100 versions. Without this, AnimGraph nodes still reference the source Mannequin animations (Bug #14).
- **4c:** Remove Control Rig and IK Rig nodes from AnimGraph (they reference Mannequin-specific rigs that don't exist for c3100)

### Step 5: Create Data Assets

**What:** Creates `PDA_c3100_CombatReaction` and `PDA_c3100_PoiseBreak` data assets.

**CombatReaction** contains hit reaction montage references.
**PoiseBreak** contains poise break start/loop montage references.

### Step 6: Create AI Ability Data Assets

**What:** Creates 4 `UPDA_AIAbility` data assets, each pointing to an attack montage.

| Asset | Montage | Weight | Max Distance |
|-------|---------|--------|-------------|
| DA_c3100_Attack01 | AM_c3100_Attack01 | 1.0 | 2.0 |
| DA_c3100_Attack02 | AM_c3100_Attack02 | 1.0 | 2.0 |
| DA_c3100_Attack03_Fast | AM_c3100_Attack03_Fast | 1.5 | 1.0 |
| DA_c3100_HeavyAttack | AM_c3100_HeavyAttack | 0.5 | 5.0 |

### Step 6b: Verify Montage Lengths

**What:** Reloads all montages and verifies `GetPlayLength() > 0`. Catches Bug #13 (CalculateSequenceLength returns but doesn't set).

### Step 6c: Add Weapon Trace Notifies (TAE Timings)

**What:** Adds `USLFAnimNotifyStateWeaponTrace` notify states to attack montages at times derived from Elden Ring TAE data.

**Hitbox timings (widened for UE5 variable framerate):**

| Montage | TAE Original | UE5 Widened | Radius | Notes |
|---------|-------------|-------------|--------|-------|
| Attack01 | 1.267-1.367s | 1.167-1.517s | 40 | Standard swing |
| Attack02 | 0.800-0.933s | 0.700-1.083s | 40 | Fast combo hit |
| Attack03_Fast | 0.800-0.933s | 0.700-1.083s | 40 | Fast attack |
| HeavyAttack | ~2.3-2.6s | 2.200-2.750s | 80 | AoE slam, larger radius |

**Why widen:** Elden Ring runs at locked 30fps with 3-4 frame hitbox windows (100-133ms). UE5 variable framerate means NotifyTick may only fire 1-2 times in that window. Widening to 350-550ms ensures reliable hit detection. Also compensates for knockback pushing the player slightly back after the first hit in combos.

**Idempotent:** `AddWeaponTraceToMontage()` removes existing weapon trace notifies before adding new ones, so rerunning the commandlet doesn't stack duplicates.

### Steps 7-9b: Diagnostics

Verification steps that log AnimBP state, skeleton bone counts, animation references, and bone transform scale checks. Useful for debugging but not required for setup.

---

## 6. Phase 4: Runtime C++ Enemy Class

### ASLFEnemyGuard - Key Patterns

#### Pattern 1: Deferred AnimBP Application (Bug #8)
`SetSkeletalMeshAsset()` triggers a DEFERRED animation reinit that clears the AnimInstance on the next tick. You can't set the AnimBP in the same frame as the mesh.

**Solution:** Cache the AnimBP class in `ApplyC3100Config()`, then in `Tick()` check if the AnimInstance exists. If not, re-apply it. It typically takes ~6 ticks to stick.

```cpp
// In ApplyC3100Config():
C3100AnimBPClass = LoadClass<UAnimInstance>(nullptr, *(Dir + TEXT("ABP_c3100_Guard.ABP_c3100_Guard_C")));

// In Tick():
if (C3100AnimBPClass && TicksAfterBeginPlay > 5) {
    UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
    if (!AnimInst || !AnimInst->IsA(C3100AnimBPClass)) {
        MeshComp->SetAnimInstanceClass(C3100AnimBPClass);
        MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
        MeshComp->InitAnim(true);
        // ... mark as applied, refresh AI state machine cache
    }
}
```

#### Pattern 2: Refresh AI State Machine Cache
`USLFAIStateMachineComponent` caches the AnimInstance during its BeginPlay. After we replace the AnimInstance (Pattern 1), the cached reference is stale.

```cpp
if (USLFAIStateMachineComponent* AISMComp = FindComponentByClass<USLFAIStateMachineComponent>()) {
    AISMComp->RefreshCachedAnimInstance();
}
```

#### Pattern 3: Montage-Based Locomotion
BlendSpacePlayer evaluation collapses bones to ~1/100 scale (see Bug section). We bypass it entirely by playing locomotion as looping montages on DefaultSlot.

```cpp
// In Tick(), after AnimBP applied:
float Speed = GetCharacterMovement()->Velocity.Size2D();
UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();
bool bIsLocomotionMontage = (CurrentMontage == WalkMontage || CurrentMontage == RunMontage || CurrentMontage == IdleMontage);
bool bIsAttackOrOther = (CurrentMontage != nullptr && !bIsLocomotionMontage);

if (!bIsAttackOrOther) {
    UAnimMontage* DesiredMontage = nullptr;
    if (Speed > 300) DesiredMontage = RunMontage;
    else if (Speed > 30) DesiredMontage = WalkMontage;
    else DesiredMontage = IdleMontage;

    if (DesiredMontage != ActiveLocomotionMontage) {
        AnimInst->Montage_Stop(0.2f, ActiveLocomotionMontage);
        AnimInst->Montage_Play(DesiredMontage, 1.0f);
        AnimInst->Montage_SetNextSection(FName("Default"), FName("Default"), DesiredMontage); // Loop
        ActiveLocomotionMontage = DesiredMontage;
    }
} else {
    ActiveLocomotionMontage = nullptr; // Clear tracking during attacks
}
```

**Key:** `Montage_SetNextSection("Default", "Default", Montage)` makes the montage loop its single section. Attack montages played by the AI interrupt the locomotion montage; when the attack ends, Tick re-evaluates and starts the appropriate locomotion montage.

#### Pattern 4: Remove Weapon Child Actor
If the FLVER mesh has a weapon built into the geometry, destroy the Blueprint SCS weapon:

```cpp
TArray<UChildActorComponent*> ChildActorComps;
GetComponents<UChildActorComponent>(ChildActorComps);
for (UChildActorComponent* CAC : ChildActorComps) {
    if (CAC && CAC->GetName().Contains(TEXT("Weapon"))) {
        CAC->DestroyChildActor();
        CAC->DestroyComponent();
    }
}
```

#### Pattern 5: Mesh Setup
```cpp
MeshComp->SetSkeletalMeshAsset(C3100Mesh);
MeshComp->SetRelativeScale3D(FVector(1.0f));          // NEVER change this
MeshComp->SetRelativeLocation(FVector(0, 0, -90.0f)); // Center in capsule
MeshComp->SetBoundsScale(200.0f);                     // Large bounds for cm-scale anim
MeshComp->bEnableUpdateRateOptimizations = false;      // Always evaluate
MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
```

**Scale MUST be 1.0.** The animations are in cm-scale (skeleton height ~253 units). At scale 1.0 this looks correct. DO NOT change scale to fix visibility - the issue was in the animation evaluation path, not scale.

---

## 7. Critical Bugs & Workarounds

### Bug A: BlendSpacePlayer Bone Collapse (THE BIG ONE)

**Symptom:** Mesh visible during idle (SequencePlayer) and attacks (Montage), but invisible/collapsed during walking/running (BlendSpacePlayer). Skeleton height drops from ~253 to ~2.5 units.

**Root Cause:** Unknown UE5 bug where BlendSpacePlayer evaluation produces bone transforms at ~1/100 scale compared to SequencePlayer and Montage evaluation paths. The animation DATA is correct (cm-scale in DataModel), but the runtime BlendSpacePlayer evaluator produces meter-scale results.

**Investigated and ruled out:**
- Pin wiring (confirmed correct via exhaustive dump)
- BlendSpace1D vs 2D (both exhibit the same issue)
- DDC compression (re-compression didn't fix it)
- Scale settings (the data is correct, evaluation is wrong)

**Workaround:** Drive ALL locomotion through montages (idle/walk/run montages on DefaultSlot). Montage evaluation produces correct cm-scale bone transforms. The BlendSpace exists in the AnimBP but is never reached because a locomotion montage is always playing on the Slot node.

**Time spent:** ~8 hours debugging across 2 sessions.

### Bug B: Root Motion Snap-Back

**Symptom:** Character teleports back to start position when animation loops or changes.

**Root Cause:** Elden Ring animations have root/Master bone translation baked in. UE5 plays this as regular bone animation (not root motion), so the mesh visually drifts. On loop, the root bone resets to frame 0 → snap.

**Fix:** `bForceRootLock = true` with `ERootMotionRootLock::AnimFirstFrame` on all AnimSequences. This locks the root bone to frame 0's position (correct cm-scale), preventing drift.

**NEVER use `ERootMotionRootLock::RefPose`** - the reference pose is meter-scale (1x), but animations encode cm-scale (100x) in the root bone. RefPose collapses the entire skeleton.

### Bug C: SetSkeletalMeshAsset Deferred Reinit (Bug #8)

**Symptom:** AnimBP set in BeginPlay doesn't stick. AnimInstance is NULL on next tick.

**Root Cause:** `SetSkeletalMeshAsset()` triggers a deferred anim reinit that clears the AnimInstance.

**Fix:** Apply AnimBP in Tick after a few frames, keep retrying until it sticks.

### Bug D: Stale AI State Machine Cache

**Symptom:** AI doesn't play attack montages after AnimBP replacement.

**Root Cause:** `USLFAIStateMachineComponent` caches the AnimInstance during its own BeginPlay. After we replace the AnimInstance, it holds a stale pointer.

**Fix:** Call `AISMComp->RefreshCachedAnimInstance()` after successfully applying the new AnimBP.

### Bug E: Montage Duration Zero (Bug #13)

**Symptom:** Montages have GetPlayLength() = 0 at runtime, attacks don't play.

**Root Cause:** `UAnimMontage::CalculateSequenceLength()` RETURNS the length but doesn't SET it. PostLoad DataModel can also overwrite SequenceLength.

**Fix:** Runtime patch in `FixMontageDurations()` that recalculates and force-sets via FProperty pointer.

---

## 8. TAE (Time Act Events) - Hitbox Timing

### What is TAE?
TAE (Time Act Events) is Elden Ring's animation event system. It defines when hitboxes activate, when sounds play, when I-frames apply, etc. Each animation has a list of events with start/end times and parameters.

### TAE JSON Location
`C:\scripts\elden_ring_tools\output\{charId}\{charId}\tae\{charId}_tae.json`

### Key Event Types for Combat

| Type ID | Name | UE5 Equivalent | Purpose |
|---------|------|----------------|---------|
| 1 | AttackBehavior | ANS_WeaponTrace | Hitbox activation window |
| 0 (flag 5) | ParryPossible | ANS_ParryWindow | Player can parry |
| 0 (flag 8) | Invincible | ANS_InvincibilityFrame | I-frames (dodge) |
| 0 (flag 24) | HyperArmor | ANS_HyperArmor | Poise active |
| 795 | Toughness | ANS_HyperArmor | Parametric poise |
| 96 | PlayFFX | AN_PlayNiagara | Visual effects |
| 128-132 | PlaySound | AN_PlaySound | Audio |

### How to Extract Hitbox Timings

```python
import json
with open('c3100_tae.json') as f:
    data = json.load(f)

for anim_key in ['3000', '3001', '3017', '4100']:
    anim = data['animations'].get(anim_key, {})
    events = anim if isinstance(anim, list) else anim.get('events', [])
    for evt in events:
        if evt.get('type_id') == 1:  # AttackBehavior = hitbox
            print(f"Anim {anim_key}: hitbox {evt['start_time']:.3f}s - {evt['end_time']:.3f}s")
            print(f"  behavior_judge_id: {evt['params']['behavior_judge_id']}")
```

### Widening for UE5
Elden Ring TAE windows are 3-4 frames at 30fps (100-133ms). For UE5 variable framerate:
- **Start 0.1s earlier** (catch wind-up phase)
- **End 0.15s later** (catch follow-through)
- **Increase trace radius** by ~33% (40 instead of 30)

This prevents missed hits in combos where knockback pushes the target slightly back.

---

## 9. Damage & Combat System Integration

### How Damage Flows (Guard Attacks Player)

```
AI State Machine selects ability → CombatManager plays attack montage
    → ANS_WeaponTrace.NotifyBegin() clears HitActors
    → ANS_WeaponTrace.NotifyTick() each frame:
        1. Get socket positions (weapon_start, weapon_end)
        2. SphereTraceMultiForObjects (Pawn, WorldDynamic, Destructible)
        3. For each hit actor (skip duplicates):
           a. Find target's UAC_CombatManager (player) or UAICombatManagerComponent (AI)
           b. Call HandleIncomingWeaponDamage(Owner, Damage=50, PoiseDamage=25, HitResult)
           c. Apply status effects from DefaultAttackStatusEffects (e.g., Bleed)
    → ANS_WeaponTrace.NotifyEnd() clears HitActors
```

### Default Damage Values
- **Physical damage:** 50.0 (10% of typical 500 HP)
- **Poise damage:** 25.0
- **Status effects:** Configured on `AICombatManagerComponent::DefaultAttackStatusEffects`

### Socket Requirements for Weapon Traces
- `weapon_start` and `weapon_end` sockets on the skeleton
- If missing, falls back to: `hand_r` → `weapon_r` → `RightHand` → character forward 150 units
- Sockets added to skeleton in Step 1 of the commandlet

### How Player Damages Guard
Player's weapon traces use `UAC_EquipmentManager` for damage values and hit against the guard's `UAICombatManagerComponent::HandleIncomingWeaponDamage_AI()`. The poise system determines whether the guard staggers or absorbs the hit.

---

## 10. Checklist for New Enemies

### Copy-Paste Template

For a new enemy (e.g., c2120 Malenia), copy and adapt:

1. **[ ] Extract animations:** `python extract_animations.py c2120`
2. **[ ] Export TAE:** `python tae_parser.py c2120`
3. **[ ] Import mesh FBX to UE5** via `SLF.ImportMesh`
4. **[ ] Import animation FBXs** via `SLF.ImportAnims`
5. **[ ] Create commandlet** - copy `SetupC3100Commandlet.cpp/h`, rename to `SetupC2120Commandlet`
6. **[ ] Update commandlet paths** - change all `/c3100_guard/` to `/c2120/`
7. **[ ] Update animation IDs** - change a000_003000 etc. to c2120's attack animation IDs
8. **[ ] Extract TAE hitbox timings** for the new character's attacks
9. **[ ] Update weapon trace timings** in Step 6c with new TAE data (widen for UE5)
10. **[ ] Update socket bones** - check which bone has the weapon (may not be R_Sword)
11. **[ ] Create C++ enemy class** - copy `SLFEnemyGuard.h/cpp`, adapt paths
12. **[ ] Register commandlet** in Build.cs if needed
13. **[ ] Run commandlet** - `UnrealEditor-Cmd.exe -run=SetupC2120`
14. **[ ] Blueprint setup** - create or reparent Blueprint to new C++ class
15. **[ ] PIE test** - spawn enemy, verify:
    - [ ] Mesh visible at all times (idle, walk, run, attack)
    - [ ] No snap-back on animation loop
    - [ ] Attacks deal damage (check ANS_WeaponTrace logs)
    - [ ] Hit reactions play
    - [ ] Poise break works
    - [ ] Death animation plays
    - [ ] Status effects apply
    - [ ] Walking/running visible (not collapsed)

### Things That Will Bite You
1. **bForceRootLock must be AnimFirstFrame, NEVER RefPose** (see Bug B)
2. **SetSkeletalMeshAsset nulls AnimInstance** - apply AnimBP in Tick (see Bug C)
3. **BlendSpacePlayer bone collapse** - use montage locomotion (see Bug A)
4. **DDC compression is async** - FinishAllCompilation before SavePackage
5. **Montage CalculateSequenceLength doesn't set** - must call SetCompositeLength
6. **AnimBP duplication doesn't remap anim refs** - call ReplaceAnimReferencesInAnimBP
7. **Skeleton package must be saved separately** after mesh import
8. **TAE hitbox windows are too tight for UE5** - widen by ~0.25s total

---

## 11. File Reference

### Source Files
| File | Purpose |
|------|---------|
| `Source/SLFConversion/SetupC3100Commandlet.cpp` | Editor-time asset setup (THE TEMPLATE) |
| `Source/SLFConversion/SetupC3100Commandlet.h` | Commandlet header |
| `Source/SLFConversion/Blueprints/SLFEnemyGuard.cpp` | Runtime enemy class |
| `Source/SLFConversion/Blueprints/SLFEnemyGuard.h` | Runtime enemy header |
| `Source/SLFConversion/SLFAutomationLibrary.cpp` | All automation functions |
| `Source/SLFConversion/Animation/SLFAnimNotifyStateWeaponTrace.cpp` | Hitbox detection |
| `Source/SLFConversion/Components/AICombatManagerComponent.cpp` | Damage/poise system |

### Asset Paths (c3100 Guard)
| Asset | Path |
|-------|------|
| FLVER Mesh | `/Game/EldenRingAnimations/c3100_guard/c3100_mesh` |
| Skeleton | `/Game/EldenRingAnimations/c3100_guard/c3100_mesh_Skeleton` |
| Animations (112) | `/Game/EldenRingAnimations/c3100_guard/Animations/a000_*` |
| AnimBP | `/Game/EldenRingAnimations/c3100_guard/ABP_c3100_Guard` |
| BlendSpace | `/Game/EldenRingAnimations/c3100_guard/BS_c3100_Locomotion` |
| Attack Montages | `/Game/EldenRingAnimations/c3100_guard/AM_c3100_Attack*` |
| Reaction Data | `/Game/EldenRingAnimations/c3100_guard/PDA_c3100_CombatReaction` |
| Poise Data | `/Game/EldenRingAnimations/c3100_guard/PDA_c3100_PoiseBreak` |
| AI Abilities | `/Game/EldenRingAnimations/c3100_guard/DA_c3100_Attack*` |

### Script Paths
| Script | Purpose |
|--------|---------|
| `C:\scripts\elden_ring_tools\extract_animations.py` | Extract HKX → FBX |
| `C:\scripts\elden_ring_tools\tae_parser.py` | Export TAE → JSON |
| `C:\scripts\elden_ring_tools\build_proper_blend.py` | Build .blend for visual review |

### TAE Data
| File | Content |
|------|---------|
| `C:\scripts\elden_ring_tools\output\c3100\c3100\tae\c3100_tae.json` | All 128 c3100 animations with events |
| `C:\scripts\slfconversion\TAE_QUICK_REFERENCE.md` | TAE event type reference |
| `C:\scripts\slfconversion\TAE_FORMAT_ANALYSIS.md` | Binary format documentation |
