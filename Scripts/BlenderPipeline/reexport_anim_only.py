"""
reexport_anim_only.py - Re-export Phase 3 forensic FBXes as animation-only (no mesh).

Problem: Phase 3 FBXes contain both mesh and animation. UE5's native FBX importer
gets confused by the mesh and either produces 0 bone tracks or imports as StaticMesh.

Solution: Strip the mesh, export armature-only FBX with primary_bone_axis='X'
(UE5 Mannequin convention) instead of 'Y' (Blender default).

Usage:
  blender --background --python reexport_anim_only.py

Input:  C:/scripts/elden_ring_tools/output/sentinel_pipeline/phase3/final/Sentinel_*.fbx
Output: C:/scripts/elden_ring_tools/output/sentinel_pipeline/phase3/anim_only/Sentinel_*.fbx
"""

import bpy
import os
import sys
import gc

# ============================================================
# Configuration
# ============================================================

INPUT_DIR = r"C:/scripts/elden_ring_tools/output/sentinel_pipeline/phase3/final"
OUTPUT_DIR = r"C:/scripts/elden_ring_tools/output/sentinel_pipeline/phase3/anim_only"

ANIM_NAMES = [
    "Sentinel_Idle",
    "Sentinel_Walk",
    "Sentinel_Run",
    "Sentinel_Attack01",
    "Sentinel_Attack02",
    "Sentinel_Attack03_Fast",
    "Sentinel_HeavyAttack",
    "Sentinel_HitReact",
    "Sentinel_HitReact_Light",
    "Sentinel_GuardHit",
    "Sentinel_Death_Front",
    "Sentinel_Death_Back",
    "Sentinel_Death_Left",
    "Sentinel_Dodge_Fwd",
    "Sentinel_Dodge_Bwd",
    "Sentinel_Dodge_Left",
    "Sentinel_Dodge_Right",
    "Sentinel_Guard",
    "Sentinel_PoiseBreak_Start",
    "Sentinel_PoiseBreak_Loop",
]

# FBX import settings (same as Phase 3 pipeline used)
FBX_IMPORT = dict(
    use_anim=True,
    ignore_leaf_bones=True,
    automatic_bone_orientation=False,
    primary_bone_axis='Y',
    secondary_bone_axis='X',
    axis_forward='-Y',
    axis_up='Z',
)

# FBX export settings — MUST match mesh export bone axis (Y/X) to avoid skeleton mismatch
FBX_EXPORT = dict(
    use_selection=True,
    global_scale=1.0,
    apply_unit_scale=True,
    apply_scale_options='FBX_SCALE_ALL',
    axis_forward='-Y',
    axis_up='Z',
    use_mesh_modifiers=False,
    mesh_smooth_type='OFF',
    add_leaf_bones=False,
    primary_bone_axis='Y',       # MUST match mesh FBX (Phase 3 used 'Y')
    secondary_bone_axis='X',     # MUST match mesh FBX (Phase 3 used 'X')
    use_armature_deform_only=False,
    bake_anim=True,
    bake_anim_use_all_bones=True,
    bake_anim_use_nla_strips=False,
    bake_anim_use_all_actions=False,
    bake_anim_force_startend_keying=True,
    bake_anim_simplify_factor=0.0,
    object_types={'ARMATURE'},   # ARMATURE ONLY - no mesh
)


def clean_scene():
    """Remove all objects and orphan data."""
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete(use_global=True)
    for coll in [bpy.data.meshes, bpy.data.armatures, bpy.data.actions,
                 bpy.data.cameras, bpy.data.lights, bpy.data.materials]:
        for block in list(coll):
            if block.users == 0:
                coll.remove(block)


def process_animation(anim_name):
    """Load one Phase 3 FBX, strip mesh, re-export as armature-only."""
    input_path = os.path.join(INPUT_DIR, f"{anim_name}.fbx")
    output_path = os.path.join(OUTPUT_DIR, f"{anim_name}.fbx")

    if not os.path.exists(input_path):
        print(f"  SKIP {anim_name}: {input_path} not found")
        return False

    # Clean scene
    clean_scene()

    # Import FBX
    print(f"  Importing {anim_name}...")
    bpy.ops.import_scene.fbx(filepath=input_path, **FBX_IMPORT)

    # Find the armature
    arm = None
    for obj in bpy.data.objects:
        if obj.type == 'ARMATURE':
            arm = obj
            break

    if arm is None:
        print(f"  ERROR {anim_name}: No armature found in FBX")
        return False

    # Delete all mesh objects (keep only armature)
    meshes_deleted = 0
    for obj in list(bpy.data.objects):
        if obj.type == 'MESH':
            bpy.data.objects.remove(obj, do_unlink=True)
            meshes_deleted += 1

    # Clean orphan mesh data
    for mesh in list(bpy.data.meshes):
        if mesh.users == 0:
            bpy.data.meshes.remove(mesh)

    print(f"  Stripped {meshes_deleted} mesh objects, armature has {len(arm.data.bones)} bones")

    # Verify animation data exists
    if arm.animation_data and arm.animation_data.action:
        action = arm.animation_data.action
        # Count fcurves
        fc_count = 0
        if hasattr(action, 'is_action_layered') and action.is_action_layered:
            for layer in action.layers:
                for strip in layer.strips:
                    for bag in strip.channelbags:
                        fc_count += len(bag.fcurves)
        elif hasattr(action, 'fcurves'):
            fc_count = len(action.fcurves)
        print(f"  Action: {action.name}, {fc_count} fcurves")
    else:
        print(f"  WARNING {anim_name}: No animation data on armature")

    # Select only armature for export
    bpy.ops.object.select_all(action='DESELECT')
    arm.select_set(True)
    bpy.context.view_layer.objects.active = arm

    # Export
    print(f"  Exporting armature-only to {output_path}...")
    bpy.ops.export_scene.fbx(filepath=output_path, **FBX_EXPORT)

    return True


def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    print("=" * 60)
    print("Re-exporting Phase 3 animations as armature-only FBX")
    print(f"  Input:  {INPUT_DIR}")
    print(f"  Output: {OUTPUT_DIR}")
    print(f"  Key change: primary_bone_axis='X' (was 'Y')")
    print(f"  Key change: object_types=ARMATURE only (no mesh)")
    print("=" * 60)

    success = 0
    failed = 0

    for i, anim_name in enumerate(ANIM_NAMES):
        print(f"\n[{i+1}/{len(ANIM_NAMES)}] {anim_name}")
        try:
            if process_animation(anim_name):
                success += 1
            else:
                failed += 1
        except Exception as e:
            print(f"  EXCEPTION {anim_name}: {e}")
            failed += 1

        # GC every 5 animations to prevent memory issues
        if (i + 1) % 5 == 0:
            gc.collect()

    print("\n" + "=" * 60)
    print(f"DONE: {success} succeeded, {failed} failed out of {len(ANIM_NAMES)}")
    print(f"Output directory: {OUTPUT_DIR}")
    print("=" * 60)


if __name__ == "__main__":
    main()
