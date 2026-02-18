# Animation Migration Pipeline

End-to-end pipeline for importing Elden Ring character animations into UE5.

---

## Overview

5-phase pipeline: Extract -> Import -> Setup -> Reskin -> Test

All scripts located in `C:\scripts\elden_ring_tools\`.

---

## Phase 1: Extract (Blender Headless)

Extract animations and mesh from game archives.

| Script | Purpose |
|--------|---------|
| `extract_animations.py` | ANIBND -> FBX + TAE JSON |
| `extract_character_mesh.py` | CHRBND/FLVER -> FBX mesh |
| `tae_parser.py` | Standalone TAE parser |
| `export_skeleton_fbx.py` | Export reference skeleton FBX |

### Key Commands
```bash
# Extract animations
blender --background --python extract_animations.py -- --character c3100

# Extract mesh
blender --background --python extract_character_mesh.py -- --character c3100
```

---

## Phase 2: Import (C++ Automation - NOT Python)

Import FBX files into UE5 project using C++ console commands.

**Full runbook with commands and sample output: `.claude/reference/animation-import-runbook.md`**

| Command | Purpose |
|---------|---------|
| `SLF.ImportMesh` | Import mesh FBX -> SkeletalMesh + Skeleton (with scale) |
| `SLF.ImportAnims` | Batch import animation FBX -> AnimSequence (with scale) |
| `SLF.CheckAnim` | Validate DataModel bone data + compression |

### Two-Step Import Process
1. **Import mesh** at 100x scale -> creates cm-scale skeleton + mesh
2. **Import animations** at 100x scale targeting skeleton from step 1

### UE5 5.7 Import Gotchas
- Animation FBX must include a mesh or Interchange says "no data"
- Raw extract FBX has minimal 1-triangle mesh (OK for anim import, NOT for mesh import)
- Use real mesh FBX (from reskin pipeline) for Step 1 mesh import
- Scale=100 converts meter-scale Blender FBX to cm-scale UE5
- DDC compression: must call FinishAllCompilation() BEFORE SavePackage()
- Skeleton must be explicitly saved to disk (auto-created skeleton is in-memory only)

---

## Phase 3: Setup (C++ Automation + UE5 Scripts)

Configure AnimBP, abilities, boss Blueprint.

| Script/Function | Purpose |
|--------|---------|
| `SetupBossAnimGraph()` | StateMachine -> Slot -> Output |
| `ConfigureBlendSpace1DSamples()` | Idle/Walk/Run blendspace |
| `ConfigureEnemyAbilities()` | AI abilities from DA_AI_* assets |
| `assign_mesh_to_boss.py` | Compatible Skeletons + boss BP |

---

## Phase 4: Reskin (Blender Headless)

Re-skin custom mesh to target skeleton.

| Script | Purpose |
|--------|---------|
| `reskin_pipeline.py` | Master orchestrator |
| `prepare_reskin_blend.py` | Create working .blend |
| `reskin_mesh.py` | Auto-weights + corrections |
| `validate_reskin.py` | Deformation metrics + renders |

### Critical: FLVER Transfer > Auto-Weights
Auto-weights cause A-pose bug (bone heat diffusion assigns body verts to finger bones).
Use `parent_with_flver_transfer()` - KD-tree weight transfer from FLVER mesh.

Visual validation is MANDATORY - deformation metrics alone miss A-pose problem.

---

## Phase 5: Test (C++ + PIE)

Validate animations work correctly in-game.

| Method | Purpose |
|--------|---------|
| `SLF.Test.SpawnEnemy` | Spawn enemy at distance |
| `SLF.Test.Screenshot` | Capture labeled screenshot |
| PIE visual inspection | Verify animations play correctly |
| Log export | Check for missing bones/notifies |

---

## Completed Character Imports

### c3100 (Crucible Knight)
- 104 bones, 112 animations, 44K verts
- **cm-scale (100x)**: `/Game/EldenRingAnimations/c3100_cm/` - 114 assets, 95 MB
  - Skeleton: `SKM_Manny_export_c3100_Skeleton`
  - DataModel confirmed: 25/30 bones moving (attack anims)
- **Reskinned (game-ready)**: `/Game/Temp/c3100_v8_meter/SKM_Manny_c3100_v8_meter`
  - Skeleton: `c3100_guard_a000_000020_idle_Skeleton`, runtime scale 76
  - Fully working: idle+walk+attacks+weapon traces
- **meter-scale (raw)**: `/Game/EldenRingAnimations/c3100_direct/` - 1x scale

### c4810 (Erdtree Avatar)
- 49 anims, 176-bone skeleton, 712 TAE notifies
- Path: `/Game/EldenRingAnimations/c4810/`
- 49/49 anims retargeted to SK_Mannequin

### c2120 (Malenia)
- 146 anims, 337-bone skeleton
- Path: `/Game/EldenRingAnimations/c2120/`
- Mesh re-skinned via reskin pipeline

---

## Blender .blend Review Files

Build .blend files with correct mesh + animation for visual review in Blender.

**Script:** `C:\scripts\elden_ring_tools\build_proper_blend.py`

```bash
blender --background --python build_proper_blend.py
```

**Output:** `C:\scripts\elden_ring_tools\output\c3100_proper\`
- `c3100_idle.blend`, `c3100_walk.blend`, `c3100_attack.blend`
- 15 rendered preview PNGs (5 frames per animation)

### How It Works
1. Build HKX armature in **Blender space** (see Coordinate Space section below)
2. Load FLVER mesh with Y/Z swap (game Y-up → Blender Z-up)
3. Skin mesh to armature via vertex groups (FLVER bone names match HKX)
4. Import animations via `import_animation_to_action()` (already Blender space)
5. Strip root motion FCurves so character stays centered
6. Save per-animation .blend files + render preview frames

### To Add a New Character
Edit `CHAR_ID`, `WANTED_ANIMS`, `ANIM_LABELS` at the top of `build_proper_blend.py`.

---

## Coordinate Space: The BONE_CoB Chain Rule (CRITICAL)

### The Problem
`import_animation_to_action()` converts game-space animation data to Blender space via
`_game_bone_transform_to_bl_bone_matrix()`, which applies Y/Z swap + BONE_CoB matrix.
Then `_get_basis_matrix()` uses `bone.matrix_local` (armature rest pose) to compute
pose_bone.matrix_basis. **Both must be in the same coordinate space.**

### The Trap
**NEVER chain Blender-space bone local transforms that have BONE_CoB appended.**

```
CoB(A) @ CoB(B) != CoB(A @ B)
```

The BONE_CoB matrix between parent and child doesn't cancel out. Chaining
CoB-converted locals produces wrong armature-space transforms.

### The Correct Approach (Proven Working)

```
1. Chain bone locals in GAME SPACE → per-bone game world transform
2. Decompose each bone's game world transform to T, R, S
3. Convert each bone's WORLD transform to Blender space:
   - Y/Z swap on translation: (x, y, z) → (x, z, y)
   - Y/Z swap on rotation matrix: swap rows 1&2, cols 1&2
   - Y/Z swap on scale: (x, y, z) → (x, z, y)
   - Assemble: Translation @ Rotation.to_4x4() (with scale on diagonal)
   - Append: @ BONE_CoB
4. Set eb.matrix to the Blender-space world transform
```

This is implemented in both:
- `extract_animations.py` → `create_armature_from_skeleton_hkx()`
- `build_proper_blend.py` → `build_hkx_armature_blender_space()`

### Coordinate Conventions

| Space | Up | Forward | Handedness |
|-------|-----|---------|------------|
| FromSoft game | Y | X | Left |
| Blender | Z | -Y | Right |

**Y/Z swap:** `(x, y, z)game → (x, z, y)blender`

**BONE_CoB matrix:** Additional per-bone orientation change:
```
[[0, 1,  0, 0],
 [1, 0,  0, 0],
 [0, 0, -1, 0],
 [0, 0,  0, 1]]
```

### FLVER Mesh Coordinate Conversion
Mesh vertices use the same Y/Z swap (no BONE_CoB):
```python
bl_positions = np.column_stack([positions[:, 0], positions[:, 2], positions[:, 1]])
```

---

## Critical Notes

### FLVER vs HKX Bind Pose Mismatch
FLVER (mesh) and HKX (animation) store slightly different rest poses.
**Solution:** Use animation FBX armature as skeleton, NOT FLVER armature.
Bone POSITIONS are identical, but rest ROTATIONS differ for eye/jaw/shield bones.

### FBX Axis Convention (Must Match Across Scripts)
```python
axis_forward='-Y', axis_up='Z', primary_bone_axis='Y', secondary_bone_axis='X'
```

### Elden Ring Character IDs
- c2120 = Malenia (NOT Malgareth)
- c4810 = Erdtree Avatar (NOT Malenia)
- Source: `unpack_er_archives.py` line 323
