"""
Elden Ring Animation Extraction - Headless Blender Script
=========================================================

Extracts animations from Elden Ring .anibnd.dcx files and exports as FBX
for import into Unreal Engine 5.

Usage:
    blender --background --python extract_animations.py -- \
        --anibnd "C:/path/to/c0000.anibnd.dcx" \
        --output "C:/output/animations/" \
        [--filter "a000_*"] \
        [--model-name "c0000"] \
        [--chrbnd "C:/path/to/c0000.chrbnd.dcx"]  # Optional: use FLVER skeleton

Also supports batch mode:
    blender --background --python extract_animations.py -- \
        --game-dir "C:/SteamLibrary/steamapps/common/ELDEN RING/Game" \
        --output "C:/output/animations/" \
        --characters c0000 c3400 c4100
"""

from __future__ import annotations

import argparse
import json
import math
import os
import re
import sys
import time
import traceback
from pathlib import Path
from typing import Optional

# Ensure the script's own directory is on sys.path for local imports (tae_parser)
_SCRIPT_DIR = str(Path(__file__).resolve().parent)
if _SCRIPT_DIR not in sys.path:
    sys.path.insert(0, _SCRIPT_DIR)

import bpy
import numpy as np
from mathutils import Matrix, Vector, Quaternion, Euler

# Soulstruct imports
from soulstruct.containers import Binder, EntryNotFoundError
from soulstruct.havok.core import HKX
from soulstruct.havok.utilities.maths import TRSTransform
from soulstruct.utilities.maths import Vector3, Matrix3

# These handle game-specific HKX formats
from soulstruct.havok.fromsoft.eldenring import AnimationHKX, SkeletonHKX

# For TAE parsing - use standalone parser (soulstruct TAE has constrata bytes/str bug)
from tae_parser import TAEParser, TAEAnimation

# Try to import DivBinder for Elden Ring split binders
try:
    from soulstruct.eldenring.containers import DivBinder
except ImportError:
    DivBinder = None


# ============================================================================
# COORDINATE CONVERSION (inlined from soulstruct-blender to avoid GPU imports)
# ============================================================================

# Change of Basis: FromSoft (X-forward, Y-up, left-handed)
#                → Blender  (X-right, Z-up, right-handed)
# Swap Y and Z axes.

# Bone CoB: additional matrix for bone-space orientation (X-forward in Blender)
BONE_CoB_4x4 = Matrix((
    (0.0, 1.0,  0.0, 0.0),
    (1.0, 0.0,  0.0, 0.0),
    (0.0, 0.0, -1.0, 0.0),
    (0.0, 0.0,  0.0, 1.0),
))


def _to_blender_vector(v: Vector3) -> Vector:
    """Swap Y and Z axes: game (X,Y,Z) → Blender (X,Z,Y)."""
    return Vector((v.x, v.z, v.y))


def _to_blender_matrix3(m: Matrix3) -> Matrix:
    """Swap rows 1&2, columns 1&2 for 3x3 rotation matrix."""
    return Matrix((
        (m[0][0], m[0][2], m[0][1]),
        (m[2][0], m[2][2], m[2][1]),
        (m[1][0], m[1][2], m[1][1]),
    ))


def _game_bone_transform_to_bl_bone_matrix(
    game_translate: Vector3,
    game_rotmat: Matrix3,
    game_scale: Vector3,
) -> Matrix:
    """Convert a game bone transform to a Blender bone matrix with bone CoB applied."""
    bl_translate = _to_blender_vector(game_translate)
    bl_rotmat = _to_blender_matrix3(game_rotmat)
    bl_scale = _to_blender_vector(game_scale)
    bl_transform = Matrix.Translation(bl_translate) @ bl_rotmat.to_4x4()
    for i in range(3):
        bl_transform[i][i] *= bl_scale[i]
    return bl_transform @ BONE_CoB_4x4


def _get_basis_matrix(
    armature: bpy.types.Object,
    bone_name: str,
    armature_matrix: Matrix,
    armature_inv_matrices: dict,
    cached_local_inv_matrices: dict,
) -> Matrix:
    """Get pose_bone.matrix_basis from desired armature-space matrix."""
    if bone_name not in cached_local_inv_matrices:
        cached_local_inv_matrices[bone_name] = armature.data.bones[bone_name].matrix_local.inverted()
    local_inv = cached_local_inv_matrices[bone_name]
    parent_bone = armature.pose.bones[bone_name].parent

    if parent_bone is None:
        return local_inv @ armature_matrix

    parent_removed = armature_inv_matrices[parent_bone.name] @ armature_matrix
    return local_inv @ armature.data.bones[parent_bone.name].matrix_local @ parent_removed


# ============================================================================
# LOGGING
# ============================================================================

class HeadlessLogger:
    """Stand-in for Blender's LoggingOperator in headless mode."""

    def info(self, msg):
        print(f"[INFO] {msg}")

    def warning(self, msg):
        print(f"[WARN] {msg}")

    def error(self, msg):
        print(f"[ERROR] {msg}")
        return {"CANCELLED"}

    def debug(self, msg):
        pass  # Suppress debug in headless


logger = HeadlessLogger()


# ============================================================================
# SKELETON → ARMATURE CREATION
# ============================================================================

def create_armature_from_skeleton_hkx(
    skeleton_hkx: SkeletonHKX,
    model_name: str = "character",
) -> bpy.types.Object:
    """Create a Blender Armature from a Havok skeleton HKX file.

    Builds the armature in BLENDER SPACE (with Y/Z swap and BONE_CoB applied).
    This is required because import_animation_to_action() converts animation
    data to Blender space via _game_bone_transform_to_bl_bone_matrix(), and
    _get_basis_matrix() uses bone.matrix_local (rest pose) which must be in
    the same coordinate space.

    IMPORTANT: Locals must be chained in GAME SPACE first, then each bone's
    world transform is converted to Blender space. Chaining Blender-space
    locals (each with BONE_CoB) produces wrong results because
    CoB(A) @ CoB(B) != CoB(A @ B).
    """
    skeleton = skeleton_hkx.skeleton
    bones_data = skeleton.bones

    logger.info(f"Creating armature with {len(bones_data)} bones from skeleton HKX")

    # PASS 1: Chain local transforms in GAME SPACE to get per-bone world matrices
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

        # Build local transform in game space
        quat = Quaternion((rotation.w, rotation.x, rotation.y, rotation.z))
        local_mat = Matrix.Translation(Vector((
            translation.x, translation.y, translation.z
        ))) @ quat.to_matrix().to_4x4()
        scale_mat = Matrix.Diagonal(Vector((scale.x, scale.y, scale.z, 1.0)))
        local_mat = local_mat @ scale_mat

        # Chain parents in game space
        if parent_idx >= 0 and parent_idx in game_world_matrices:
            game_world_matrices[i] = game_world_matrices[parent_idx] @ local_mat
        else:
            game_world_matrices[i] = local_mat

    # PASS 2: Convert each bone's game-space world transform to Blender space
    armature_data = bpy.data.armatures.new(f"{model_name}_Armature")
    armature_obj = bpy.data.objects.new(f"{model_name}", armature_data)
    bpy.context.collection.objects.link(armature_obj)
    bpy.context.view_layer.objects.active = armature_obj
    armature_obj.select_set(True)

    bpy.ops.object.mode_set(mode='EDIT')
    edit_bones = armature_data.edit_bones
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

        # Convert to Blender space (same as _game_bone_transform_to_bl_bone_matrix)
        bl_translate = Vector((t.x, t.z, t.y))
        game_rotmat = r.to_matrix()
        bl_rotmat = Matrix((
            (game_rotmat[0][0], game_rotmat[0][2], game_rotmat[0][1]),
            (game_rotmat[2][0], game_rotmat[2][2], game_rotmat[2][1]),
            (game_rotmat[1][0], game_rotmat[1][2], game_rotmat[1][1]),
        ))
        bl_scale = Vector((s.x, s.z, s.y))

        bl_transform = Matrix.Translation(bl_translate) @ bl_rotmat.to_4x4()
        for j in range(3):
            bl_transform[j][j] *= bl_scale[j]
        world_mat = bl_transform @ BONE_CoB_4x4

        # Set bone head and tail
        head = world_mat.translation
        bone_length = 0.05
        tail_dir = world_mat @ Vector((0, bone_length, 0))

        eb.head = head
        eb.tail = tail_dir
        eb.matrix = world_mat

        if (eb.tail - eb.head).length < 0.001:
            eb.tail = eb.head + Vector((0, 0.05, 0))

        bone_map[i] = eb

    bpy.ops.object.mode_set(mode='OBJECT')

    logger.info(f"Created armature '{armature_obj.name}' with {len(bone_map)} bones")
    return armature_obj


# ============================================================================
# ANIMATION IMPORT (adapted from soulstruct-blender)
# ============================================================================

def read_animation_hkx_entry(hkx_entry, compendium=None):
    """Read animation HKX from binder entry, auto-detecting game version."""
    data = hkx_entry.get_uncompressed_data()
    tagfile_version = data[0x10:0x18]

    if tagfile_version == b"20180100":  # Elden Ring
        hkx = AnimationHKX.from_bytes(data, compendium=compendium)
    else:
        raise ValueError(
            f"Unsupported HKX version. Tagfile: {tagfile_version}. "
            f"This script currently only supports Elden Ring."
        )
    hkx.path = Path(hkx_entry.name)
    return hkx


def read_skeleton_hkx_entry(hkx_entry, compendium=None):
    """Read skeleton HKX from binder entry."""
    data = hkx_entry.get_uncompressed_data()
    tagfile_version = data[0x10:0x18]

    if tagfile_version == b"20180100":  # Elden Ring
        hkx = SkeletonHKX.from_bytes(data, compendium=compendium)
    else:
        raise ValueError(f"Unsupported skeleton HKX version: {tagfile_version}")
    hkx.path = Path(hkx_entry.name)
    return hkx


def _get_armature_frames(animation_hkx, skeleton_hkx):
    """Get armature-space animation frames from HKX data (inlined from soulstruct-blender)."""
    track_bone_indices = animation_hkx.animation_container.get_track_bone_indices()
    track_bone_names = [skeleton_hkx.skeleton.bones[i].name for i in track_bone_indices]
    interleaved_frames = animation_hkx.animation_container.get_interleaved_data_in_armature_space(
        skeleton_hkx.skeleton
    )
    return [
        {bone_name: transform for bone_name, transform in zip(track_bone_names, frame)}
        for frame in interleaved_frames
    ]


def _get_root_motion(animation_hkx):
    """Extract root motion from HKX animation (inlined from soulstruct-blender)."""
    try:
        root_motion = animation_hkx.animation_container.get_reference_frame_samples()
    except (ValueError, TypeError):
        return None
    # Swap Y and Z axes and negate rotation for Blender
    root_motion = np.c_[root_motion[:, 0], root_motion[:, 2], root_motion[:, 1], -root_motion[:, 3]]
    return root_motion


def import_animation_to_action(
    context: bpy.types.Context,
    animation_hkx: AnimationHKX,
    skeleton_hkx: SkeletonHKX,
    name: str,
    armature_obj: bpy.types.Object,
    model_name: str,
) -> bpy.types.Action:
    """Import a single HKX animation as a Blender Action.

    Self-contained implementation that doesn't require soulstruct.blender imports
    (which fail in headless mode due to GPU shader initialization).
    """
    logger.info(f"Importing animation: {name}")

    bl_bone_names = set(b.name for b in armature_obj.data.bones)

    # Convert to interleaved if needed
    if not animation_hkx.animation_container.is_interleaved:
        interleaved = animation_hkx.to_interleaved_hkx()
    else:
        interleaved = animation_hkx

    arma_frames = _get_armature_frames(interleaved, skeleton_hkx)
    root_motion = _get_root_motion(interleaved)

    if not arma_frames:
        raise ValueError(f"No animation frames found in {name}")

    # Remove bones not in armature
    for frame in arma_frames:
        for bone_name in list(frame.keys()):
            if bone_name not in bl_bone_names:
                del frame[bone_name]

    # Convert armature-space TRSTransforms to Blender bone basis matrices
    frame_count = len(arma_frames)
    arma_local_inv_matrices = {
        bone.name: bone.matrix_local.inverted()
        for bone in armature_obj.data.bones
    }

    # Build bone basis samples: dict[bone_name, np.ndarray(frame_count, 11)]
    # Columns: t, loc_x, loc_y, loc_z, rot_w, rot_x, rot_y, rot_z, scale_x, scale_y, scale_z
    bone_basis_samples = {
        bone_name: np.empty((frame_count, 11))
        for bone_name in arma_frames[0].keys()
    }
    last_frame_rotations = {}

    for frame_i, frame in enumerate(arma_frames):
        # Convert game transforms to Blender bone matrices
        bl_arma_matrices = {}
        for bone_name, trs in frame.items():
            bl_arma_matrices[bone_name] = _game_bone_transform_to_bl_bone_matrix(
                trs.translation,
                trs.rotation.to_matrix3(),
                trs.scale,
            )

        cached_arma_inv_matrices = {}

        for bone_name, bl_arma_matrix in bl_arma_matrices.items():
            if bone_name not in bone_basis_samples:
                continue

            bl_edit_bone = armature_obj.data.bones[bone_name]

            if (
                bl_edit_bone.parent is not None
                and bl_edit_bone.parent.name not in cached_arma_inv_matrices
            ):
                parent_name = bl_edit_bone.parent.name
                if parent_name in bl_arma_matrices:
                    cached_arma_inv_matrices[parent_name] = bl_arma_matrices[parent_name].inverted()
                else:
                    cached_arma_inv_matrices[parent_name] = Matrix.Identity(4)

            bl_basis_matrix = _get_basis_matrix(
                armature_obj,
                bone_name,
                bl_arma_matrix,
                cached_arma_inv_matrices,
                arma_local_inv_matrices,
            )

            t, r, s = bl_basis_matrix.decompose()

            # Handle quaternion discontinuities
            if bone_name in last_frame_rotations:
                if last_frame_rotations[bone_name].dot(r) < 0.0:
                    r.negate()

            bone_basis_samples[bone_name][frame_i] = [
                float(frame_i),  # keyframe time
                *t, *r, *s,
            ]
            last_frame_rotations[bone_name] = r

    # Create Blender Action and add keyframes
    action_name = f"{model_name}|{name}"
    armature_obj.animation_data_create()
    action = bpy.data.actions.new(name=action_name)

    # Blender 5.0+ uses Slots/Layers/Strips/ChannelBags for FCurves
    # Blender < 5.0 uses action.fcurves directly
    use_slotted_actions = bpy.app.version >= (5, 0, 0)

    if use_slotted_actions:
        slot = action.slots.new(id_type='OBJECT', name=action_name)
        layer = action.layers.new(name=action_name)
        strip = layer.strips.new(type='KEYFRAME')
        channelbag = strip.channelbags.new(slot)
        fcurve_container = channelbag.fcurves
    else:
        if hasattr(action, 'id_root'):
            action.id_root = "OBJECT"
        fcurve_container = action.fcurves

    def new_fcurve(data_path: str, index: int = 0):
        return fcurve_container.new(data_path=data_path, index=index)

    try:
        all_fcurves = []

        # Add root motion FCurves if present
        if root_motion is not None and root_motion.ndim == 2 and root_motion.shape[1] == 4:
            # Prepend keyframe_t column
            keyframe_t = np.arange(root_motion.shape[0], dtype=np.float32)
            if len(root_motion) != frame_count and len(root_motion) > 1:
                keyframe_t *= frame_count / (root_motion.shape[0] - 1)
            root_motion_with_t = np.hstack([keyframe_t[:, None], root_motion])

            root_fcurves = [new_fcurve(data_path="location", index=i) for i in range(3)]
            root_fcurves.append(new_fcurve(data_path="rotation_euler", index=2))  # Z

            for fcurve_i, root_fcurve in enumerate(root_fcurves):
                data = root_motion_with_t[:, [0, fcurve_i + 1]]
                root_fcurve.keyframe_points.add(count=data.shape[0])
                root_fcurve.keyframe_points.foreach_set("co", data.ravel().tolist())
                for kp in root_fcurve.keyframe_points:
                    kp.interpolation = "LINEAR"
                all_fcurves.append(root_fcurve)

        # Add bone FCurves
        for bone_name, basis_samples in bone_basis_samples.items():
            bone_fcurves = []
            bone_fcurves += [
                new_fcurve(data_path=f'pose.bones["{bone_name}"].location', index=i)
                for i in range(3)
            ]
            bone_fcurves += [
                new_fcurve(data_path=f'pose.bones["{bone_name}"].rotation_quaternion', index=i)
                for i in range(4)
            ]
            bone_fcurves += [
                new_fcurve(data_path=f'pose.bones["{bone_name}"].scale', index=i)
                for i in range(3)
            ]

            for fcurve_i, bone_fcurve in enumerate(bone_fcurves):
                data = basis_samples[:, [0, fcurve_i + 1]]
                bone_fcurve.keyframe_points.add(count=data.shape[0])
                bone_fcurve.keyframe_points.foreach_set("co", data.ravel().tolist())
                for kp in bone_fcurve.keyframe_points:
                    kp.interpolation = "LINEAR"
                all_fcurves.append(bone_fcurve)

        # Assign action to armature
        armature_obj.animation_data.action = action
        if use_slotted_actions:
            armature_obj.animation_data.action_slot = slot
        elif bpy.app.version >= (4, 4, 0) and len(action.slots) > 0:
            armature_obj.animation_data.action_slot = action.slots[0]

        action.use_fake_user = True
        for fc in all_fcurves:
            fc.update()

        context.scene.frame_start = int(action.frame_range[0])
        context.scene.frame_end = int(action.frame_range[1])

    except Exception:
        bpy.data.actions.remove(action)
        raise

    return action


def _ensure_export_mesh(armature_obj: bpy.types.Object) -> bpy.types.Object:
    """Create or find a minimal mesh skinned to the armature for FBX export.

    UE5 5.7's Interchange FBX pipeline requires a mesh to be present for
    skeletal animation import. This creates a simple 1-triangle mesh skinned
    to the root bone. The mesh is reused across exports and cleaned up later.
    """
    mesh_name = f"{armature_obj.name}_ExportMesh"

    # Reuse existing if present
    for obj in bpy.data.objects:
        if obj.name == mesh_name and obj.type == 'MESH':
            return obj

    # Create minimal mesh: 1 triangle at origin
    import bmesh
    mesh = bpy.data.meshes.new(mesh_name)
    bm = bmesh.new()
    v1 = bm.verts.new((0.0, 0.0, 0.0))
    v2 = bm.verts.new((0.01, 0.0, 0.0))
    v3 = bm.verts.new((0.0, 0.01, 0.0))
    bm.faces.new([v1, v2, v3])
    bm.to_mesh(mesh)
    bm.free()

    mesh_obj = bpy.data.objects.new(mesh_name, mesh)
    bpy.context.collection.objects.link(mesh_obj)

    # Parent to armature with armature modifier
    mesh_obj.parent = armature_obj
    mod = mesh_obj.modifiers.new(name="Armature", type='ARMATURE')
    mod.object = armature_obj

    # Create vertex groups for all bones (UE5 expects them)
    root_bone_name = None
    for bone in armature_obj.data.bones:
        vg = mesh_obj.vertex_groups.new(name=bone.name)
        if bone.parent is None and root_bone_name is None:
            root_bone_name = bone.name
            # Assign all verts to root bone
            vg.add([0, 1, 2], 1.0, 'REPLACE')

    return mesh_obj


def export_action_as_fbx(
    armature_obj: bpy.types.Object,
    action: bpy.types.Action,
    output_path: Path,
    fps: float = 30.0,
):
    """Export a single animation action as FBX."""
    # Set the action as active
    if not armature_obj.animation_data:
        armature_obj.animation_data_create()
    armature_obj.animation_data.action = action
    if bpy.app.version >= (5, 0, 0) and len(action.slots) > 0:
        armature_obj.animation_data.action_slot = action.slots[0]

    # Set scene FPS
    bpy.context.scene.render.fps = int(fps)

    # Set frame range from action
    frame_start = int(action.frame_range[0])
    frame_end = int(action.frame_range[1])
    bpy.context.scene.frame_start = frame_start
    bpy.context.scene.frame_end = frame_end

    # Ensure a minimal mesh exists (UE5 5.7 Interchange needs it)
    export_mesh = _ensure_export_mesh(armature_obj)

    # Select armature and mesh
    bpy.ops.object.select_all(action='DESELECT')
    armature_obj.select_set(True)
    export_mesh.select_set(True)
    bpy.context.view_layer.objects.active = armature_obj

    # Export FBX
    output_path.parent.mkdir(parents=True, exist_ok=True)

    bpy.ops.export_scene.fbx(
        filepath=str(output_path),
        use_selection=True,
        object_types={'ARMATURE', 'MESH'},
        use_mesh_modifiers=True,
        use_armature_deform_only=False,
        add_leaf_bones=False,
        bake_anim=True,
        bake_anim_use_all_bones=True,
        bake_anim_use_nla_strips=False,
        bake_anim_use_all_actions=False,
        bake_anim_force_startend_keying=True,
        bake_anim_step=1.0,
        bake_anim_simplify_factor=0.0,  # No simplification
        path_mode='AUTO',
        embed_textures=False,
        axis_forward='-Y',
        axis_up='Z',
        global_scale=1.0,
        apply_scale_options='FBX_SCALE_ALL',
    )

    logger.info(f"Exported: {output_path}")


# ============================================================================
# TAE EXTRACTION
# ============================================================================

def extract_tae_from_anibnd(anibnd: Binder, anibnd_path: Path) -> Optional[dict]:
    """Extract and parse TAE from an ANIBND using standalone parser.

    Extracts raw TAE bytes via soulstruct Binder (handles DivBinder), then
    parses with our standalone tae_parser (avoids constrata bytes/str bug).

    Returns dict mapping anim_id -> TAEAnimation, or None on failure.
    """
    try:
        # Extract TAE bytes from the Binder (soulstruct handles DCX/BND/DivBinder)
        tae_entry = anibnd.find_entry_matching_name(r".*\.tae(\.dcx)?")
        tae_bytes = tae_entry.get_uncompressed_data()

        # Parse with standalone parser (struct-based, no constrata)
        from io import BytesIO
        parser = TAEParser()
        animations = parser._parse_tae(BytesIO(tae_bytes))
        logger.info(f"TAE parsed: {len(animations)} animations, skeleton: {parser.skeleton_name}")
        return animations
    except EntryNotFoundError:
        # Try loading from pre-extracted .tae file next to the anibnd
        tae_path = anibnd_path.with_suffix('').with_suffix('.tae')
        if not tae_path.exists():
            tae_path = anibnd_path.parent / (anibnd_path.name.split('.')[0] + '.tae')
        if tae_path.exists():
            logger.info(f"No TAE in binder, using extracted file: {tae_path}")
            try:
                parser = TAEParser()
                animations = parser.parse_file(str(tae_path))
                logger.info(f"TAE parsed: {len(animations)} animations")
                return animations
            except Exception as e2:
                logger.warning(f"Could not parse extracted TAE: {e2}")
                return None
        logger.warning("No TAE entry found in ANIBND and no .tae file found")
        return None
    except Exception as e:
        logger.warning(f"Could not extract TAE: {e}")
        return None


def export_tae_combat_events(animations: dict, output_path: Path, model_name: str = ""):
    """Export TAE events to JSON for UE5 AnimNotify creation.

    Args:
        animations: dict mapping anim_id -> TAEAnimation from tae_parser
        output_path: Path for output JSON file
        model_name: Character model name for metadata
    """
    result = {
        "model": model_name,
        "total_animations": len(animations),
        "animations": {}
    }

    for anim_id, anim in sorted(animations.items()):
        anim_data = anim.to_dict()
        result["animations"][str(anim_id)] = anim_data

    output_path.parent.mkdir(parents=True, exist_ok=True)
    with open(output_path, 'w') as f:
        json.dump(result, f, indent=2)

    # Count combat events
    total_iframes = sum(len(a.get_iframes()) for a in animations.values())
    total_hitboxes = sum(len(a.get_hitboxes()) for a in animations.values())
    total_hyper = sum(len(a.get_hyper_armor()) for a in animations.values())
    total_parry = sum(len(a.get_parry_windows()) for a in animations.values())

    logger.info(
        f"Exported TAE: {len(animations)} anims, "
        f"{total_iframes} i-frames, {total_hitboxes} hitboxes, "
        f"{total_hyper} hyper armor, {total_parry} parry windows -> {output_path}"
    )
    return result


# ============================================================================
# MAIN EXTRACTION PIPELINE
# ============================================================================

def extract_character_animations(
    anibnd_path: Path,
    output_dir: Path,
    model_name: str = "",
    anim_filter: str = "",
    export_fbx: bool = True,
    export_tae: bool = True,
    max_anims: int = 0,
    skip_existing: bool = False,
):
    """Extract all animations from a character's ANIBND file.

    Args:
        anibnd_path: Path to .anibnd or .anibnd.dcx file
        output_dir: Directory for output files
        model_name: Character model name (e.g., "c0000"). Auto-detected if empty.
        anim_filter: Glob pattern to filter animations (e.g., "a000_*")
        export_fbx: Whether to export FBX files
        export_tae: Whether to export TAE combat events
        max_anims: Maximum animations to export (0 = all)
        skip_existing: Skip animations with existing FBX files
    """
    start_time = time.perf_counter()

    # Auto-detect model name from filename
    if not model_name:
        model_name = anibnd_path.name.split('.')[0]  # e.g., "c0000"

    logger.info(f"=== Extracting animations for {model_name} ===")
    logger.info(f"ANIBND: {anibnd_path}")
    logger.info(f"Output: {output_dir}")

    # Create output directories
    fbx_dir = output_dir / model_name / "fbx"
    tae_dir = output_dir / model_name / "tae"
    fbx_dir.mkdir(parents=True, exist_ok=True)
    tae_dir.mkdir(parents=True, exist_ok=True)

    # Load ANIBND
    logger.info("Loading ANIBND...")
    if DivBinder:
        anibnd = DivBinder.from_path(anibnd_path)
    else:
        anibnd = Binder.from_path(anibnd_path)

    # For sub-ANIBNDs (c0000_aXx, cXXXX_divNN, etc.), skeleton is in main chr anibnd
    base_match = re.match(r"(c\d+)_", anibnd_path.name)
    if base_match:
        base_chr_id = base_match.group(1)
        skeleton_path = anibnd_path.parent / f"{base_chr_id}.anibnd.dcx"
        if not skeleton_path.exists():
            skeleton_path = anibnd_path.parent / f"{base_chr_id}.anibnd"
        if skeleton_path.exists():
            logger.info(f"Sub-ANIBND detected, loading skeleton from: {skeleton_path}")
            if DivBinder:
                skeleton_binder = DivBinder.from_path(skeleton_path)
            else:
                skeleton_binder = Binder.from_path(skeleton_path)
        else:
            logger.warning(f"Main ANIBND not found at {skeleton_path}, using current binder")
            skeleton_binder = anibnd
    else:
        skeleton_binder = anibnd

    # Extract compendium (if present)
    logger.info("Loading compendium...")
    compendium = None
    try:
        comp_entry = anibnd.find_entry_matching_name(r".*\.compendium")
        compendium = HKX.from_binder_entry(comp_entry)
        logger.info("Compendium loaded")
    except (EntryNotFoundError, Exception):
        logger.info("No compendium found (normal for some character types)")

    # Extract skeleton
    logger.info("Loading skeleton HKX...")
    skeleton_entry = skeleton_binder.find_entry_matching_name(
        r"skeleton\.hkx(\.dcx)?", flags=re.IGNORECASE
    )
    skeleton_hkx = read_skeleton_hkx_entry(skeleton_entry, compendium)
    logger.info(f"Skeleton has {len(skeleton_hkx.skeleton.bones)} bones")

    # Extract TAE if requested
    if export_tae:
        logger.info("Extracting TAE combat events...")
        # For sub-ANIBNDs, TAE is in the main chr anibnd
        tae_binder = skeleton_binder if base_match else anibnd
        tae_binder_path = skeleton_path if (base_match and skeleton_path.exists()) else anibnd_path
        tae_animations = extract_tae_from_anibnd(tae_binder, tae_binder_path)
        if tae_animations:
            # Use base chr ID for TAE filename (not div name)
            tae_name = base_match.group(1) if base_match else model_name
            export_tae_combat_events(tae_animations, tae_dir / f"{tae_name}_tae.json", tae_name)

    if not export_fbx:
        logger.info("FBX export disabled, done.")
        return

    # Create armature from skeleton
    armature_obj = create_armature_from_skeleton_hkx(skeleton_hkx, model_name)

    # Find all animation HKX entries
    anim_entries = anibnd.find_entries_matching_name(r"a.*\.hkx(\.dcx)?")
    logger.info(f"Found {len(anim_entries)} animation entries")

    # Apply filter if specified
    if anim_filter:
        filter_re = re.compile(anim_filter.replace("*", ".*"))
        anim_entries = [e for e in anim_entries if filter_re.match(e.name.split('.')[0])]
        logger.info(f"After filter '{anim_filter}': {len(anim_entries)} animations")

    # Limit count if specified
    if max_anims > 0:
        anim_entries = anim_entries[:max_anims]

    # Import and export each animation
    exported = 0
    failed = 0

    skipped = 0
    for i, entry in enumerate(anim_entries):
        anim_name = entry.name.split('.')[0]  # e.g., "a000_003000"

        # Skip if FBX already exists
        if skip_existing:
            fbx_path_check = fbx_dir / f"{anim_name}.fbx"
            if fbx_path_check.exists():
                skipped += 1
                continue

        try:
            logger.info(f"[{i+1}/{len(anim_entries)}] Processing {anim_name}...")

            # Read animation HKX
            anim_hkx = read_animation_hkx_entry(entry, compendium)

            # Import to Blender action
            action = import_animation_to_action(
                bpy.context,
                anim_hkx,
                skeleton_hkx,
                anim_name,
                armature_obj,
                model_name,
            )

            # Export as FBX
            fbx_path = fbx_dir / f"{anim_name}.fbx"
            export_action_as_fbx(armature_obj, action, fbx_path)

            exported += 1

            # Clean up action to save memory
            bpy.data.actions.remove(action)

            # Force garbage collection every 3 animations to prevent memory crash
            if (i + 1) % 3 == 0:
                import gc
                gc.collect()
                # Also purge orphan data blocks periodically
                if (i + 1) % 30 == 0:
                    bpy.ops.outliner.orphans_purge(do_local_ids=True, do_linked_ids=False, do_recursive=True)

        except Exception as e:
            logger.warning(f"Failed to process {anim_name}: {e}")
            traceback.print_exc()
            failed += 1

    elapsed = time.perf_counter() - start_time
    skip_msg = f", {skipped} skipped" if skipped > 0 else ""
    logger.info(f"=== Done: {exported} exported, {failed} failed{skip_msg} in {elapsed:.1f}s ===")

    # Write manifest
    manifest = {
        "model_name": model_name,
        "source": str(anibnd_path),
        "skeleton_bones": len(skeleton_hkx.skeleton.bones),
        "total_animations": len(anim_entries),
        "exported": exported,
        "failed": failed,
        "elapsed_seconds": round(elapsed, 1),
    }
    manifest_path = output_dir / model_name / "manifest.json"
    with open(manifest_path, 'w') as f:
        json.dump(manifest, f, indent=2)


def batch_extract(
    game_dir: Path,
    output_dir: Path,
    characters: list[str],
    **kwargs,
):
    """Extract animations for multiple characters from Elden Ring game directory."""
    chr_dir = game_dir / "chr"

    if not chr_dir.exists():
        logger.error(f"Character directory not found: {chr_dir}")
        return

    for char_id in characters:
        # Find ANIBND files for this character
        anibnd_files = list(chr_dir.glob(f"{char_id}*.anibnd*"))

        if not anibnd_files:
            logger.warning(f"No ANIBND found for {char_id} in {chr_dir}")
            continue

        for anibnd_path in anibnd_files:
            model_name = anibnd_path.name.split('.')[0]
            extract_character_animations(
                anibnd_path=anibnd_path,
                output_dir=output_dir,
                model_name=model_name,
                **kwargs,
            )


# ============================================================================
# CLI
# ============================================================================

def parse_args():
    """Parse arguments after Blender's '--' separator."""
    argv = sys.argv
    if "--" in argv:
        argv = argv[argv.index("--") + 1:]
    else:
        argv = []

    parser = argparse.ArgumentParser(
        description="Extract Elden Ring animations to FBX + TAE JSON"
    )

    # Single file mode
    parser.add_argument("--anibnd", type=str,
                       help="Path to .anibnd or .anibnd.dcx file")

    # Batch mode
    parser.add_argument("--game-dir", type=str,
                       help="Path to Elden Ring 'Game' directory for batch extraction")
    parser.add_argument("--characters", nargs="+", default=[],
                       help="Character IDs to extract (e.g., c0000 c3400)")

    # Common options
    parser.add_argument("--output", type=str, required=True,
                       help="Output directory for FBX and TAE files")
    parser.add_argument("--model-name", type=str, default="",
                       help="Override model name (auto-detected from filename)")
    parser.add_argument("--filter", type=str, default="",
                       help="Glob pattern to filter animations (e.g., 'a000_*')")
    parser.add_argument("--max-anims", type=int, default=0,
                       help="Maximum animations to export (0 = all)")
    parser.add_argument("--no-fbx", action="store_true",
                       help="Skip FBX export (only extract TAE)")
    parser.add_argument("--no-tae", action="store_true",
                       help="Skip TAE extraction")
    parser.add_argument("--skip-existing", action="store_true",
                       help="Skip animations that already have exported FBX files")

    return parser.parse_args(argv)


def main():
    args = parse_args()

    output_dir = Path(args.output)

    if args.game_dir:
        # Batch mode
        batch_extract(
            game_dir=Path(args.game_dir),
            output_dir=output_dir,
            characters=args.characters,
            anim_filter=args.filter,
            export_fbx=not args.no_fbx,
            export_tae=not args.no_tae,
            max_anims=args.max_anims,
        )
    elif args.anibnd:
        # Single file mode
        extract_character_animations(
            anibnd_path=Path(args.anibnd),
            output_dir=output_dir,
            model_name=args.model_name,
            anim_filter=args.filter,
            export_fbx=not args.no_fbx,
            export_tae=not args.no_tae,
            max_anims=args.max_anims,
            skip_existing=args.skip_existing,
        )
    else:
        print("Error: Must specify either --anibnd or --game-dir")
        sys.exit(1)


if __name__ == "__main__":
    main()
