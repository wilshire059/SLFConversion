"""
Build proper .blend files: HKX armature + FLVER mesh + HKX animations.

KEY FIX: The armature MUST be in BLENDER SPACE (Y/Z swap + CoB applied to
rest pose), same as the animation data. import_animation_to_action() applies
_game_bone_transform_to_bl_bone_matrix() to the animation, so the armature
rest pose must go through the SAME conversion to avoid cross-space basis
matrix multiplication.

APPROACH:
1. Build HKX armature with CoB applied to bone rest poses (Blender space)
2. Build FLVER mesh with Y/Z swap (Blender space)
3. import_animation_to_action() for animation (already in Blender space)
4. Both armature and animation in same space → correct basis matrices
"""
import bpy
import sys
import os
import re
import math
import traceback
import numpy as np
from pathlib import Path
from mathutils import Vector, Quaternion, Matrix

sys.path.insert(0, "C:/scripts/elden_ring_tools")

from extract_animations import (
    read_animation_hkx_entry,
    read_skeleton_hkx_entry,
    import_animation_to_action,
)
from soulstruct.containers import Binder
from soulstruct.flver import FLVER
from soulstruct.havok.core import HKX

# Config
CHAR_ID = "c3100"
CHRBND = Path(f"C:/scripts/elden_ring_tools/unpacked/chr/{CHAR_ID}.chrbnd.dcx")
ANIBND = Path(f"C:/scripts/elden_ring_tools/unpacked/chr/{CHAR_ID}.anibnd.dcx")
OUTPUT_DIR = Path("C:/scripts/elden_ring_tools/output/c3100_proper")

WANTED_ANIMS = ["a000_000020", "a000_001800", "a000_003000"]
ANIM_LABELS = {"a000_000020": "idle", "a000_001800": "walk", "a000_003000": "attack"}


# ============================================================================
# Coordinate conversion functions (same as extract_animations.py)
# ============================================================================

def _to_blender_vector(v):
    """Game (x,y,z) -> Blender (x,z,y)"""
    return Vector((v[0], v[2], v[1]))

def _to_blender_matrix3(m):
    """Swap rows 1&2 and columns 1&2"""
    return Matrix((
        (m[0][0], m[0][2], m[0][1]),
        (m[2][0], m[2][2], m[2][1]),
        (m[1][0], m[1][2], m[1][1]),
    ))

BONE_CoB = Matrix((
    (0.0, 1.0,  0.0, 0.0),
    (1.0, 0.0,  0.0, 0.0),
    (0.0, 0.0, -1.0, 0.0),
    (0.0, 0.0,  0.0, 1.0),
))

def game_bone_to_bl_matrix(translate, rotmat, scale):
    """Same as _game_bone_transform_to_bl_bone_matrix from extract_animations.py"""
    bl_translate = _to_blender_vector(translate)
    bl_rotmat = _to_blender_matrix3(rotmat)
    bl_scale = _to_blender_vector(scale)
    bl_transform = Matrix.Translation(bl_translate) @ bl_rotmat.to_4x4()
    for i in range(3):
        bl_transform[i][i] *= bl_scale[i]
    return bl_transform @ BONE_CoB


def clean_scene():
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete(use_global=False)
    for coll in [bpy.data.meshes, bpy.data.armatures, bpy.data.actions,
                 bpy.data.cameras, bpy.data.lights, bpy.data.materials]:
        for block in list(coll):
            coll.remove(block, do_unlink=True)


def strip_root_motion(action):
    """Remove root motion FCurves so character stays centered."""
    root_paths = {"location", "rotation_euler", "rotation_quaternion"}
    if hasattr(action, 'layers'):
        for layer in action.layers:
            for strip in layer.strips:
                for bag in strip.channelbags:
                    to_remove = [fc for fc in bag.fcurves if fc.data_path in root_paths]
                    for fc in to_remove:
                        bag.fcurves.remove(fc)
                    if to_remove:
                        print(f"    Stripped {len(to_remove)} root motion FCurves")


def build_hkx_armature_blender_space():
    """Build HKX armature in BLENDER SPACE (with CoB applied to rest poses).

    CRITICAL: The animation pipeline works like this:
    1. _get_armature_frames() returns ARMATURE-SPACE game transforms per bone
    2. _game_bone_transform_to_bl_bone_matrix() converts each to Blender space
    3. _get_basis_matrix() uses bone.matrix_local (must be in same Blender space)

    So we must: chain locals in GAME space -> get game world transform per bone
    -> convert each bone's WORLD transform to Blender space via the SAME conversion.

    Chaining Blender-space locals is WRONG because CoB(A)@CoB(B) != CoB(A@B)
    due to the intermediate BONE_CoB terms.
    """
    print(f"\n=== Building HKX armature (Blender space) ===")
    anibnd = Binder.from_path(ANIBND)

    compendium = None
    try:
        comp_entry = anibnd.find_entry_matching_name(r".*\.compendium")
        compendium = HKX.from_binder_entry(comp_entry)
    except:
        pass

    skel_entry = anibnd.find_entry_matching_name(r"skeleton\.hkx(\.dcx)?", flags=re.IGNORECASE)
    skeleton_hkx = read_skeleton_hkx_entry(skel_entry, compendium)
    skeleton = skeleton_hkx.skeleton
    bones_data = skeleton.bones
    print(f"  {len(bones_data)} bones")

    # PASS 1: Compute GAME-SPACE world matrices by chaining locals
    game_world_matrices = {}  # bone_index -> 4x4 Matrix (game space)
    bone_parent_indices = {}

    for i, hk_bone in enumerate(bones_data):
        ref_pose = hk_bone.get_reference_pose()
        translation = ref_pose.translation
        rotation = ref_pose.rotation
        scale = ref_pose.scale

        parent_bone = hk_bone.parent
        parent_idx = parent_bone.index if parent_bone is not None else -1
        bone_parent_indices[i] = parent_idx

        # Build LOCAL matrix in game space (same as create_armature_from_skeleton_hkx)
        quat = Quaternion((rotation.w, rotation.x, rotation.y, rotation.z))
        local_mat = Matrix.Translation(Vector((
            translation.x, translation.y, translation.z
        ))) @ quat.to_matrix().to_4x4()
        scale_mat = Matrix.Diagonal(Vector((scale.x, scale.y, scale.z, 1.0)))
        local_mat = local_mat @ scale_mat

        # Chain parents in GAME SPACE
        if parent_idx >= 0 and parent_idx in game_world_matrices:
            game_world_matrices[i] = game_world_matrices[parent_idx] @ local_mat
        else:
            game_world_matrices[i] = local_mat

    # PASS 2: Convert each bone's game-space WORLD transform to Blender space
    arm_data = bpy.data.armatures.new(f"{CHAR_ID}_Armature")
    arm_obj = bpy.data.objects.new(CHAR_ID, arm_data)
    bpy.context.collection.objects.link(arm_obj)
    bpy.context.view_layer.objects.active = arm_obj
    arm_obj.select_set(True)

    bpy.ops.object.mode_set(mode='EDIT')
    edit_bones = arm_data.edit_bones
    bone_map = {}

    for i, hk_bone in enumerate(bones_data):
        eb = edit_bones.new(hk_bone.name)

        # Set parent
        parent_idx = bone_parent_indices[i]
        if parent_idx >= 0 and parent_idx in bone_map:
            eb.parent = bone_map[parent_idx]

        # Decompose game-space world matrix into T, R, S
        game_world = game_world_matrices[i]
        t, r, s = game_world.decompose()

        # Convert to Blender space using SAME logic as _game_bone_transform_to_bl_bone_matrix
        # Y/Z swap on translation
        bl_translate = Vector((t.x, t.z, t.y))
        # Y/Z swap on rotation matrix (swap rows 1&2, cols 1&2)
        game_rotmat = r.to_matrix()
        bl_rotmat = Matrix((
            (game_rotmat[0][0], game_rotmat[0][2], game_rotmat[0][1]),
            (game_rotmat[2][0], game_rotmat[2][2], game_rotmat[2][1]),
            (game_rotmat[1][0], game_rotmat[1][2], game_rotmat[1][1]),
        ))
        # Y/Z swap on scale
        bl_scale = Vector((s.x, s.z, s.y))

        # Build Blender-space transform (same assembly as _game_bone_transform_to_bl_bone_matrix)
        bl_transform = Matrix.Translation(bl_translate) @ bl_rotmat.to_4x4()
        for j in range(3):
            bl_transform[j][j] *= bl_scale[j]
        bl_world_mat = bl_transform @ BONE_CoB

        # Set bone
        head = bl_world_mat.translation
        bone_length = 0.05
        tail_dir = bl_world_mat @ Vector((0, bone_length, 0))

        eb.head = head
        eb.tail = tail_dir
        eb.matrix = bl_world_mat

        if (eb.tail - eb.head).length < 0.001:
            eb.tail = eb.head + Vector((0, 0.05, 0))

        bone_map[i] = eb

    bpy.ops.object.mode_set(mode='OBJECT')
    print(f"  Created armature with {len(bone_map)} bones in Blender space")
    return arm_obj, skeleton_hkx, anibnd, compendium


def load_animations(armature_obj, skeleton_hkx, anibnd, compendium):
    """Import animations (already in Blender space via _game_bone_transform_to_bl_bone_matrix)."""
    print(f"\n=== Loading animations ===")
    actions = {}
    anim_entries = anibnd.find_entries_matching_name(r"a.*\.hkx(\.dcx)?")

    for entry in anim_entries:
        anim_name = entry.name.split('.')[0]
        if anim_name not in WANTED_ANIMS:
            continue

        label = ANIM_LABELS.get(anim_name, anim_name)
        print(f"  {anim_name} ({label})...")

        try:
            anim_hkx = read_animation_hkx_entry(entry, compendium)
            action = import_animation_to_action(
                bpy.context, anim_hkx, skeleton_hkx, anim_name, armature_obj, CHAR_ID
            )
            strip_root_motion(action)
            actions[label] = action
            fs, fe = int(action.frame_range[0]), int(action.frame_range[1])
            print(f"    OK: frames {fs}-{fe}")
        except Exception as e:
            traceback.print_exc()
            print(f"    FAILED: {e}")

    return actions


def add_flver_mesh(armature_obj, flver):
    """Build FLVER mesh in BLENDER SPACE (Y/Z swap) to match armature."""
    print(f"\n=== Building FLVER mesh ({len(flver.meshes)} submeshes, Blender space) ===")

    flver_bone_names = [b.name for b in flver.bones]
    arm_bone_names = set(b.name for b in armature_obj.data.bones)

    all_verts = []
    all_faces = []
    all_weights = {}
    vert_offset = 0

    for mi, fmesh in enumerate(flver.meshes):
        try:
            verts_arr = fmesh.vertices
            positions = verts_arr['position']
        except:
            continue
        if positions is None or len(positions) == 0:
            continue

        # BLENDER SPACE: swap Y and Z (game Y-up → Blender Z-up)
        bl_positions = np.column_stack([positions[:, 0], positions[:, 2], positions[:, 1]])
        verts = [(float(p[0]), float(p[1]), float(p[2])) for p in bl_positions]
        all_verts.extend(verts)

        face_set = None
        for fs in fmesh.face_sets:
            if not (fs.flags & 0x80) and not (fs.flags & 0x02):
                face_set = fs
                break
        if face_set is None and fmesh.face_sets:
            face_set = fmesh.face_sets[0]

        if face_set:
            tri_indices = face_set.vertex_indices
            if tri_indices.ndim == 2 and tri_indices.shape[1] == 3:
                for tri in tri_indices:
                    f0, f1, f2 = int(tri[0]), int(tri[1]), int(tri[2])
                    if f0 != f1 and f1 != f2 and f0 != f2:
                        all_faces.append((f0 + vert_offset, f1 + vert_offset, f2 + vert_offset))

        try:
            bone_indices_arr = verts_arr['bone_indices']
            bone_weights_arr = verts_arr['bone_weights']
        except:
            bone_indices_arr = None
            bone_weights_arr = None

        if bone_indices_arr is not None and bone_weights_arr is not None:
            for vi in range(len(positions)):
                for bi in range(min(4, bone_indices_arr.shape[1])):
                    local_idx = int(bone_indices_arr[vi, bi])
                    weight = float(bone_weights_arr[vi, bi])
                    if weight > 0.001:
                        if fmesh.bone_indices is not None and local_idx < len(fmesh.bone_indices):
                            global_idx = int(fmesh.bone_indices[local_idx])
                        else:
                            global_idx = local_idx
                        if global_idx < len(flver_bone_names):
                            bname = flver_bone_names[global_idx]
                            if bname in arm_bone_names:
                                if bname not in all_weights:
                                    all_weights[bname] = []
                                all_weights[bname].append((vi + vert_offset, weight))
        vert_offset += len(verts)

    print(f"  {len(all_verts)} verts, {len(all_faces)} faces, {len(all_weights)} groups")

    mesh_data = bpy.data.meshes.new(f"{CHAR_ID}_mesh")
    mesh_data.from_pydata(all_verts, [], all_faces)
    mesh_data.update()

    mesh_obj = bpy.data.objects.new(f"{CHAR_ID}_mesh", mesh_data)
    bpy.context.scene.collection.objects.link(mesh_obj)

    for bname, weights in all_weights.items():
        vg = mesh_obj.vertex_groups.new(name=bname)
        for vi, w in weights:
            vg.add([vi], w, 'REPLACE')

    mesh_obj.parent = armature_obj
    mod = mesh_obj.modifiers.new("Armature", 'ARMATURE')
    mod.object = armature_obj

    mat = bpy.data.materials.new(f"{CHAR_ID}_material")
    mat.use_nodes = True
    bsdf = mat.node_tree.nodes.get("Principled BSDF")
    if bsdf:
        bsdf.inputs["Base Color"].default_value = (0.45, 0.42, 0.38, 1.0)
        bsdf.inputs["Roughness"].default_value = 0.55
        bsdf.inputs["Metallic"].default_value = 0.4
    mesh_data.materials.append(mat)

    return mesh_obj


def setup_viewport():
    """Camera and lighting. Blender space: Z-up, Y-forward."""
    import mathutils

    cam_data = bpy.data.cameras.new("Camera")
    cam_data.lens = 40  # Wider to capture full character
    cam = bpy.data.objects.new("Camera", cam_data)
    bpy.context.scene.collection.objects.link(cam)
    # Blender space: character at Z=0(feet) to Z=2.8(head), facing Y
    cam.location = (1.2, -7.0, 1.5)
    target = mathutils.Vector((0, 0, 1.3))
    direction = target - cam.location
    rot = direction.to_track_quat('-Z', 'Y')
    cam.rotation_euler = rot.to_euler()
    bpy.context.scene.camera = cam

    # Key light
    sun = bpy.data.lights.new("KeyLight", 'SUN')
    sun.energy = 3.5
    sun.color = (1.0, 0.95, 0.9)
    sun_obj = bpy.data.objects.new("KeyLight", sun)
    bpy.context.scene.collection.objects.link(sun_obj)
    sun_obj.rotation_euler = (math.radians(60), math.radians(15), math.radians(-30))

    # Fill light
    fill = bpy.data.lights.new("FillLight", 'SUN')
    fill.energy = 1.5
    fill.color = (0.85, 0.9, 1.0)
    fill_obj = bpy.data.objects.new("FillLight", fill)
    bpy.context.scene.collection.objects.link(fill_obj)
    fill_obj.rotation_euler = (math.radians(50), math.radians(-10), math.radians(40))

    # Rim light
    rim = bpy.data.lights.new("RimLight", 'SUN')
    rim.energy = 2.0
    rim_obj = bpy.data.objects.new("RimLight", rim)
    bpy.context.scene.collection.objects.link(rim_obj)
    rim_obj.rotation_euler = (math.radians(30), math.radians(0), math.radians(160))

    # Ground plane at Z=0
    bpy.ops.mesh.primitive_plane_add(size=10, location=(0, 0, 0))
    ground = bpy.context.active_object
    ground_mat = bpy.data.materials.new("GroundMat")
    ground_mat.use_nodes = True
    bsdf = ground_mat.node_tree.nodes.get("Principled BSDF")
    if bsdf:
        bsdf.inputs["Base Color"].default_value = (0.08, 0.08, 0.1, 1.0)
        bsdf.inputs["Roughness"].default_value = 0.9
    ground.data.materials.append(ground_mat)

    # World
    world = bpy.data.worlds.get("World") or bpy.data.worlds.new("World")
    bpy.context.scene.world = world
    try:
        world.use_nodes = True
        bg = world.node_tree.nodes.get("Background")
        if bg:
            bg.inputs["Color"].default_value = (0.12, 0.12, 0.16, 1.0)
            bg.inputs["Strength"].default_value = 0.3
    except:
        pass

    bpy.context.scene.render.resolution_x = 1920
    bpy.context.scene.render.resolution_y = 1080
    bpy.context.scene.render.engine = 'BLENDER_EEVEE'


def main():
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    clean_scene()

    # Build HKX armature in Blender space (matching animation coordinate space)
    armature_obj, skeleton_hkx, anibnd, compendium = build_hkx_armature_blender_space()

    # Load FLVER mesh in Blender space
    print("\n=== Loading FLVER ===")
    binder = Binder.from_path(CHRBND)
    for entry in binder.entries:
        if entry.name.lower().endswith('.flver') or entry.name.lower().endswith('.flver.dcx'):
            flver = FLVER.from_binder_entry(entry)
            break
    print(f"  {len(flver.bones)} bones, {len(flver.meshes)} meshes")
    mesh_obj = add_flver_mesh(armature_obj, flver)

    # Import animations (root motion stripped)
    actions = load_animations(armature_obj, skeleton_hkx, anibnd, compendium)

    # Set up viewport
    setup_viewport()

    # Save .blend files
    for label, action in actions.items():
        if not armature_obj.animation_data:
            armature_obj.animation_data_create()
        armature_obj.animation_data.action = action
        if hasattr(action, 'slots') and len(action.slots) > 0:
            for slot in action.slots:
                try:
                    armature_obj.animation_data.action_slot = slot
                    break
                except:
                    continue

        fs, fe = int(action.frame_range[0]), int(action.frame_range[1])
        bpy.context.scene.frame_start = fs
        bpy.context.scene.frame_end = fe
        bpy.context.scene.frame_set(fs)

        path = str(OUTPUT_DIR / f"{CHAR_ID}_{label}.blend")
        bpy.ops.wm.save_as_mainfile(filepath=path)
        print(f"  Saved: {path}")

    # Render preview frames
    for label, action in actions.items():
        armature_obj.animation_data.action = action
        if hasattr(action, 'slots') and len(action.slots) > 0:
            for slot in action.slots:
                try:
                    armature_obj.animation_data.action_slot = slot
                    break
                except:
                    continue

        fs, fe = int(action.frame_range[0]), int(action.frame_range[1])
        quarter = fs + (fe - fs) // 4
        mid = (fs + fe) // 2
        three_q = fs + 3 * (fe - fs) // 4

        for frame in [fs, quarter, mid, three_q, fe]:
            bpy.context.scene.frame_set(frame)
            bpy.context.view_layer.update()
            render_path = str(OUTPUT_DIR / f"{CHAR_ID}_{label}_f{frame:04d}.png")
            bpy.context.scene.render.filepath = render_path
            bpy.context.scene.render.resolution_percentage = 50
            bpy.ops.render.render(write_still=True)

    print(f"\n{'='*60}")
    print(f"DONE! Output: {OUTPUT_DIR}")
    blends = [f for f in sorted(os.listdir(OUTPUT_DIR)) if f.endswith('.blend')]
    pngs = [f for f in sorted(os.listdir(OUTPUT_DIR)) if f.endswith('.png')]
    print(f"  {len(blends)} .blend files, {len(pngs)} renders")


if __name__ == "__main__":
    main()
