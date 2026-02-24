---
name: custom-enemy-pipeline
description: End-to-end pipeline for creating a forensically distinct custom enemy from an AI-generated mesh zip + Elden Ring animations. Covers mesh prep, ARP retarget, forensic transforms, UE5 commandlet setup, and runtime C++ class. Triggers on "custom enemy", "new enemy", "AI mesh", "enemy pipeline", "forensic enemy", "Meshy". (project)
---

# Custom Enemy Pipeline — AI Mesh + Elden Ring Animations

## PURPOSE

Create a fully functional Soulslike enemy from:
1. An **AI-generated mesh** (Meshy AI zip file with FBX + textures)
2. **Elden Ring character animations** (extracted via Soulstruct)

The enemy uses forensically distinct animations (13 transform layers), a legally clean mesh, and standard UE5 bone naming. No FromSoft assets ship in the final game.

---

## PREREQUISITES

| Tool | Version | Purpose |
|------|---------|---------|
| Blender | 5.0+ | Mesh rigging, ARP retarget, FBX export |
| Auto-Rig Pro (ARP) | Latest | Automatic rigging + animation retargeting |
| Soulstruct | Latest | Elden Ring ANIBND/CHRBND extraction |
| UE5 | 5.7+ | Game engine |
| SLFConversion C++ | Compiled | `SLFAutomationLibrary`, commandlets, enemy classes |

### Required Elden Ring Files
```
C:/Program Files (x86)/Steam/steamapps/common/ELDEN RING/Game/chr/
  cXXXX.anibnd.dcx    # Animation bundle
  cXXXX.chrbnd.dcx    # Character bundle (mesh + skeleton)
```

### Character ID Reference
| ID | Character | Animations | Notes |
|----|-----------|-----------|-------|
| c3100 | Crucible Knight / Guard | 112 | Standard humanoid, sword+shield |
| c2120 | Malenia | 146 | Dual swords, complex combos |
| c4810 | Erdtree Avatar | 49 | Large boss, unique skeleton |

---

## PHASE 1: PREPARE AI MESH

### 1a. Download and Extract

```bash
# Meshy AI exports a zip with FBX + texture PNGs
unzip "C:/Users/james/Downloads/Meshy_AI_<NAME>_texture_fbx.zip" \
  -d "C:/scripts/elden_ring_tools/output/<enemy_name>/mesh_raw"
```

Output structure:
```
output/<enemy_name>/mesh_raw/
  model.fbx           # AI-generated mesh
  texture_diffuse.png  # Color map
  texture_normal.png   # Normal map (optional)
  texture_metallic.png # PBR maps (optional)
```

### 1b. Rig with Auto-Rig Pro in Blender

Open Blender (GUI required for initial ARP setup):

1. **Import FBX**: File → Import → FBX (`model.fbx`)
2. **Scale to human proportions**: The mesh should be ~1.7-1.8m tall in Blender units
3. **Auto-Rig Pro → Smart**: Places bone markers on the mesh
4. **Adjust markers**: Ensure hips, spine, neck, head, shoulders, elbows, wrists, knees, ankles are correct
5. **Match to Rig**: Generates the ARP rig with IK/FK chains
6. **Skin → Voxelize**: Auto-weights the mesh to the rig
7. **Save as**: `C:/scripts/elden_ring_tools/output/<enemy_name>/final/SKM_<EnemyName>_bound.blend`

### 1c. Verify Deform Bones

The ARP rig creates ~339 bones total, but only ~28 are deform bones. These map to our custom naming:

```python
# ARP deform bone → Custom bone name (forensically distinct from FromSoft)
ARP_TO_CUSTOM = {
    "root.x":             "Hips",
    "spine_01.x":         "SpineLower",
    "spine_02.x":         "SpineUpper",
    "neck.x":             "Neck",
    "head.x":             "Head",
    "shoulder.l":         "Clavicle_L",
    "arm_stretch.l":      "UpperArm_L",
    "forearm_stretch.l":  "LowerArm_L",
    "hand.l":             "Hand_L",
    "shoulder.r":         "Clavicle_R",
    "arm_stretch.r":      "UpperArm_R",
    "forearm_stretch.r":  "LowerArm_R",
    "hand.r":             "Hand_R",
    "thigh_stretch.l":    "Thigh_L",
    "leg_stretch.l":      "Calf_L",
    "foot.l":             "Foot_L",
    "toes_01.l":          "Toe_L",
    "thigh_stretch.r":    "Thigh_R",
    "leg_stretch.r":      "Calf_R",
    "foot.r":             "Foot_R",
    "toes_01.r":          "Toe_R",
    # Twist bones
    "thigh_twist.l":      "ThighTwist_L",
    "thigh_twist.r":      "ThighTwist_R",
    "leg_twist.l":        "CalfTwist_L",
    "leg_twist.r":        "CalfTwist_R",
    "forearm_twist.l":    "ForearmTwist_L",
    "forearm_twist.r":    "ForearmTwist_R",
    "c_arm_twist_offset.l": "UpperArmTwist_L",
    "c_arm_twist_offset.r": "UpperArmTwist_R",
}
```

**CRITICAL**: Use `_L`/`_R` suffix (UE5 convention), NOT `L_`/`R_` prefix (FromSoft convention).

---

## PHASE 2: EXTRACT ELDEN RING ANIMATIONS

### 2a. Run Extraction Script

```bash
"C:/Program Files/Blender Foundation/Blender 5.0/blender.exe" --background \
  --python C:/scripts/elden_ring_tools/extract_animations.py -- \
  --game-dir "C:/Program Files (x86)/Steam/steamapps/common/ELDEN RING/Game" \
  --characters cXXXX \
  --output C:/scripts/elden_ring_tools/output/cXXXX \
  --skip-existing
```

Output:
```
output/cXXXX/cXXXX/
  fbx/
    a000_000020.fbx    # Each animation as FBX (armature + mesh + action)
    a000_002000.fbx
    ... (all animations)
  tae/
    a000_000020.json   # TAE events (hitbox windows, sounds, effects)
    a000_002000.json
    ...
```

### 2b. Select Animation Subset

Choose which animations to retarget. Standard humanoid enemy moveset (20 animations):

```python
# Source FBX name → Output name (NO FromSoft naming)
ANIM_MAP = {
    # Locomotion
    "a000_000020": "<Enemy>_Idle",
    "a000_002000": "<Enemy>_Walk",
    "a000_002100": "<Enemy>_Run",
    # Attacks
    "a000_003000": "<Enemy>_Attack01",
    "a000_003001": "<Enemy>_Attack02",
    "a000_003017": "<Enemy>_Attack03_Fast",
    "a000_004100": "<Enemy>_HeavyAttack",
    # Reactions
    "a000_010000": "<Enemy>_HitReact",
    "a000_010001": "<Enemy>_HitReact_Light",
    "a000_008030": "<Enemy>_GuardHit",
    # Deaths
    "a000_011010": "<Enemy>_Death_Front",
    "a000_011060": "<Enemy>_Death_Back",
    "a000_011070": "<Enemy>_Death_Left",
    # Dodges
    "a000_005000": "<Enemy>_Dodge_Fwd",
    "a000_005001": "<Enemy>_Dodge_Bwd",
    "a000_005002": "<Enemy>_Dodge_Left",
    "a000_005003": "<Enemy>_Dodge_Right",
    # Combat
    "a000_007000": "<Enemy>_Guard",
    "a000_009210": "<Enemy>_PoiseBreak_Start",
    "a000_009200": "<Enemy>_PoiseBreak_Loop",
}
```

These IDs are for c3100 (Crucible Knight). Other characters use different IDs — check the TAE JSON files for animation purposes.

---

## PHASE 3: ARP RETARGET + FORENSIC TRANSFORMS

### 3a. Create Retarget Script

Copy `mesh_animation_pipeline.py` as template, update these values:

```python
# === CHANGE THESE FOR EACH ENEMY ===
ENEMY_NAME = "<EnemyName>"  # e.g., "Warlord", "Sentinel"
BLEND_FILE = "C:/scripts/elden_ring_tools/output/<enemy_name>/final/SKM_<EnemyName>_bound.blend"
SOURCE_ANIM_DIR = "C:/scripts/elden_ring_tools/output/cXXXX/cXXXX/fbx"
OUTPUT_DIR = "C:/scripts/elden_ring_tools/output/<enemy_name>/final"
ANIM_MAP = { ... }  # From Phase 2b
```

### 3b. FBX Import/Export Settings

```python
FBX_IMPORT = dict(
    use_anim=True, ignore_leaf_bones=True,
    automatic_bone_orientation=False,
    primary_bone_axis='Y', secondary_bone_axis='X',
    axis_forward='-Y', axis_up='Z',
)

FBX_EXPORT = dict(
    use_selection=True, global_scale=1.0,
    apply_unit_scale=True, apply_scale_options='FBX_SCALE_ALL',
    axis_forward='-Y', axis_up='Z',
    use_mesh_modifiers=True, mesh_smooth_type='OFF',
    add_leaf_bones=False,
    primary_bone_axis='Y', secondary_bone_axis='X',
    use_armature_deform_only=True,  # Only deform bones (~28, not ~339)
    bake_anim=True,
    bake_anim_use_all_bones=True,
    bake_anim_use_nla_strips=False,
    bake_anim_use_all_actions=False,
    bake_anim_force_startend_keying=True,
    bake_anim_simplify_factor=0.0,
)
```

### 3c. Retarget Pipeline (per animation)

For each animation FBX:
1. Import FBX (brings FLVER armature + action)
2. ARP Remap: source=FLVER armature, target=ARP rig
3. `strip_spurious_translation()` — ARP injects world-space pivots on rotation-only bones
4. Apply forensic transforms (see 3d)
5. Resample 30fps → 24fps
6. Export FBX (target armature + mesh + baked action)
7. Cleanup imported objects, GC

### 3d. Forensic Transform Stack (13 layers, applied in order)

All transforms are in `mesh_animation_pipeline.py` `phase3_forensic_transforms()`. Each changes exported data while remaining visually imperceptible:

| # | Transform | Function | Effect |
|---|-----------|----------|--------|
| 1 | Static rotation offset | `apply_static_offset()` | 2-5 deg constant bias on 10 bones (spine, neck, head, shoulders, pelvis) |
| 8b | Per-bone phase shift | `apply_phase_shift()` | Shift each bone's keyframes by 0-2 frames. Deterministic per bone name hash. Root/pelvis/IK excluded. |
| 2 | Per-bone smooth noise | `apply_noise()` | Up to 2.5 deg Catmull-Rom sine noise, bone-weighted (see BONE_NOISE_WEIGHTS) |
| 2b | Low-pass Butterworth filter | `apply_lowpass_filter()` | 2nd-order 8Hz cutoff on all rotation channels. Destroys high-frequency source fingerprint. scipy or moving-average fallback. |
| 3 | Time warp | `apply_timewarp()` | 15% non-linear sine distortion: `t' = t + 0.15*sin(2*pi*t)/(2*pi)` |
| 3b | Direction/anim swap | `OUTPUT_SWAP_PAIRS` | Swap source FBX for output: Dodge_Left↔Right, HitReact↔HitReact_Light |
| 4 | Per-animation speed | `apply_speed_variation()` | 0.88x-1.12x per animation, SLERP resampled |
| 5 | 24fps resample | `apply_resample()` | 30fps→24fps with SLERP quaternion interpolation |
| 5b | Keyframe decimation | `apply_keyframe_decimation()` | Remove keyframes where SLERP error < 0.15 deg. Typically 30-50% reduction on slow bones. |
| 6 | Bone pruning | `prune_bones()` | 91→68 bones (23 cosmetic bones removed) |
| 7 | Rest pose perturbation | `perturb_rest_pose()` | 12 bones rotated 1-3 deg in edit mode |
| 7b | Bone length scaling | `perturb_rest_pose()` | 12 bones: torso +3%, neck -4%, arms ±2-3%, legs ±2% |
| 8 | Full bone rename | `apply_bone_rename()` | FLVER→UE5 Mannequin names (68 bones) |

#### Bone Noise Weights (FLVER bone names)
```python
BONE_NOISE_WEIGHTS = {
    'Master': 0.0, 'Root': 0.0,
    'L_Foot_Target': 0.0, 'R_Foot_Target': 0.0,
    'Pelvis': 0.15,
    'Spine': 1.0, 'Spine1': 0.9, 'Spine2': 0.8,
    'Neck': 0.7, 'Head': 0.6,
    'L_UpperArm': 0.7, 'R_UpperArm': 0.7,
    'L_Forearm': 0.8, 'R_Forearm': 0.8,
    'L_UpperLeg': 0.4, 'R_UpperLeg': 0.4,
    'L_LowerLeg': 0.5, 'R_LowerLeg': 0.5,
    'L_Foot': 0.2, 'R_Foot': 0.2,
    # Finger/twist bones: 0.1-0.3
}
```

#### Bone Length Scaling
```python
BONE_LENGTH_SCALE = {
    'Spine': 1.03, 'Spine1': 1.03, 'Spine2': 1.03,
    'Neck': 0.96,
    'L_UpperArm': 0.97, 'R_UpperArm': 0.97,
    'L_Forearm': 1.02, 'R_Forearm': 1.02,
    'L_Thigh': 1.02, 'R_Thigh': 1.02,
    'L_Calf': 0.98, 'R_Calf': 0.98,
}
```

#### Static Rotation Offsets (degrees)
```python
BONE_OFFSETS_DEG = {
    'Spine': (0.0, 5.0, 2.0), 'Spine1': (0.0, 3.0, 1.0),
    'Spine2': (2.0, 2.0, 0.0), 'Neck': (0.0, -3.0, 0.0),
    'Head': (-2.0, -2.0, 0.0),
    'L_Clavicle': (0.0, 0.0, 3.0), 'R_Clavicle': (0.0, 0.0, -3.0),
    'L_UpperArm': (0.0, 0.0, 4.0), 'R_UpperArm': (0.0, 0.0, -4.0),
    'Pelvis': (0.0, 2.0, 1.0),
}
```

**NOTE**: All bone names use FLVER convention (pre-rename). The rename to UE5 Mannequin names happens as transform #8.

### 3e. Run Pipeline (Phases 1-3)

```bash
# All phases (mesh prep + ARP retarget + forensic transforms):
"C:/Program Files/Blender Foundation/Blender 5.0/blender.exe" --background \
  --python C:/scripts/elden_ring_tools/mesh_animation_pipeline.py -- --all

# Phase 3 only (re-apply forensic transforms to existing retargets):
"C:/Program Files/Blender Foundation/Blender 5.0/blender.exe" --background \
  --python C:/scripts/elden_ring_tools/mesh_animation_pipeline.py -- --phase 3
```

Expected output per animation:
```
  [1/20] Sentinel_Idle
    1.Offset:10 8b.PhaseShift:59 2.Noise:82 2b.LowPass:91 3.Warp:276
    4.Speed:0.93x(113->121) 5.Resample:121->97 5b.Decimate:8827->294
    6.Prune:23(->68) 7.RestPose:12+12lengths 8.Rename:68
    -> .../phase3/final/Sentinel_Idle.fbx
```

Final output:
```
output/<enemy_name>/final/
  SKM_<EnemyName>.fbx           # Mesh-only FBX
  <EnemyName>_Idle.fbx          # 20 animation FBXs
  <EnemyName>_Walk.fbx
  <EnemyName>_Run.fbx
  ...
  SKM_<EnemyName>_bound.blend   # Original rigged .blend
  SKM_<EnemyName>_retarget.blend # Post-retarget .blend (debug)
```

### 3f. Backup Strategy

**ALWAYS back up before each forensic transform change.** Pattern:
```bash
# Before adding change N:
cp retarget_<enemy>_arp.py retarget_<enemy>_arp.py.bakN
cp -r output/<enemy>/final/ output/<enemy>/backup_with_<previous_change>/
```

---

## PHASE 4: UE5 COMMANDLET SETUP

### 4a. Create Setup Commandlet

Copy `SetupSentinelCommandlet.h/.cpp` as template. The commandlet handles all UE5 asset creation:

**Header** (`Setup<EnemyName>Commandlet.h`):
```cpp
#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "Setup<EnemyName>Commandlet.generated.h"

UCLASS()
class USetup<EnemyName>Commandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    USetup<EnemyName>Commandlet();
    virtual int32 Main(const FString& Params) override;
};
```

**9-Step Pipeline** (in `Main()`):

| Step | Action | Method |
|------|--------|--------|
| 0 | Import mesh FBX | `UAssetImportTask` + `UFbxFactory` (CRITICAL: same pipeline as animations — see MEMORY #26) |
| 0b | Sync skeleton ref pose | `UpdateReferencePoseFromMesh()` + translation retargeting (root=Animation, pelvis=AnimationScaled, all others=Skeleton) |
| 1 | Import animation FBXs | `UAssetImportTask` per animation (bImportMesh=false, SetDetectImportTypeOnImport(false)) |
| 2 | Add sockets to skeleton | `AddSocketToSkeleton()` |
| 3 | Create 17 montages | `CreateMontageFromSequence()` |
| 4 | Create 2D blend space | `CreateBlendSpace2DFromSequences()` |
| 5 | Duplicate + retarget AnimBP | `DuplicateAnimBPForSkeleton()` + `ReplaceAnimReferencesInAnimBP()` + `DisableControlRigInAnimBP()` |
| 6 | Create data assets | Manual: `UPDA_AnimData`, `UPDA_WeaponAnimset`, `UPDA_CombatReactionAnimData`, `UPDA_PoiseBreakAnimData`, `UPDA_AI_Ability` (x4) |
| 6c | Add weapon trace notifies | `AddWeaponTraceToMontage()` |
| 7 | Validation + diagnostics | `DiagnoseAnimBPDetailed()`, montage length check |
| 8 | Create Blueprint | `FKismetEditorUtilities::CreateBlueprint()` |

**CRITICAL**: Steps 0 and 1 MUST use `UAssetImportTask` (native UE5 FBX importer), NOT custom C++ functions like `ImportSkeletalMeshFromFBX` or `ImportAnimFBXDirect`. Different import code paths handle FBX PreRotation differently, causing bind pose mismatch → vertex stretching. Using the same `UAssetImportTask` pipeline for both mesh and animation ensures matching bind poses.

**CRITICAL**: Initialize `FSlateApplication::Create()` before import tasks. Do NOT use `-AllowCommandletRendering` (triggers 30+ min shader compilation). Slate creation alone is sufficient for `UAssetImportTask` to work in commandlet mode.

### 4b. Socket Configuration

```cpp
const FSocketInfo Sockets[] = {
    { TEXT("weapon_start"), TEXT("Hand_R"), FVector(0, 0, 50) },
    { TEXT("weapon_end"),   TEXT("Hand_R"), FVector(0, 0, -50) },
    { TEXT("hand_r"),       TEXT("Hand_R"),  FVector::ZeroVector },
    { TEXT("hand_l"),       TEXT("Hand_L"),  FVector::ZeroVector },
    { TEXT("foot_l"),       TEXT("Foot_L"),  FVector::ZeroVector },
    { TEXT("foot_r"),       TEXT("Foot_R"),  FVector::ZeroVector },
};
```

### 4c. Weapon Trace Timings (from TAE)

Extract hitbox windows from TAE JSON files, then widen for UE5 variable framerate:
- **Start**: original_start - 0.1s
- **End**: original_end + 0.15s
- **Radius**: increase ~33% (light=40, heavy=80)

```cpp
const FWeaponTraceConfig TraceConfigs[] = {
    { TEXT("AM_<Enemy>_Attack01"),      1.167f, 1.517f, 40.0f },
    { TEXT("AM_<Enemy>_Attack02"),      0.700f, 1.083f, 40.0f },
    { TEXT("AM_<Enemy>_Attack03_Fast"), 0.700f, 1.083f, 40.0f },
    { TEXT("AM_<Enemy>_HeavyAttack"),   2.200f, 2.750f, 80.0f },
};
```

### 4d. Register Commandlet

In `SLFConversion.Build.cs`, the commandlet is auto-discovered. Register a console command shortcut in `SLFConversion.cpp`:

```cpp
static FAutoConsoleCommand CmdSetup<Enemy>(
    TEXT("SLF.Setup<Enemy>"),
    TEXT("Run Setup<Enemy> commandlet"),
    FConsoleCommandDelegate::CreateLambda([]() { /* ... */ })
);
```

### 4e. Nuclear Clean + Run

**CRITICAL**: Delete ALL existing .uasset files (including mesh AND skeleton) before running commandlet (Bug #21 — "partially loaded" package crash). The commandlet recreates everything.

```bash
# 1. Close UE5 Editor (releases file locks)

# 2. Nuclear clean — delete ALL assets including mesh and skeleton
rm -rf "C:/scripts/SLFConversion/Content/CustomEnemies/<EnemyName>/Animations/"
rm -f "C:/scripts/SLFConversion/Content/CustomEnemies/<EnemyName>/"*.uasset
find "C:/scripts/SLFConversion/Saved/" -name "*<EnemyName>*" -delete

# 3. Run commandlet (NO -NullRHI — DDC compression needs real RHI)
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" \
  "C:/scripts/SLFConversion/SLFConversion.uproject" \
  -run=Setup<EnemyName> -forensic \
  -unattended -nosplash -stdout -UTF8Output -FullStdOutLogOutput -NoSound
```

Expected: ~49 assets created (20 anims + 17 montages + BS + ABP + 4 PDAs + 4 AI abilities + BP).

---

## PHASE 5: RUNTIME C++ ENEMY CLASS

### 5a. Create Enemy Class

Copy `SLFEnemySentinel.h/.cpp` as template. Key patterns:

**Constructor**: Create components not in Blueprint SCS
```cpp
A<EnemyName>::A<EnemyName>()
{
    if (!FindComponentByClass<UAICombatManagerComponent>())
        CreateDefaultSubobject<UAICombatManagerComponent>(TEXT("<Enemy>CombatManager"));
}
```

**BeginPlay**: Load mesh, cache AnimBP class, set capsule
```cpp
void A<EnemyName>::BeginPlay()
{
    Super::BeginPlay();

    // Capsule
    UCapsuleComponent* Capsule = GetCapsuleComponent();
    Capsule->SetCapsuleHalfHeight(90.0f);
    Capsule->SetCapsuleRadius(35.0f);

    // Mesh
    USkeletalMesh* Mesh = LoadObject<USkeletalMesh>(...);
    MeshComp->SetSkeletalMeshAsset(Mesh);
    MeshComp->SetRelativeScale3D(FVector(RUNTIME_SCALE));  // Meter→cm
    MeshComp->SetRelativeLocation(FVector(0, 0, -90.0f));

    // Cache AnimBP (apply in Tick — Bug #8: SetSkeletalMeshAsset nulls AnimInstance)
    AnimBPClass = LoadClass<UAnimInstance>(...);

    // Cache locomotion montages
    IdleMontage = LoadObject<UAnimMontage>(...);
    WalkMontage = LoadObject<UAnimMontage>(...);
    RunMontage  = LoadObject<UAnimMontage>(...);

    // Set data assets on combat manager
    CombatManagerComponent->ReactionAnimset = LoadObject<UPDA_CombatReactionAnimData>(...);
    CombatManagerComponent->PoiseBreakAsset = LoadObject<UPDA_PoiseBreakAnimData>(...);
}
```

**Tick**: Deferred AnimBP + montage-based locomotion
```cpp
void A<EnemyName>::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    TicksAfterBeginPlay++;

    // Bug #8: Apply AnimBP after deferred reinit completes (~6 ticks)
    if (!bAnimBPApplied && TicksAfterBeginPlay >= 6 && AnimBPClass)
    {
        MeshComp->SetAnimInstanceClass(AnimBPClass);
        if (MeshComp->GetAnimInstance())
        {
            bAnimBPApplied = true;
            // Start idle montage
            MeshComp->GetAnimInstance()->Montage_Play(IdleMontage, 1.0f);
            ActiveLocomotionMontage = IdleMontage;
            bMontageLocomotionActive = true;
        }
    }

    // Bug #16: BlendSpacePlayer produces 1/100 scale — use montage locomotion
    if (bMontageLocomotionActive)
    {
        float Speed = GetCharacterMovement()->Velocity.Size2D();
        UAnimMontage* Desired = (Speed > 300) ? RunMontage
                              : (Speed > 30)  ? WalkMontage
                              :                  IdleMontage;
        if (Desired != ActiveLocomotionMontage)
        {
            AnimInst->Montage_Stop(0.2f, ActiveLocomotionMontage);
            AnimInst->Montage_Play(Desired, 1.0f);
            AnimInst->Montage_SetNextSection(FName("Default"), FName("Default"), Desired);
            ActiveLocomotionMontage = Desired;
        }
    }
}
```

### 5b. Critical Runtime Bugs

| Bug | Issue | Fix |
|-----|-------|-----|
| #8 | `SetSkeletalMeshAsset()` nulls AnimInstance via deferred reinit | Apply AnimBP in Tick after 6+ ticks |
| #16 | BlendSpacePlayer produces 1/100 scale bone transforms | Drive ALL locomotion via montages |
| #17 | `ERootMotionRootLock::RefPose` collapses skeleton to 1/100 | ALWAYS use `AnimFirstFrame` |
| #13 | `CalculateSequenceLength()` returns but doesn't set | Call `SetCompositeLength(CalcLength)` |

### 5c. Runtime Scale Calculation

```
AI mesh in Blender: ~1.7m tall (1.7 Blender units)
UE5 units: 1 unit = 1 cm
Target height: ~180 cm (human)
Scale factor: 180 / 1.7 ≈ 106x (adjust per mesh)

Sentinel example: 76x scale (shorter mesh)
```

Set in BeginPlay: `MeshComp->SetRelativeScale3D(FVector(SCALE));`

---

## PHASE 6: VERIFICATION

### 6a. Animation Preview (UE5 Editor)

1. Open UE5 Editor
2. Content Browser → `/Game/CustomEnemies/<EnemyName>/Animations/`
3. Double-click any animation to open Animation Editor
4. **Check**: No A-pose, bones move correctly, no deformation artifacts

### 6b. PIE Gameplay Test

```bash
# Build C++, then:
SLF.Test.SpawnEnemy /Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_<EnemyName> 500
SLF.Test.Screenshot <Enemy>_Idle
SLF.SimAttack
SLF.Test.Screenshot <Enemy>_Combat
```

### 6c. Checklist

- [ ] Mesh visible at correct scale
- [ ] Idle animation plays (not A-pose)
- [ ] Walk/Run transitions work
- [ ] Attacks play montages with weapon traces
- [ ] Hit reactions work
- [ ] Death animations play
- [ ] AI state machine enters combat
- [ ] No FromSoft asset names in shipped content

---

## TROUBLESHOOTING

### "Partially loaded" crash on commandlet run
**Cause**: Existing .uasset files were lazy-loaded at engine startup.
**Fix**: Delete ALL .uasset files (except SKM_* mesh/skeleton) before running commandlet.

### Animations show A-pose
**Cause**: Skeleton mismatch between mesh and animations.
**Fix**: Ensure animations are imported against the mesh's own skeleton (auto-created during mesh FBX import), NOT a different skeleton.

### Locked .uasset files during nuclear clean
**Cause**: UE5 Editor has files open.
**Fix**: Close UE5 Editor before running nuclear clean. Check with `tasklist.exe | grep UnrealEditor`.

### ARP retarget "0 mapped bones"
**Cause**: Bone names don't match between source and target.
**Fix**: Rename ARP deform bones to match FLVER names BEFORE retargeting (the `ARP_TO_CUSTOM` dict).

### Posture offset "0 bones adjusted"
**Cause**: Using custom bone names instead of ARP internal names.
**Fix**: Posture offsets must use ARP internal names (`c_spine_01.x`, `c_arm_fk.l`) because retargeted actions store fcurves with those names.

### Root offset "0 fcurves shifted"
**Cause**: ARP doesn't produce location fcurves for Hips bone.
**Fix**: Create location fcurves dynamically using Blender 5.0 layered action API:
```python
if hasattr(action, 'is_action_layered') and action.is_action_layered:
    for layer in action.layers:
        for strip in layer.strips:
            for bag in strip.channelbags:
                fc = bag.fcurves.new(data_path, index=axis)
```

### Montage has zero play length
**Cause**: Bug #13 — `CalculateSequenceLength()` returns but doesn't set.
**Fix**: After creating montage, call `SetCompositeLength(CalculateSequenceLength())`.

---

## FILE REFERENCE

### Scripts
| File | Purpose |
|------|---------|
| `C:/scripts/elden_ring_tools/extract_animations.py` | Phase 2: HKX → FBX extraction |
| `C:/scripts/elden_ring_tools/mesh_animation_pipeline.py` | Phases 1-5: Mesh prep, ARP retarget, 13 forensic transforms, renders, validation (TEMPLATE) |
| `Source/SLFConversion/SetupSentinelCommandlet.cpp` | Phase 4: UE5 asset creation (TEMPLATE) |
| `Source/SLFConversion/Blueprints/SLFEnemySentinel.cpp` | Phase 5: Runtime enemy class (TEMPLATE) |
| `Source/SLFConversion/SLFAutomationLibrary.cpp` | C++ automation functions |

### Key C++ Functions (SLFAutomationLibrary)
| Function | Purpose |
|----------|---------|
| `UAssetImportTask` + `UFbxFactory` | Import mesh AND animation FBXs (native UE5 pipeline — preferred over custom functions) |
| `UpdateReferencePoseFromMesh()` | Sync skeleton ref pose with mesh bind pose (CRITICAL after mesh import) |
| `AddSocketToSkeleton()` | Add named socket to bone |
| `CreateMontageFromSequence()` | Create montage from anim sequence |
| `CreateBlendSpace2DFromSequences()` | Create 2D blend space (direction + speed) |
| `DuplicateAnimBPForSkeleton()` | Copy AnimBP and retarget skeleton |
| `ReplaceAnimReferencesInAnimBP()` | Remap anim refs in AnimGraph nodes |
| `DisableControlRigInAnimBP()` | Remove ControlRig/IK nodes |
| `AddWeaponTraceToMontage()` | Add ANS_WeaponTrace notify state |
| `DiagnoseAnimBPDetailed()` | Export AnimBP state for debugging |

### Asset Paths (per enemy)
```
/Game/CustomEnemies/<EnemyName>/
  SKM_<EnemyName>.uasset                    # Skeletal mesh
  SKM_<EnemyName>_Skeleton.uasset           # Skeleton
  ABP_<EnemyName>.uasset                    # AnimBP
  BS_<EnemyName>_Locomotion.uasset          # Blend space
  AM_<EnemyName>_*.uasset                   # 17 montages
  PDA_<EnemyName>_AnimData.uasset           # Locomotion PDA
  PDA_<EnemyName>_WeaponAnimset.uasset      # Weapon PDA
  PDA_<EnemyName>_CombatReaction.uasset     # Hit/death PDA
  PDA_<EnemyName>_PoiseBreak.uasset         # Poise break PDA
  DA_<EnemyName>_Attack*.uasset             # 4 AI abilities
  Animations/
    <EnemyName>_*.uasset                    # 20 animation sequences
```

### Blender Output (per enemy)
```
C:/scripts/elden_ring_tools/output/<enemy_name>/final/
  SKM_<EnemyName>.fbx                       # Mesh-only FBX
  <EnemyName>_Idle.fbx                      # 20 animation FBXs
  <EnemyName>_Walk.fbx
  ...
  SKM_<EnemyName>_bound.blend               # ARP-rigged mesh
  SKM_<EnemyName>_retarget.blend            # Post-retarget debug
```

---

## FORENSIC RISK ASSESSMENT

### Clean (no risk)
- AI-generated mesh (legally yours)
- Custom skeleton with UE5 standard bone names
- All asset names use your enemy name, no FromSoft references

### Low risk
- Generic animations (idle, walk, run, basic attacks) — not copyrightable
- Dodges, guard, hit reacts — standard game mechanics

### Medium risk (if using complex animations)
- Distinctive combo choreography — recognizable timing/motion patterns
- Specific heavy attack wind-ups — artistic expression in original

### Mitigations applied
- 13-layer forensic transform stack changes every exported data sample
- Different skeleton topology (ARP vs FLVER)
- Different mesh proportions warp retargeted motion
- 24fps vs 30fps source
- No FromSoft file names, bone names, or asset paths anywhere
