# Elden Ring Animation Import Runbook

End-to-end process for extracting Elden Ring character animations and importing them into UE5 at visible (cm) scale. Last verified: Feb 2026 with c3100 (Crucible Knight).

---

## Quick Reference

| Step | Tool | Time | Output |
|------|------|------|--------|
| 1. Extract | Blender headless | ~5 min/char | `output/<charID>/<charID>/fbx/*.fbx` |
| 2. Visual Review | Blender headless | ~2 min | `output/<charID>_proper/*.blend` |
| 3. Import Mesh | UE5 `SLF.ImportMesh` | ~4 sec | Skeleton + SkeletalMesh .uasset |
| 4. Import Anims | UE5 `SLF.ImportAnims` | ~40 sec/112 anims | AnimSequence .uasset per anim |
| 5. Validate | UE5 `SLF.CheckAnim` | ~4 sec | Log output |

---

## Prerequisites

### Tools
- Blender 4.x+ (headless OK)
- UE5.7 with SLFConversion project compiled
- Elden Ring unpacked archives (`C:/Program Files (x86)/Steam/steamapps/common/ELDEN RING/Game`)

### Scripts
All extraction scripts at `C:\scripts\elden_ring_tools\`:

| Script | Purpose |
|--------|---------|
| `extract_animations.py` | ANIBND -> per-animation FBX + TAE JSON |
| `build_proper_blend.py` | Build .blend files for visual review |
| `reskin_guard_v8.py` | Reskin Mannequin mesh to target skeleton |

### C++ Automation (SLFAutomationLibrary)
Console commands registered in `SLFConversion.cpp`:

| Command | Purpose |
|---------|---------|
| `SLF.ImportMesh <FBX> <ContentPath> <AssetName> [SkeletonPath] [Scale]` | Import skeletal mesh + create skeleton |
| `SLF.ImportAnims <FBXDir> <ContentPath> <SkeletonPath> [Scale]` | Batch import animations |
| `SLF.CheckAnim <AnimPath>` | Diagnose animation data (DataModel + compressed) |

---

## Step 1: Extract Animations from Game Archives

Extracts HKX animation data from ANIBND archives, builds correct armature in Blender, and exports per-animation FBX files.

### Command
```bash
blender --background --python C:/scripts/elden_ring_tools/extract_animations.py -- \
  --game-dir "C:/Program Files (x86)/Steam/steamapps/common/ELDEN RING/Game" \
  --characters c3100 \
  --output C:/scripts/elden_ring_tools/output/c3100 \
  --skip-existing
```

### Arguments
- `--characters`: Space-separated character IDs (c3100, c2120, c4810, etc.)
- `--output`: Root output directory
- `--filter`: Glob pattern to limit animations (e.g., `a000_003*` for attacks only)
- `--max-anims`: Cap number of animations (0 = all)
- `--skip-existing`: Don't re-export existing FBX files

### Output Structure
```
output/c3100/c3100/
  fbx/
    a000_000020.fbx    # idle
    a000_003000.fbx    # attack
    a000_003001.fbx    # attack combo 2
    ... (112 files for c3100)
  tae/
    a000_000020.json   # TAE events (AnimNotify timing)
    ...
```

### Key Details
- Each FBX contains the full HKX skeleton + animation keyframes + a minimal 1-triangle mesh
- Armature is built using the **correct** BONE_CoB chain rule: chain locals in GAME space first, then convert world transforms to Blender space
- FBX axis convention: `axis_forward='-Y', axis_up='Z', primary_bone_axis='Y', secondary_bone_axis='X'`
- Scale: meter-scale (1 unit = 1 meter). UE5 uses cm, so need 100x scale at import.

---

## Step 2: Visual Review in Blender (Optional but Recommended)

Creates .blend files with FLVER mesh skinned to HKX armature + animation for visual verification.

### Command
```bash
blender --background --python C:/scripts/elden_ring_tools/build_proper_blend.py
```

### Configuration
Edit constants at top of `build_proper_blend.py`:
```python
CHAR_ID = "c3100"
WANTED_ANIMS = {"a000_000020": "idle", "a000_003000": "attack", ...}
```

### Output
```
output/c3100_proper/
  c3100_idle.blend
  c3100_walk.blend
  c3100_attack.blend
  renders/  (15 preview PNGs)
```

Open .blend in Blender, press Space to play animation. Verify character moves, not stuck in A-pose.

---

## Step 3: Import Mesh into UE5 (Creates Skeleton)

Imports a skeletal mesh FBX at 100x scale to create a cm-scale skeleton for animations.

### Mesh Source
Use the Mannequin mesh FBX from the reskin pipeline (has actual geometry, unlike the minimal 1-triangle mesh in animation FBX):
```
C:/scripts/elden_ring_tools/output/c3100_guard/a000_000020_idle/SKM_Manny_export_c3100.fbx
```

If no reskinned mesh exists, any FBX with actual mesh geometry and the correct skeleton will work.

### Command
```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" \
  "C:/scripts/SLFConversion/SLFConversion.uproject" \
  -ExecCmds="SLF.ImportMesh C:/scripts/elden_ring_tools/output/c3100_guard/a000_000020_idle/SKM_Manny_export_c3100.fbx /Game/EldenRingAnimations/c3100_cm c3100_cm_mesh _ 100" \
  -unattended -nosplash -nullrhi
```

### Parameters
```
SLF.ImportMesh <FBXPath> <ContentPath> <AssetName> [SkeletonPath] [Scale]
```
- `FBXPath`: Absolute path to mesh FBX file
- `ContentPath`: UE5 content path (e.g., `/Game/EldenRingAnimations/c3100_cm`)
- `AssetName`: Name for imported asset (used as prefix)
- `SkeletonPath`: Existing skeleton to assign, or `_` to auto-create
- `Scale`: Import scale factor (100 = meter-to-cm conversion)

### Sample Log Output
```
Cmd: SLF.ImportMesh C:/.../SKM_Manny_export_c3100.fbx /Game/EldenRingAnimations/c3100_cm c3100_cm_mesh _ 100
LogSLFAutomation: [ImportMesh] Importing: ... -> /Game/EldenRingAnimations/c3100_cm/c3100_cm_mesh (scale=100.0)
LogFbx: Bones digested - 104  Depth of hierarchy - 13
LogSkeletalMesh: Built Skeletal Mesh [1.40s] /Game/EldenRingAnimations/c3100_cm/SKM_Manny_export_c3100
LogSLFAutomation: [ImportMesh] Saved OK: .../Content/EldenRingAnimations/c3100_cm/SKM_Manny_export_c3100.uasset
LogSLFAutomation: [ImportMesh] Saved skeleton OK: .../Content/EldenRingAnimations/c3100_cm/SKM_Manny_export_c3100_Skeleton.uasset
[SLF.ImportMesh] Imported 1 asset(s):
Mesh: /Game/EldenRingAnimations/c3100_cm/SKM_Manny_export_c3100, Bones: 104, LODs: 1
Final skeleton: /Game/EldenRingAnimations/c3100_cm/SKM_Manny_export_c3100_Skeleton
```

### Output Files
```
Content/EldenRingAnimations/c3100_cm/
  SKM_Manny_export_c3100.uasset           (16.8 MB mesh)
  SKM_Manny_export_c3100_Skeleton.uasset  (22.9 KB skeleton)
```

### Critical: Skeleton Persistence
The mesh AND skeleton packages are explicitly saved to disk. This was a bug fix (Feb 2026) - previously the auto-created skeleton was only in memory and lost between editor sessions.

---

## Step 4: Batch Import Animations

Imports all FBX animations at 100x scale targeting the skeleton from Step 3.

### Command
```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" \
  "C:/scripts/SLFConversion/SLFConversion.uproject" \
  -ExecCmds="SLF.ImportAnims C:/scripts/elden_ring_tools/output/c3100/c3100/fbx /Game/EldenRingAnimations/c3100_cm /Game/EldenRingAnimations/c3100_cm/SKM_Manny_export_c3100_Skeleton 100" \
  -unattended -nosplash -nullrhi
```

### Parameters
```
SLF.ImportAnims <FBXDir> <ContentPath> <SkeletonPath> [Scale]
```
- `FBXDir`: Directory containing .fbx animation files
- `ContentPath`: UE5 content destination path
- `SkeletonPath`: Full path to skeleton asset (from Step 3), or `auto` to create from first FBX
- `Scale`: Import scale factor (must match Step 3 - typically 100)

### Sample Log Output
```
Cmd: SLF.ImportAnims C:/.../fbx /Game/EldenRingAnimations/c3100_cm /Game/.../SKM_Manny_export_c3100_Skeleton 100
[BatchImportAnims] Importing 112 FBX files (scale=100.0) to /Game/EldenRingAnimations/c3100_cm
  OK: a000_000020.fbx -> /Game/EldenRingAnimations/c3100_cm/a000_000020
  OK: a000_001800.fbx -> /Game/EldenRingAnimations/c3100_cm/a000_001800
  ...
[SLF.ImportAnims] Imported 112/112 animations to /Game/EldenRingAnimations/c3100_cm (scale=100.0)
Skeleton: /Game/EldenRingAnimations/c3100_cm/SKM_Manny_export_c3100_Skeleton
```

### Output Files
```
Content/EldenRingAnimations/c3100_cm/
  SKM_Manny_export_c3100.uasset            (mesh)
  SKM_Manny_export_c3100_Skeleton.uasset   (skeleton)
  a000_000020.uasset                        (idle)
  a000_003000.uasset                        (attack 1)
  a000_003001.uasset                        (attack 2)
  ... (112 animation uassets)
  Total: 114 files, ~95 MB
```

### DDC Compression Order (Critical)
The import function uses this order for each animation:
1. `CacheDerivedDataForCurrentPlatform()` - triggers async DDC compression
2. `FAssetCompilingManager::Get().FinishAllCompilation()` - WAIT for compression
3. `UPackage::SavePackage()` - save WITH compressed data

**If save happens before FinishAllCompilation, the saved .uasset won't have compressed data** and editor runtime eval (GetBoneTransform) returns identity/A-pose.

---

## Step 5: Validate

### Quick Validation (DataModel)
```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" \
  "C:/scripts/SLFConversion/SLFConversion.uproject" \
  -ExecCmds="SLF.CheckAnim /Game/EldenRingAnimations/c3100_cm/a000_003000" \
  -unattended -nosplash -nullrhi
```

### Sample Output
```
[SLF.CheckAnim] === /Game/EldenRingAnimations/c3100_cm/a000_003000 ===
[SLF.CheckAnim] Skeleton: .../SKM_Manny_export_c3100_Skeleton
[SLF.CheckAnim] PlayLength: 3.167, CompressedValid: 1
[SLF.CheckAnim] DataModel class: AnimationSequencerDataModel
[SLF.CheckAnim] NumKeys: 96, NumFrames: 95, FrameRate: 30.0
[SLF.CheckAnim]   DM[1] Master: R0=(-0.98,-.18,0.00,0.00) RM=(0.75,-0.66,-0.00,0.00) MOVES
[SLF.CheckAnim]   DM[8] Root: R0=(0.48,-0.49,-0.51,0.52) RM=(0.33,-0.42,-0.53,0.67) MOVES
[SLF.CheckAnim]   DM[9] Pelvis: R0=(-0.02,0.00,-0.02,1.00) RM=(-0.00,0.00,-0.00,1.00) MOVES
[SLF.CheckAnim] DataModel eval: 25/30 bones have movement
```

### What to Check
- **DataModel eval: X/30 bones have movement** - should be >20 for attack anims, >5 for idle
- **DataModel class: AnimationSequencerDataModel** - correct for UE5.7
- **NumKeys > 1** - confirms actual keyframe data
- **PlayLength > 0** - confirms animation has duration

### Known Issue: Compressed Eval = 0
`GetBoneTransform()` with `FSkeletonPoseBoneIndex` may return identity in headless mode. This does NOT affect editor preview (which uses DataModel) or AnimGraph playback. DataModel is the ground truth.

### Visual Validation
Open UE5 Editor, browse to `/Game/EldenRingAnimations/c3100_cm/`, double-click any animation asset. The preview viewport should show the mesh with actual movement at human scale.

---

## Troubleshooting

### Animation shows A-pose
1. Check DataModel eval - if 0/30 bones move, the FBX has no animation data
2. Verify extraction used correct BONE_CoB chain rule (see animation-pipeline.md)
3. Re-run `build_proper_blend.py` to visually confirm in Blender first

### "Cannot load skeleton" error
- Skeleton wasn't saved to disk. Fixed in Feb 2026 (`ImportSkeletalMeshFromFBX` now saves both mesh and skeleton packages)
- Verify files exist: `ls Content/EldenRingAnimations/<charID>_cm/*Skeleton*`

### Mesh invisible in editor
- Likely meter-scale (1.8cm tall in UE5). Use Scale=100 for import
- Verify: `SLF.CheckAnim` should show Bones: 104 for c3100

### FBX import returns no assets
- Raw animation FBX from extract_animations.py has minimal 1-triangle mesh
- For mesh import (Step 3), use a real mesh FBX (e.g., from reskin pipeline)
- Animation-only import (Step 4) works fine with minimal mesh

### Skeleton mismatch (silent AnimInstance NULL)
- If mesh's USkeleton differs from AnimBP's TargetSkeleton, InitAnim returns NULL silently
- Use `SLF.MergeSkeleton` to reassign, then `SLF.AddSocket` for missing sockets

---

## Character ID Reference

| ID | Character | Anim Count | Notes |
|----|-----------|------------|-------|
| c3100 | Crucible Knight | 112 | Fully imported + reskinned |
| c4810 | Erdtree Avatar | 49 | Retargeted to SK_Mannequin |
| c2120 | Malenia | 146 | Mesh re-skinned |

### Completed Import Locations

| Character | Path | Scale | Skeleton |
|-----------|------|-------|----------|
| c3100 (cm-scale) | `/Game/EldenRingAnimations/c3100_cm/` | 100x | `SKM_Manny_export_c3100_Skeleton` |
| c3100 (meter-scale) | `/Game/EldenRingAnimations/c3100_direct/` | 1x | original |
| c3100 (reskinned) | `/Game/Temp/c3100_v8_meter/` | 76x runtime | `c3100_guard_a000_000020_idle_Skeleton` |
| c4810 | `/Game/EldenRingAnimations/c4810/` | - | retargeted |
| c2120 | `/Game/EldenRingAnimations/c2120/` | - | re-skinned |

---

## Adding a New Character

1. **Find character ID** - Check `C:/scripts/elden_ring_tools/unpack_er_archives.py` or Elden Ring modding wikis
2. **Extract**: `blender --background --python extract_animations.py -- --characters cXXXX --output output/cXXXX --game-dir "C:/..."`
3. **Review**: Edit `CHAR_ID` in `build_proper_blend.py`, run it, open .blend files
4. **Get mesh FBX**: Either reskin Mannequin (`reskin_guard_v8.py`) or use existing mesh
5. **Import mesh**: `SLF.ImportMesh <mesh.fbx> /Game/EldenRingAnimations/cXXXX_cm cXXXX_mesh _ 100`
6. **Import anims**: `SLF.ImportAnims <fbx_dir> /Game/EldenRingAnimations/cXXXX_cm /Game/.../cXXXX_mesh_Skeleton 100`
7. **Validate**: `SLF.CheckAnim /Game/EldenRingAnimations/cXXXX_cm/<anim_name>`
8. **Visual check**: Open editor, preview animations
