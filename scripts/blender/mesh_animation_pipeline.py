"""
mesh_animation_pipeline.py - Reusable Blender-only pipeline for custom enemy mesh + animations.

Takes an AI-generated mesh zip, rigs it to the FLVER armature, retargets all 20 Elden Ring
animations via ARP, applies 13 forensic transforms, and generates side-by-side comparison renders.

5 Phases:
  1. Mesh Prep   - Unzip, import, height-match, auto-weight to FLVER armature
  2. ARP Retarget - Retarget 20 source animations (HKX) onto FLVER armature via ARP
  3. Forensic     - Apply 13 forensic transform layers, export final FBXes
  4. Renders      - Side-by-side comparison PNGs (new mesh vs FLVER reference)
  5. Report       - Validation summary

Usage:
  blender --background --python mesh_animation_pipeline.py -- --all
  blender --background --python mesh_animation_pipeline.py -- --phase 1
  blender --background --python mesh_animation_pipeline.py -- --phase 3 --phase 4

  # Custom paths:
  blender --background --python mesh_animation_pipeline.py -- \\
      --zip "C:/path/to/mesh.zip" \\
      --source-anims "C:/scripts/elden_ring_tools/output/c3100/c3100/fbx" \\
      --reference-blend "C:/scripts/elden_ring_tools/output/sentinel/rest_pose_comparison.blend" \\
      --output-dir "C:/scripts/elden_ring_tools/output/my_enemy" \\
      --all
"""

import bpy
import os
import sys
import math
import random
import gc
import argparse
import zipfile
import time
import glob as glob_mod
from mathutils import Quaternion, Euler, Matrix, Vector


# ============================================================
# Configuration / Defaults
# ============================================================

DEFAULT_ZIP = r"C:/Users/james/Downloads/Meshy_AI_Ironbound_Warlord_0220214142_texture_fbx.zip"
DEFAULT_SOURCE_ANIMS = r"C:/scripts/elden_ring_tools/output/c3100/c3100/fbx"
DEFAULT_REFERENCE_BLEND = r"C:/scripts/elden_ring_tools/output/sentinel/rest_pose_comparison.blend"
DEFAULT_OUTPUT_DIR = r"C:/scripts/elden_ring_tools/output/sentinel_pipeline"

# Animation mapping: source FBX name -> output name
ANIM_MAP = {
    "a000_000020": "Sentinel_Idle",
    "a000_002000": "Sentinel_Walk",
    "a000_002100": "Sentinel_Run",
    "a000_003000": "Sentinel_Attack01",
    "a000_003001": "Sentinel_Attack02",
    "a000_003017": "Sentinel_Attack03_Fast",
    "a000_004100": "Sentinel_HeavyAttack",
    "a000_010000": "Sentinel_HitReact",
    "a000_010001": "Sentinel_HitReact_Light",
    "a000_008030": "Sentinel_GuardHit",
    "a000_011010": "Sentinel_Death_Front",
    "a000_011060": "Sentinel_Death_Back",
    "a000_011070": "Sentinel_Death_Left",
    "a000_005000": "Sentinel_Dodge_Fwd",
    "a000_005001": "Sentinel_Dodge_Bwd",
    "a000_005002": "Sentinel_Dodge_Left",
    "a000_005003": "Sentinel_Dodge_Right",
    "a000_007000": "Sentinel_Guard",
    "a000_009210": "Sentinel_PoiseBreak_Start",
    "a000_009200": "Sentinel_PoiseBreak_Loop",
}

# Key animations for comparison renders (Phase 4)
RENDER_ANIMS = ["Sentinel_Idle", "Sentinel_Walk", "Sentinel_Attack01",
                "Sentinel_HitReact", "Sentinel_Death_Front"]

# FBX import/export settings
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
    use_armature_deform_only=False,
    bake_anim=True,
    bake_anim_use_all_bones=True,
    bake_anim_use_nla_strips=False,
    bake_anim_use_all_actions=False,
    bake_anim_force_startend_keying=True,
    bake_anim_simplify_factor=0.0,
)

# Bone scale overrides for proportion correction (arm shortening)
BONE_SCALE_OVERRIDES = {
    "L_UpperArm": 0.80, "R_UpperArm": 0.80,
    "L_UpArmTwist": 0.80, "R_UpArmTwist": 0.80,
    "L_UpArmTwist1": 0.80, "R_UpArmTwist1": 0.80,
    "L_Forearm": 0.85, "R_Forearm": 0.85,
}

# ============================================================
# Forensic Transform Configuration
# ============================================================

# Change 1: Static Rotation Offset (degrees)
BONE_OFFSETS_DEG = {
    'Spine':      (0.0,  5.0,  2.0),
    'Spine1':     (0.0,  3.0,  1.0),
    'Spine2':     (2.0,  2.0,  0.0),
    'Neck':       (0.0, -3.0,  0.0),
    'Head':       (-2.0, -2.0, 0.0),
    'L_Clavicle': (0.0,  0.0,  3.0),
    'R_Clavicle': (0.0,  0.0, -3.0),
    'L_UpperArm': (0.0,  0.0,  4.0),
    'R_UpperArm': (0.0,  0.0, -4.0),
    'Pelvis':     (0.0,  2.0,  1.0),
}

# Change 2: Noise Injection
NOISE_AMPLITUDE_DEG = 2.5
NUM_SINE_HARMONICS = 3
BONE_NOISE_WEIGHTS = {
    'Master': 0.0, 'Root': 0.0,
    'L_Foot_Target': 0.0, 'L_Foot_Target1': 0.0, 'L_Foot_Target2': 0.0,
    'R_Foot_Target': 0.0, 'R_Foot_Target1': 0.0, 'R_Foot_Target2': 0.0,
    'Pelvis': 0.15,
    'Spine': 1.0, 'Spine1': 0.9, 'Spine2': 0.8,
    'Neck': 0.7, 'Head': 0.6,
    'L_Clavicle': 0.5, 'R_Clavicle': 0.5,
    'L_Shoulder': 0.5, 'R_Shoulder': 0.5,
    'L_UpperArm': 0.7, 'R_UpperArm': 0.7,
    'L_Forearm': 0.8, 'R_Forearm': 0.8,
    'L_Elbow': 0.3, 'R_Elbow': 0.3,
    'L_Hand': 0.3, 'R_Hand': 0.3,
    'L_UpperLeg': 0.4, 'R_UpperLeg': 0.4,
    'L_LowerLeg': 0.5, 'R_LowerLeg': 0.5,
    'L_Foot': 0.2, 'R_Foot': 0.2,
    'L_Toes': 0.1, 'R_Toes': 0.1,
    'SpineArmor': 0.3, 'R_Sword': 0.0,
    'Jaw': 0.1, 'L_Eyelid': 0.0, 'R_Eyelid': 0.0,
}

# Change 3: Time Warp
TIMEWARP_STRENGTH = 0.15

# Change 4: Per-Animation Speed Variation
SPEED_VAR_MIN = 0.88
SPEED_VAR_MAX = 1.12
SPEED_VAR_SEED = 77742

# Change 5: Framerate Resample
SOURCE_FPS = 30.0
TARGET_FPS = 24.0

# Change 6: Bone Pruning (23 cosmetic bones to remove)
BONES_TO_PRUNE = {
    'L_Thigh_Skirt', 'R_Thigh_Skirt',
    'L_Knee_Skirt', 'R_Knee_Skirt',
    'L_Calf_Skirt', 'R_Calf_Skirt',
    'SpineArmor1', 'SpineArmor2',
    'Jaw', 'L_Eye', 'R_Eye', 'L_Eyelid', 'R_Eyelid',
    'L_Foot_Target1', 'L_Foot_Target2',
    'R_Foot_Target1', 'R_Foot_Target2',
    'L_Shoulder', 'R_Shoulder',
    'L_Hip', 'R_Hip',
    'Shoulder', 'Collar',
}

# Change 7: Rest Pose Perturbation (degrees)
REST_POSE_OFFSETS_DEG = {
    'Spine':      (0.0,  3.0,  1.5),
    'Spine1':     (1.0,  2.0,  0.0),
    'Spine2':     (0.5,  1.0,  0.0),
    'Neck':       (0.0, -2.0,  0.0),
    'Head':       (-1.5, -1.0, 0.0),
    'L_Clavicle': (0.0,  0.0,  2.0),
    'R_Clavicle': (0.0,  0.0, -2.0),
    'L_UpperArm': (0.0, -2.0,  3.0),
    'R_UpperArm': (0.0,  2.0, -3.0),
    'Pelvis':     (0.0,  1.5,  0.5),
    'L_Thigh':    (0.0,  0.0,  1.0),
    'R_Thigh':    (0.0,  0.0, -1.0),
}

# Change 8b: Per-Bone Phase Shift (between static offset and noise)
PHASE_SHIFT_SEED = 55123
PHASE_SHIFT_MAX_FRAMES = 2
PHASE_SHIFT_EXCLUDE = {'Master', 'Root', 'Pelvis',
    'L_Foot_Target', 'R_Foot_Target',
    'L_Foot_Target1', 'L_Foot_Target2',
    'R_Foot_Target1', 'R_Foot_Target2'}

# Change 2b: Low-Pass Butterworth Filter (after noise, before time warp)
LOWPASS_CUTOFF_HZ = 8.0    # Cutoff frequency
LOWPASS_ORDER = 2           # Butterworth order

# Change 5b: Keyframe Decimation (after resample, before bone prune)
DECIMATION_THRESHOLD_DEG = 0.15  # Max interpolation error to allow keyframe removal

# Change 7b: Bone Length Proportion Changes (extends rest pose perturbation)
BONE_LENGTH_SCALE = {
    'Spine': 1.03, 'Spine1': 1.03, 'Spine2': 1.03,
    'Neck': 0.96,
    'L_UpperArm': 0.97, 'R_UpperArm': 0.97,
    'L_Forearm': 1.02, 'R_Forearm': 1.02,
    'L_Thigh': 1.02, 'R_Thigh': 1.02,
    'L_Calf': 0.98, 'R_Calf': 0.98,
}

# Change 3b: Dodge/HitReact swap pairs - swap source FBX for these output names
# Key = output name that should load from value's source FBX (and vice versa)
OUTPUT_SWAP_PAIRS = {
    'Sentinel_Dodge_Left': 'Sentinel_Dodge_Right',
    # HitReact pair is asymmetric (different feel), adds misdirection
    'Sentinel_HitReact': 'Sentinel_HitReact_Light',
}

# Change 8: Bone Rename (FLVER -> UE5 Mannequin convention, 68 bones post-prune)
BONE_RENAME = {
    'Master':           'root',
    'Root':             'root_motion',
    'Pelvis':           'pelvis',
    'L_Thigh':          'thigh_l',
    'L_ThighTwist':     'thigh_twist_l',
    'L_ThighTwist1':    'thigh_twist_01_l',
    'L_Knee':           'knee_l',
    'L_Calf':           'calf_l',
    'L_CalfTwist':      'calf_twist_l',
    'L_CalfTwist1':     'calf_twist_01_l',
    'L_Foot':           'foot_l',
    'L_FootTwist':      'foot_twist_l',
    'L_Toe0':           'ball_l',
    'R_Thigh':          'thigh_r',
    'R_ThighTwist':     'thigh_twist_r',
    'R_ThighTwist1':    'thigh_twist_01_r',
    'R_Knee':           'knee_r',
    'R_Calf':           'calf_r',
    'R_CalfTwist':      'calf_twist_r',
    'R_CalfTwist1':     'calf_twist_01_r',
    'R_Foot':           'foot_r',
    'R_FootTwist':      'foot_twist_r',
    'R_Toe0':           'ball_r',
    'Spine':            'spine_01',
    'Spine1':           'spine_02',
    'Spine2':           'spine_03',
    'Neck':             'neck_01',
    'Head':             'head',
    'L_Clavicle':       'clavicle_l',
    'L_UpperArm':       'upperarm_l',
    'L_UpArmTwist':     'upperarm_twist_l',
    'L_UpArmTwist1':    'upperarm_twist_01_l',
    'L_Elbow':          'elbow_l',
    'L_Forearm':        'lowerarm_l',
    'L_ForeArmTwist':   'lowerarm_twist_l',
    'L_ForeArmTwist1':  'lowerarm_twist_01_l',
    'L_Hand':           'hand_l',
    'L_Shield':         'weapon_l',
    'L_Finger0':        'thumb_01_l',
    'L_Finger01':       'thumb_02_l',
    'L_Finger1':        'index_01_l',
    'L_Finger11':       'index_02_l',
    'L_Finger2':        'middle_01_l',
    'L_Finger21':       'middle_02_l',
    'L_Finger3':        'ring_01_l',
    'L_Finger31':       'ring_02_l',
    'L_Finger4':        'pinky_01_l',
    'L_Finger41':       'pinky_02_l',
    'R_Clavicle':       'clavicle_r',
    'R_UpperArm':       'upperarm_r',
    'R_UpArmTwist':     'upperarm_twist_r',
    'R_UpArmTwist1':    'upperarm_twist_01_r',
    'R_Elbow':          'elbow_r',
    'R_Forearm':        'lowerarm_r',
    'R_ForeArmTwist':   'lowerarm_twist_r',
    'R_ForeArmTwist1':  'lowerarm_twist_01_r',
    'R_Hand':           'hand_r',
    'R_Sword':          'weapon_r',
    'R_Finger0':        'thumb_01_r',
    'R_Finger01':       'thumb_02_r',
    'R_Finger1':        'index_01_r',
    'R_Finger11':       'index_02_r',
    'R_Finger2':        'middle_01_r',
    'R_Finger21':       'middle_02_r',
    'R_Finger3':        'ring_01_r',
    'R_Finger31':       'ring_02_r',
    'R_Finger4':        'pinky_01_r',
    'R_Finger41':       'pinky_02_r',
}


# ============================================================
# Shared Utilities
# ============================================================

def get_action_fcurves(action):
    """Get all FCurves from an action, handling Blender 5.0's layered API."""
    all_fcurves = []
    if hasattr(action, 'is_action_layered') and action.is_action_layered:
        for layer in action.layers:
            for strip in layer.strips:
                for bag in strip.channelbags:
                    all_fcurves.extend(bag.fcurves)
    elif hasattr(action, 'fcurves'):
        all_fcurves.extend(action.fcurves)
    return all_fcurves


def get_fcurves_collection(action):
    """Get the fcurves collection (for removal ops). Blender 5.0 channelbag version."""
    try:
        for layer in action.layers:
            for strip in layer.strips:
                for bag in strip.channelbags:
                    return bag.fcurves
    except Exception:
        pass
    if hasattr(action, 'fcurves'):
        return action.fcurves
    return None


def clean_orphans():
    """Remove orphan data blocks."""
    for coll in [bpy.data.meshes, bpy.data.armatures, bpy.data.actions,
                 bpy.data.cameras, bpy.data.lights]:
        for block in list(coll):
            if block.users == 0:
                coll.remove(block)


def import_fbx(filepath, **overrides):
    """Import FBX and return (armature, meshes, action)."""
    before_objs = set(bpy.data.objects.keys())
    before_acts = set(bpy.data.actions.keys())

    opts = dict(FBX_IMPORT)
    opts.update(overrides)
    bpy.ops.import_scene.fbx(filepath=filepath, **opts)

    new_objs = set(bpy.data.objects.keys()) - before_objs
    new_acts = set(bpy.data.actions.keys()) - before_acts

    arm = None
    meshes = []
    for name in sorted(new_objs):
        obj = bpy.data.objects[name]
        if obj.type == 'ARMATURE':
            arm = obj
        elif obj.type == 'MESH':
            meshes.append(obj)

    action = None
    if new_acts:
        action = bpy.data.actions[sorted(new_acts)[0]]
    if not action and arm and arm.animation_data and arm.animation_data.action:
        action = arm.animation_data.action

    return arm, meshes, action


def get_armature_bounds(arm):
    """Get vertical extent of the armature bones."""
    min_z = float('inf')
    max_z = float('-inf')
    for bone in arm.data.bones:
        head_world = arm.matrix_world @ bone.head_local
        tail_world = arm.matrix_world @ bone.tail_local
        min_z = min(min_z, head_world.z, tail_world.z)
        max_z = max(max_z, head_world.z, tail_world.z)
    return min_z, max_z


def get_mesh_bounds(mesh):
    """Get bounding box of the mesh in world space."""
    min_z = float('inf')
    max_z = float('-inf')
    min_x = float('inf')
    max_x = float('-inf')
    min_y = float('inf')
    max_y = float('-inf')
    for v in mesh.data.vertices:
        world_co = mesh.matrix_world @ v.co
        min_z = min(min_z, world_co.z)
        max_z = max(max_z, world_co.z)
        min_x = min(min_x, world_co.x)
        max_x = max(max_x, world_co.x)
        min_y = min(min_y, world_co.y)
        max_y = max(max_y, world_co.y)
    return {
        'min_z': min_z, 'max_z': max_z,
        'min_x': min_x, 'max_x': max_x,
        'min_y': min_y, 'max_y': max_y,
        'height': max_z - min_z,
        'width_x': max_x - min_x,
        'width_y': max_y - min_y,
    }


# ============================================================
# Forensic Math Helpers (from forensic_final_pipeline.py)
# ============================================================

def smooth_noise(frame_idx, total_frames, seed):
    rng = random.Random(seed)
    value = 0.0
    norm = 0.0
    for h in range(NUM_SINE_HARMONICS):
        phase = rng.uniform(0, 2 * math.pi)
        freq = (h + 1) * 0.7
        amp = 1.0 / (h + 1)
        t = frame_idx / max(total_frames - 1, 1)
        value += amp * math.sin(2 * math.pi * freq * t + phase)
        norm += amp
    return value / norm


def warp_time(t, strength):
    return t + strength * math.sin(2 * math.pi * t) / (2 * math.pi)


def lerp(a, b, t):
    return a + (b - a) * t


def slerp_quat(q1, q2, t):
    dot = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z
    if dot < 0:
        q2 = Quaternion((-q2.w, -q2.x, -q2.y, -q2.z))
        dot = -dot
    if dot > 0.9995:
        result = Quaternion((
            lerp(q1.w, q2.w, t), lerp(q1.x, q2.x, t),
            lerp(q1.y, q2.y, t), lerp(q1.z, q2.z, t),
        ))
        result.normalize()
        return result
    theta = math.acos(min(dot, 1.0))
    sin_theta = math.sin(theta)
    if sin_theta < 0.0001:
        return q1.copy()
    a_w = math.sin((1 - t) * theta) / sin_theta
    b_w = math.sin(t * theta) / sin_theta
    result = Quaternion((
        a_w * q1.w + b_w * q2.w, a_w * q1.x + b_w * q2.x,
        a_w * q1.y + b_w * q2.y, a_w * q1.z + b_w * q2.z,
    ))
    result.normalize()
    return result


def get_bone_noise_weight(name):
    if name in BONE_NOISE_WEIGHTS:
        return BONE_NOISE_WEIGHTS[name]
    if name.startswith(('L_Finger', 'R_Finger')):
        return 0.1
    if name.startswith(('L_Knee', 'R_Knee')):
        return 0.2
    return 0.15


def compute_speed_factor(anim_name):
    rng = random.Random(hash((SPEED_VAR_SEED, anim_name)) & 0xFFFFFFFF)
    return rng.uniform(SPEED_VAR_MIN, SPEED_VAR_MAX)


# ============================================================
# PHASE 1: Mesh Prep
# ============================================================

def phase1_mesh_prep(zip_path, reference_blend, output_dir):
    """Unzip mesh, import, rig to FLVER armature, save .blend."""
    print("\n" + "=" * 70)
    print(" PHASE 1: MESH PREP")
    print("=" * 70)

    phase1_dir = os.path.join(output_dir, "phase1")
    os.makedirs(phase1_dir, exist_ok=True)

    # Step 1: Unzip mesh FBX
    print(f"\n[1.1] Unzipping: {os.path.basename(zip_path)}")
    extract_dir = os.path.join(phase1_dir, "extracted")
    os.makedirs(extract_dir, exist_ok=True)

    with zipfile.ZipFile(zip_path, 'r') as zf:
        zf.extractall(extract_dir)
        extracted = zf.namelist()
        print(f"  Extracted {len(extracted)} files")

    # Find FBX in extracted files
    mesh_fbx = None
    for root, dirs, files in os.walk(extract_dir):
        for f in files:
            if f.lower().endswith('.fbx'):
                mesh_fbx = os.path.join(root, f)
                break
        if mesh_fbx:
            break

    if not mesh_fbx:
        print("  FATAL: No FBX found in zip!")
        return False
    print(f"  Mesh FBX: {mesh_fbx}")
    size_mb = os.path.getsize(mesh_fbx) / (1024 * 1024)
    print(f"  Size: {size_mb:.1f} MB")

    # Step 2: Load reference blend (has FLVER armature + Sentinel_mesh as reference)
    print(f"\n[1.2] Loading reference blend: {os.path.basename(reference_blend)}")
    bpy.ops.wm.open_mainfile(filepath=reference_blend)

    # Find FLVER armature
    flver_arm = None
    reference_mesh = None
    for obj in bpy.data.objects:
        if obj.type == 'ARMATURE' and 'FLVER' in obj.name:
            flver_arm = obj
        elif obj.type == 'MESH' and obj.name == 'Sentinel_mesh':
            reference_mesh = obj

    if not flver_arm:
        for obj in bpy.data.objects:
            if obj.type == 'ARMATURE':
                flver_arm = obj
                break

    if not flver_arm:
        print("  FATAL: No FLVER armature in reference blend!")
        return False

    print(f"  FLVER armature: '{flver_arm.name}' ({len(flver_arm.data.bones)} bones)")
    if reference_mesh:
        print(f"  Reference mesh: '{reference_mesh.name}' ({len(reference_mesh.data.vertices)} verts)")
        # Hide reference mesh so auto-weights only affect new mesh
        reference_mesh.hide_set(True)
        reference_mesh.hide_viewport = True
        reference_mesh.hide_render = True

    # Hide any other meshes
    for obj in bpy.data.objects:
        if obj.type == 'MESH' and obj != reference_mesh:
            obj.hide_set(True)
            obj.hide_viewport = True

    # Step 3: Import AI mesh
    print(f"\n[1.3] Importing AI mesh...")
    existing_objs = set(obj.name for obj in bpy.data.objects)
    bpy.ops.import_scene.fbx(filepath=mesh_fbx, use_anim=False,
                              ignore_leaf_bones=True,
                              automatic_bone_orientation=False)

    new_mesh = None
    new_arm_to_delete = None
    for obj in bpy.data.objects:
        if obj.name not in existing_objs:
            if obj.type == 'MESH':
                new_mesh = obj
            elif obj.type == 'ARMATURE':
                new_arm_to_delete = obj

    # Delete any armature that came with the AI mesh
    if new_arm_to_delete:
        # Unparent mesh first
        if new_mesh and new_mesh.parent == new_arm_to_delete:
            new_mesh.parent = None
            new_mesh.matrix_parent_inverse.identity()
        bpy.data.objects.remove(new_arm_to_delete, do_unlink=True)

    if not new_mesh:
        print("  FATAL: No mesh found in AI FBX!")
        return False

    print(f"  AI mesh: '{new_mesh.name}', {len(new_mesh.data.vertices)} verts, "
          f"{len(new_mesh.data.polygons)} polys")

    # Step 4: Height-match and center mesh
    print(f"\n[1.4] Height-matching mesh to FLVER armature...")
    arm_min_z, arm_max_z = get_armature_bounds(flver_arm)
    arm_height = arm_max_z - arm_min_z

    mesh_bounds = get_mesh_bounds(new_mesh)
    mesh_height = mesh_bounds['height']

    print(f"  Armature height: {arm_height:.4f}")
    print(f"  Mesh height: {mesh_height:.4f}")

    if mesh_height < 0.001:
        print("  FATAL: Mesh has zero height!")
        return False

    scale_factor = arm_height / mesh_height
    print(f"  Scale factor: {scale_factor:.4f}")

    new_mesh.scale = (scale_factor, scale_factor, scale_factor)
    bpy.context.view_layer.update()

    # Apply scale
    bpy.ops.object.select_all(action='DESELECT')
    new_mesh.select_set(True)
    bpy.context.view_layer.objects.active = new_mesh
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    bpy.context.view_layer.update()

    # Center and align mesh base to armature base
    mesh_bounds2 = get_mesh_bounds(new_mesh)
    z_offset = arm_min_z - mesh_bounds2['min_z']
    mesh_center_x = (mesh_bounds2['min_x'] + mesh_bounds2['max_x']) / 2.0
    mesh_center_y = (mesh_bounds2['min_y'] + mesh_bounds2['max_y']) / 2.0

    new_mesh.location.x -= mesh_center_x
    new_mesh.location.y -= mesh_center_y
    new_mesh.location.z += z_offset
    bpy.context.view_layer.update()

    # Apply location
    bpy.ops.object.select_all(action='DESELECT')
    new_mesh.select_set(True)
    bpy.context.view_layer.objects.active = new_mesh
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    bpy.context.view_layer.update()

    final_bounds = get_mesh_bounds(new_mesh)
    print(f"  Final mesh: Z=[{final_bounds['min_z']:.4f}, {final_bounds['max_z']:.4f}] "
          f"height={final_bounds['height']:.4f}")

    # Step 5: Apply bone scale overrides
    if BONE_SCALE_OVERRIDES:
        print(f"\n[1.5] Applying {len(BONE_SCALE_OVERRIDES)} bone scale overrides...")
        bpy.context.view_layer.objects.active = flver_arm
        bpy.ops.object.mode_set(mode='EDIT')

        for bone_name, scale in BONE_SCALE_OVERRIDES.items():
            ebone = flver_arm.data.edit_bones.get(bone_name)
            if ebone:
                old_len = ebone.length
                direction = (ebone.tail - ebone.head).normalized()
                ebone.tail = ebone.head + direction * (old_len * scale)
                print(f"    {bone_name}: {old_len:.4f} -> {ebone.length:.4f} (x{scale})")

        bpy.ops.object.mode_set(mode='OBJECT')
        bpy.context.view_layer.update()

    # Step 6: Auto-weight mesh to FLVER armature
    print(f"\n[1.6] Parenting with automatic weights...")
    bpy.ops.object.select_all(action='DESELECT')
    new_mesh.select_set(True)
    flver_arm.select_set(True)
    bpy.context.view_layer.objects.active = flver_arm

    try:
        bpy.ops.object.parent_set(type='ARMATURE_AUTO')
        print("  Auto-weights applied successfully")
    except RuntimeError as e:
        print(f"  WARNING: Auto-weights failed ({e}), trying envelope...")
        try:
            bpy.ops.object.parent_set(type='ARMATURE_ENVELOPE')
            print("  Envelope weights applied as fallback")
        except RuntimeError as e2:
            print(f"  FATAL: Both weight methods failed: {e2}")
            return False

    # Step 7: Remove zero-weight vertex groups
    print(f"\n[1.7] Cleaning zero-weight vertex groups...")
    to_remove = []
    for vg in new_mesh.vertex_groups:
        has_weight = False
        for v in new_mesh.data.vertices:
            for g in v.groups:
                if g.group == vg.index and g.weight > 0.0001:
                    has_weight = True
                    break
            if has_weight:
                break
        if not has_weight:
            to_remove.append(vg.name)

    for name in to_remove:
        vg = new_mesh.vertex_groups.get(name)
        if vg:
            new_mesh.vertex_groups.remove(vg)

    print(f"  Removed {len(to_remove)} zero-weight VGs")
    print(f"  Remaining VGs: {len(new_mesh.vertex_groups)}")

    # Verify weights
    weighted_verts = sum(1 for v in new_mesh.data.vertices if len(v.groups) > 0)
    total_verts = len(new_mesh.data.vertices)
    pct = (weighted_verts / total_verts * 100) if total_verts > 0 else 0
    print(f"  Weighted vertices: {weighted_verts}/{total_verts} ({pct:.1f}%)")

    # Step 8: Rename mesh for pipeline consistency
    new_mesh.name = "Pipeline_mesh"

    # Step 9: Save rigged blend
    output_blend = os.path.join(phase1_dir, "rigged_mesh.blend")
    bpy.ops.wm.save_as_mainfile(filepath=output_blend)
    print(f"\n  Saved: {output_blend}")

    print(f"\n  PHASE 1 COMPLETE")
    print(f"  Mesh: {total_verts} verts, {len(new_mesh.vertex_groups)} VGs, "
          f"{pct:.1f}% weighted")
    return True


# ============================================================
# PHASE 2: ARP Retarget
# ============================================================

def enable_arp():
    """Enable Auto-Rig Pro addon."""
    try:
        bpy.ops.preferences.addon_enable(module='auto_rig_pro-master')
        print("  ARP: enabled")
        return True
    except Exception as e:
        print(f"  ARP: FAILED to enable - {e}")
        return False


def strip_non_root_translations(action, source_arm):
    """Strip ALL bone translations except root-level bones after ARP retarget."""
    ROOT_BONES = {"Pelvis", "Master", "c3100", "Root"}
    target_fcurves = get_action_fcurves(action)

    stripped = 0
    kept = 0
    for fc in target_fcurves:
        if '.location' not in fc.data_path:
            continue
        if 'pose.bones["' not in fc.data_path:
            continue
        bone_name = fc.data_path.split('pose.bones["')[1].split('"]')[0]
        if bone_name in ROOT_BONES:
            kept += 1
            continue
        has_nonzero = any(abs(kp.co[1]) > 1e-6 for kp in fc.keyframe_points)
        if has_nonzero:
            for kp in fc.keyframe_points:
                kp.co[1] = 0.0
                kp.handle_left[1] = 0.0
                kp.handle_right[1] = 0.0
            stripped += 1

    print(f"    Translation strip: zeroed {stripped} FCurves, kept {kept} root FCurves")


def retarget_with_arp(source_arm, target_arm, source_action):
    """Use ARP Remap to retarget source_action onto target_arm."""
    scene = bpy.context.scene

    if source_arm.animation_data is None:
        source_arm.animation_data_create()
    source_arm.animation_data.action = source_action

    scene.source_rig = source_arm.name
    scene.target_rig = target_arm.name
    scene.source_action = source_action.name

    bpy.context.view_layer.objects.active = target_arm
    target_arm.select_set(True)

    result = bpy.ops.arp.build_bones_list()
    if result != {'FINISHED'}:
        print(f"    WARNING: build_bones_list returned {result}")

    # Check bone map and set root
    mapped = 0
    unmapped = 0
    root_set = False
    for item in scene.bones_map_v2:
        if item.name and item.name != 'None':
            mapped += 1
        else:
            unmapped += 1
        if item.set_as_root:
            root_set = True

    print(f"    ARP bone map: {mapped} mapped, {unmapped} unmapped, root_set={root_set}")

    if not root_set:
        for item in scene.bones_map_v2:
            if item.source_bone and 'pelvis' in item.source_bone.lower():
                item.set_as_root = True
                root_set = True
                break
        if not root_set and len(scene.bones_map_v2) > 0:
            for item in scene.bones_map_v2:
                if item.name and item.name != 'None':
                    item.set_as_root = True
                    break

    frame_start = int(source_action.frame_range[0])
    frame_end = int(source_action.frame_range[1])

    result = bpy.ops.arp.retarget(frame_start=frame_start, frame_end=frame_end)

    if result != {'FINISHED'}:
        print(f"    WARNING: retarget returned {result}")
        return None

    if target_arm.animation_data and target_arm.animation_data.action:
        retargeted = target_arm.animation_data.action
        print(f"    Retargeted: '{retargeted.name}' ({frame_end - frame_start + 1} frames)")
        return retargeted

    return None


def phase2_arp_retarget(source_anims_dir, output_dir):
    """Load rigged mesh, retarget all 20 animations via ARP, export FBXes."""
    print("\n" + "=" * 70)
    print(" PHASE 2: ARP RETARGET")
    print("=" * 70)

    phase1_blend = os.path.join(output_dir, "phase1", "rigged_mesh.blend")
    phase2_dir = os.path.join(output_dir, "phase2", "retargeted")
    os.makedirs(phase2_dir, exist_ok=True)

    if not os.path.exists(phase1_blend):
        print(f"  FATAL: Phase 1 output not found: {phase1_blend}")
        return False

    # Enable ARP first
    if not enable_arp():
        print("  FATAL: Cannot enable Auto-Rig Pro")
        return False

    # Load the rigged mesh blend
    print(f"\n[2.1] Loading rigged blend: {phase1_blend}")
    bpy.ops.wm.open_mainfile(filepath=phase1_blend)

    # Re-enable ARP after file load
    enable_arp()

    # Find FLVER armature and pipeline mesh
    flver_arm = None
    pipeline_mesh = None
    for obj in bpy.data.objects:
        if obj.type == 'ARMATURE' and 'FLVER' in obj.name:
            flver_arm = obj
        elif obj.type == 'MESH' and obj.name == 'Pipeline_mesh':
            pipeline_mesh = obj

    if not flver_arm:
        for obj in bpy.data.objects:
            if obj.type == 'ARMATURE':
                flver_arm = obj
                break

    if not flver_arm:
        print("  FATAL: No armature in rigged blend!")
        return False

    print(f"  FLVER armature: '{flver_arm.name}' ({len(flver_arm.data.bones)} bones)")
    if pipeline_mesh:
        print(f"  Pipeline mesh: '{pipeline_mesh.name}'")

    # Remember "our" objects
    keep_objects = {flver_arm.name}
    if pipeline_mesh:
        keep_objects.add(pipeline_mesh.name)
    for obj in bpy.data.objects:
        if obj.parent == flver_arm and obj.type == 'MESH':
            keep_objects.add(obj.name)

    # Set scene to 24fps for Layer 8 resample
    bpy.context.scene.render.fps = int(TARGET_FPS)
    bpy.context.scene.render.fps_base = 1.0

    # Process each animation
    print(f"\n[2.2] Processing {len(ANIM_MAP)} animations...")
    results = []
    failures = []

    for i, (source_name, output_name) in enumerate(ANIM_MAP.items()):
        source_fbx = os.path.join(source_anims_dir, f"{source_name}.fbx")
        print(f"\n  [{i+1}/{len(ANIM_MAP)}] {source_name} -> {output_name}")

        if not os.path.exists(source_fbx):
            print(f"    SKIP: FBX not found: {source_fbx}")
            failures.append((source_name, "FBX not found"))
            continue

        # Import source animation FBX
        source_arm, source_meshes, source_action = import_fbx(source_fbx)

        if not source_arm:
            print(f"    SKIP: No armature in FBX")
            failures.append((source_name, "No armature"))
            continue

        if not source_action:
            if source_arm.animation_data and source_arm.animation_data.action:
                source_action = source_arm.animation_data.action
            else:
                print(f"    SKIP: No animation action in FBX")
                failures.append((source_name, "No action"))
                for m in source_meshes:
                    bpy.data.objects.remove(m, do_unlink=True)
                bpy.data.objects.remove(source_arm, do_unlink=True)
                clean_orphans()
                continue

        print(f"    Source: '{source_arm.name}', {len(source_arm.data.bones)} bones, "
              f"action='{source_action.name}' "
              f"({int(source_action.frame_range[1] - source_action.frame_range[0] + 1)} frames)")

        # Retarget via ARP
        retargeted_action = retarget_with_arp(source_arm, flver_arm, source_action)

        if retargeted_action:
            strip_non_root_translations(retargeted_action, source_arm)
            retargeted_action.name = output_name

            # Export: armature + mesh + baked action
            if flver_arm.animation_data is None:
                flver_arm.animation_data_create()
            flver_arm.animation_data.action = retargeted_action

            bpy.ops.object.select_all(action='DESELECT')
            flver_arm.select_set(True)
            if pipeline_mesh:
                pipeline_mesh.select_set(True)
            bpy.context.view_layer.objects.active = flver_arm

            frame_start = int(retargeted_action.frame_range[0])
            frame_end = int(retargeted_action.frame_range[1])
            bpy.context.scene.frame_start = frame_start
            bpy.context.scene.frame_end = frame_end

            filepath = os.path.join(phase2_dir, f"{output_name}.fbx")
            bpy.ops.export_scene.fbx(filepath=filepath, bake_anim_step=1.0, **FBX_EXPORT)
            print(f"    Exported: {output_name}.fbx ({frame_end - frame_start + 1} frames)")
            results.append((output_name, filepath))
        else:
            print(f"    FAILED: Retarget produced no action")
            failures.append((source_name, "Retarget failed"))

        # Clean up source objects
        for m in source_meshes:
            if m.name in bpy.data.objects:
                bpy.data.objects.remove(m, do_unlink=True)
        if source_arm.name in bpy.data.objects:
            bpy.data.objects.remove(source_arm, do_unlink=True)
        if source_action and source_action.name in bpy.data.actions:
            bpy.data.actions.remove(source_action)
        if flver_arm.animation_data:
            flver_arm.animation_data.action = None

        clean_orphans()

        # GC every 5 animations
        if (i + 1) % 5 == 0:
            gc.collect()
            print(f"    (GC after {i+1} anims)")

    # Export mesh-only FBX
    print(f"\n[2.3] Exporting mesh-only FBX...")
    if flver_arm.animation_data:
        flver_arm.animation_data.action = None
    bpy.ops.object.select_all(action='DESELECT')
    flver_arm.select_set(True)
    if pipeline_mesh:
        pipeline_mesh.select_set(True)
    bpy.context.view_layer.objects.active = flver_arm

    mesh_fbx = os.path.join(phase2_dir, "SKM_Mesh.fbx")
    export_opts = dict(FBX_EXPORT)
    export_opts['bake_anim'] = False
    bpy.ops.export_scene.fbx(filepath=mesh_fbx, **export_opts)
    print(f"  Exported mesh: SKM_Mesh.fbx")

    # Summary
    print(f"\n  PHASE 2 COMPLETE: {len(results)}/{len(ANIM_MAP)} succeeded")
    if failures:
        for name, reason in failures:
            print(f"    FAIL {name}: {reason}")
    return len(failures) == 0


# ============================================================
# PHASE 3: Forensic Transforms
# ============================================================

def apply_static_offset(action):
    """Change 1: Static rotation offset on key bones."""
    fcurves = get_fcurves_collection(action)
    if not fcurves:
        return 0

    bone_rot = {}
    for fc in fcurves:
        dp = fc.data_path
        if 'pose.bones[' in dp and 'rotation_quaternion' in dp:
            try:
                bone_name = dp.split('"')[1]
            except IndexError:
                continue
            bone_rot.setdefault(bone_name, {})[fc.array_index] = fc

    modified = 0
    for bone_name, offset_deg in BONE_OFFSETS_DEG.items():
        if bone_name not in bone_rot:
            continue
        rot_fcs = bone_rot[bone_name]
        if len(rot_fcs) < 4:
            continue

        offset_q = Euler((math.radians(offset_deg[0]),
                          math.radians(offset_deg[1]),
                          math.radians(offset_deg[2])), 'XYZ').to_quaternion()

        for ki in range(len(rot_fcs[0].keyframe_points)):
            orig_q = Quaternion((
                rot_fcs[0].keyframe_points[ki].co[1],
                rot_fcs[1].keyframe_points[ki].co[1],
                rot_fcs[2].keyframe_points[ki].co[1],
                rot_fcs[3].keyframe_points[ki].co[1],
            ))
            result = orig_q @ offset_q
            result.normalize()
            rot_fcs[0].keyframe_points[ki].co[1] = result.w
            rot_fcs[1].keyframe_points[ki].co[1] = result.x
            rot_fcs[2].keyframe_points[ki].co[1] = result.y
            rot_fcs[3].keyframe_points[ki].co[1] = result.z
        modified += 1

    for fc in fcurves:
        fc.update()
    return modified


def apply_phase_shift(action):
    """Change 8b: Per-bone phase shift - offset keyframe times by 0-2 frames per bone.

    Deterministic shift based on bone name hash. Root/pelvis/IK bones get 0 shift.
    Creates subtle desync within kinetic chains.
    """
    fcurves = get_fcurves_collection(action)
    if not fcurves:
        return 0

    # Group fcurves by bone name
    bone_fcs = {}
    for fc in fcurves:
        dp = fc.data_path
        if 'pose.bones["' in dp:
            try:
                bone_name = dp.split('"')[1]
            except IndexError:
                continue
            bone_fcs.setdefault(bone_name, []).append(fc)

    modified = 0
    for bone_name, fcs in bone_fcs.items():
        if bone_name in PHASE_SHIFT_EXCLUDE:
            continue

        # Deterministic shift: 0, 1, or 2 frames
        shift = hash((PHASE_SHIFT_SEED, bone_name)) % (PHASE_SHIFT_MAX_FRAMES + 1)
        if shift == 0:
            continue

        for fc in fcs:
            for kp in fc.keyframe_points:
                kp.co[0] += shift
            fc.update()
        modified += 1

    return modified


def apply_noise(action, anim_seed):
    """Change 2: Per-bone smooth noise injection, bone-weighted."""
    fcurves = get_fcurves_collection(action)
    if not fcurves:
        return 0

    bone_rot = {}
    for fc in fcurves:
        dp = fc.data_path
        if 'pose.bones[' in dp and 'rotation_quaternion' in dp:
            try:
                bone_name = dp.split('"')[1]
            except IndexError:
                continue
            bone_rot.setdefault(bone_name, {})[fc.array_index] = fc

    modified = 0
    for bone_name, rot_fcs in bone_rot.items():
        weight = get_bone_noise_weight(bone_name)
        if weight < 0.001 or len(rot_fcs) < 4:
            continue

        amplitude_rad = math.radians(NOISE_AMPLITUDE_DEG * weight)
        bone_seed = hash((anim_seed, bone_name)) & 0xFFFFFFFF
        num_kps = len(rot_fcs[0].keyframe_points)
        if num_kps == 0:
            continue

        for ki in range(num_kps):
            orig_q = Quaternion((
                rot_fcs[0].keyframe_points[ki].co[1],
                rot_fcs[1].keyframe_points[ki].co[1],
                rot_fcs[2].keyframe_points[ki].co[1],
                rot_fcs[3].keyframe_points[ki].co[1],
            ))
            nx = smooth_noise(ki, num_kps, bone_seed + 1) * amplitude_rad
            ny = smooth_noise(ki, num_kps, bone_seed + 2) * amplitude_rad
            nz = smooth_noise(ki, num_kps, bone_seed + 3) * amplitude_rad
            noise_q = Euler((nx, ny, nz), 'XYZ').to_quaternion()
            noised = orig_q @ noise_q
            noised.normalize()
            rot_fcs[0].keyframe_points[ki].co[1] = noised.w
            rot_fcs[1].keyframe_points[ki].co[1] = noised.x
            rot_fcs[2].keyframe_points[ki].co[1] = noised.y
            rot_fcs[3].keyframe_points[ki].co[1] = noised.z
        modified += 1

    for fc in fcurves:
        fc.update()
    return modified


def apply_lowpass_filter(action, fps):
    """Change 2b: Low-pass Butterworth filter on rotation quaternion channels.

    Destroys high-frequency source fingerprint while preserving motion.
    Uses scipy.signal if available, falls back to simple moving average.
    """
    fcurves = get_fcurves_collection(action)
    if not fcurves:
        return 0

    # Group rotation fcurves by bone
    bone_rot = {}
    for fc in fcurves:
        dp = fc.data_path
        if 'pose.bones["' in dp and 'rotation_quaternion' in dp:
            try:
                bone_name = dp.split('"')[1]
            except IndexError:
                continue
            bone_rot.setdefault(bone_name, {})[fc.array_index] = fc

    # Try scipy, fall back to moving average
    use_scipy = False
    try:
        from scipy.signal import butter, sosfiltfilt
        # Design Butterworth filter: normalized cutoff = cutoff / (fps/2)
        nyquist = fps / 2.0
        norm_cutoff = min(LOWPASS_CUTOFF_HZ / nyquist, 0.99)  # Clamp below Nyquist
        sos = butter(LOWPASS_ORDER, norm_cutoff, btype='low', output='sos')
        use_scipy = True
    except ImportError:
        pass

    modified = 0
    for bone_name, rot_fcs in bone_rot.items():
        if len(rot_fcs) < 4:
            continue

        num_kps = len(rot_fcs[0].keyframe_points)
        if num_kps < 6:  # Need enough samples for filtering
            continue

        # Extract quaternion channels as arrays
        channels = [[], [], [], []]  # w, x, y, z
        for ki in range(num_kps):
            for idx in range(4):
                channels[idx].append(rot_fcs[idx].keyframe_points[ki].co[1])

        # Ensure quaternion continuity (flip signs to avoid discontinuities)
        for ki in range(1, num_kps):
            dot = sum(channels[c][ki] * channels[c][ki-1] for c in range(4))
            if dot < 0:
                for c in range(4):
                    channels[c][ki] = -channels[c][ki]

        if use_scipy:
            import numpy as np
            for c in range(4):
                arr = np.array(channels[c], dtype=np.float64)
                channels[c] = sosfiltfilt(sos, arr).tolist()
        else:
            # Simple moving average fallback (window=5)
            window = 5
            half_w = window // 2
            for c in range(4):
                orig = list(channels[c])
                for ki in range(num_kps):
                    lo = max(0, ki - half_w)
                    hi = min(num_kps, ki + half_w + 1)
                    channels[c][ki] = sum(orig[lo:hi]) / (hi - lo)

        # Normalize and write back
        for ki in range(num_kps):
            w, x, y, z = channels[0][ki], channels[1][ki], channels[2][ki], channels[3][ki]
            mag = math.sqrt(w*w + x*x + y*y + z*z)
            if mag > 0.0001:
                w /= mag; x /= mag; y /= mag; z /= mag
            rot_fcs[0].keyframe_points[ki].co[1] = w
            rot_fcs[1].keyframe_points[ki].co[1] = x
            rot_fcs[2].keyframe_points[ki].co[1] = y
            rot_fcs[3].keyframe_points[ki].co[1] = z
        modified += 1

    for fc in fcurves:
        fc.update()
    return modified


def apply_timewarp(action, strength):
    """Change 3: Non-linear time remap with proper quaternion SLERP."""
    fcurves = get_fcurves_collection(action)
    if not fcurves:
        return 0

    groups = {}
    for fc in fcurves:
        groups.setdefault(fc.data_path, {})[fc.array_index] = fc

    total = 0
    for dp, idx_fcs in groups.items():
        any_fc = list(idx_fcs.values())[0]
        num_kps = len(any_fc.keyframe_points)
        if num_kps < 3:
            continue

        orig_data = []
        for ki in range(num_kps):
            frame = any_fc.keyframe_points[ki].co[0]
            values = {}
            for idx, fc in idx_fcs.items():
                if ki < len(fc.keyframe_points):
                    values[idx] = fc.keyframe_points[ki].co[1]
            orig_data.append((frame, values))

        frame_start = orig_data[0][0]
        frame_end = orig_data[-1][0]
        frame_range = frame_end - frame_start
        if frame_range < 1:
            continue

        is_quat = 'rotation_quaternion' in dp

        for ki in range(num_kps):
            t = (orig_data[ki][0] - frame_start) / frame_range
            t_w = warp_time(t, strength)
            warped_frame = frame_start + t_w * frame_range

            src_ki = 0
            for j in range(len(orig_data) - 1):
                if orig_data[j][0] <= warped_frame <= orig_data[j + 1][0]:
                    src_ki = j
                    break
            else:
                src_ki = len(orig_data) - 2
            src_ki = max(0, min(src_ki, len(orig_data) - 2))

            f0 = orig_data[src_ki][0]
            f1 = orig_data[src_ki + 1][0]
            blend = (warped_frame - f0) / max(f1 - f0, 0.001)
            blend = max(0.0, min(1.0, blend))

            vals0 = orig_data[src_ki][1]
            vals1 = orig_data[src_ki + 1][1]

            if is_quat and all(i in vals0 and i in vals1 for i in range(4)):
                q0 = Quaternion((vals0[0], vals0[1], vals0[2], vals0[3]))
                q1_q = Quaternion((vals1[0], vals1[1], vals1[2], vals1[3]))
                qi = slerp_quat(q0, q1_q, blend)
                for idx, fc in idx_fcs.items():
                    if ki < len(fc.keyframe_points):
                        fc.keyframe_points[ki].co[1] = [qi.w, qi.x, qi.y, qi.z][idx]
            else:
                for idx, fc in idx_fcs.items():
                    if ki < len(fc.keyframe_points) and idx in vals0 and idx in vals1:
                        fc.keyframe_points[ki].co[1] = lerp(vals0[idx], vals1[idx], blend)
        total += 1

    for fc in fcurves:
        fc.update()
    return total


def apply_speed_variation(action, speed_factor):
    """Change 4: Per-animation speed scaling with proper SLERP resampling."""
    fcurves = get_fcurves_collection(action)
    if not fcurves:
        return 0, 0

    fc_list = list(fcurves)
    if not fc_list:
        return 0, 0

    frame_start = float('inf')
    frame_end = float('-inf')
    for fc in fc_list:
        if len(fc.keyframe_points) > 0:
            frame_start = min(frame_start, fc.keyframe_points[0].co[0])
            frame_end = max(frame_end, fc.keyframe_points[-1].co[0])

    if frame_start >= frame_end:
        return 0, 0

    orig_count = int(frame_end - frame_start) + 1
    time_scale = 1.0 / speed_factor
    new_range = (frame_end - frame_start) * time_scale
    new_count = max(2, round(new_range) + 1)

    groups = {}
    for fc in fc_list:
        groups.setdefault(fc.data_path, {})[fc.array_index] = fc

    for dp, idx_fcs in groups.items():
        any_fc = list(idx_fcs.values())[0]
        num_orig = len(any_fc.keyframe_points)
        if num_orig < 2:
            continue

        orig_data = []
        for ki in range(num_orig):
            frame = any_fc.keyframe_points[ki].co[0]
            values = {}
            for idx, fc in idx_fcs.items():
                if ki < len(fc.keyframe_points):
                    values[idx] = fc.keyframe_points[ki].co[1]
            orig_data.append((frame, values))

        is_quat = 'rotation_quaternion' in dp

        new_kfs = []
        for ni in range(new_count):
            new_frame = frame_start + ni
            t = ni / max(new_count - 1, 1)
            source_frame = frame_start + t * (frame_end - frame_start)

            src_ki = 0
            for j in range(len(orig_data) - 1):
                if orig_data[j][0] <= source_frame <= orig_data[j + 1][0]:
                    src_ki = j
                    break
            else:
                src_ki = len(orig_data) - 2
            src_ki = max(0, min(src_ki, len(orig_data) - 2))

            f0 = orig_data[src_ki][0]
            f1 = orig_data[src_ki + 1][0]
            blend_t = (source_frame - f0) / max(f1 - f0, 0.001)
            blend_t = max(0.0, min(1.0, blend_t))

            vals0 = orig_data[src_ki][1]
            vals1 = orig_data[src_ki + 1][1]

            new_vals = {}
            if is_quat and all(i in vals0 and i in vals1 for i in range(4)):
                q0 = Quaternion((vals0[0], vals0[1], vals0[2], vals0[3]))
                q1_q = Quaternion((vals1[0], vals1[1], vals1[2], vals1[3]))
                qi = slerp_quat(q0, q1_q, blend_t)
                new_vals = {0: qi.w, 1: qi.x, 2: qi.y, 3: qi.z}
            else:
                for idx in vals0:
                    if idx in vals1:
                        new_vals[idx] = lerp(vals0[idx], vals1[idx], blend_t)
            new_kfs.append((new_frame, new_vals))

        for idx, fc in idx_fcs.items():
            while len(fc.keyframe_points) > 0:
                fc.keyframe_points.remove(fc.keyframe_points[0])
            for new_frame, new_vals in new_kfs:
                if idx in new_vals:
                    fc.keyframe_points.insert(new_frame, new_vals[idx], options={'FAST'})

    for fc in fc_list:
        fc.update()
    return orig_count, new_count


def apply_resample(action, source_fps, target_fps):
    """Change 5: Framerate resample (30fps -> 24fps)."""
    fcurves = get_fcurves_collection(action)
    if not fcurves:
        return None, None

    fc_list = list(fcurves)
    if not fc_list:
        return None, None

    any_fc = fc_list[0]
    if len(any_fc.keyframe_points) == 0:
        return None, None

    frame_start = int(any_fc.keyframe_points[0].co[0])
    frame_end = int(any_fc.keyframe_points[-1].co[0])
    orig_count = frame_end - frame_start + 1

    duration_sec = orig_count / source_fps
    new_count = max(2, round(duration_sec * target_fps))

    groups = {}
    for fc in fc_list:
        groups.setdefault(fc.data_path, {})[fc.array_index] = fc

    for dp, idx_fcs in groups.items():
        any_fc2 = list(idx_fcs.values())[0]
        num_orig = len(any_fc2.keyframe_points)
        if num_orig < 2:
            continue

        orig_data = []
        for ki in range(num_orig):
            frame = any_fc2.keyframe_points[ki].co[0]
            values = {}
            for idx, fc in idx_fcs.items():
                if ki < len(fc.keyframe_points):
                    values[idx] = fc.keyframe_points[ki].co[1]
            orig_data.append((frame, values))

        is_quat = 'rotation_quaternion' in dp

        new_kfs = []
        for ni in range(new_count):
            new_frame = frame_start + ni
            t = ni / max(new_count - 1, 1)
            source_frame = frame_start + t * (frame_end - frame_start)

            src_ki = 0
            for j in range(len(orig_data) - 1):
                if orig_data[j][0] <= source_frame <= orig_data[j + 1][0]:
                    src_ki = j
                    break
            else:
                src_ki = len(orig_data) - 2
            src_ki = max(0, min(src_ki, len(orig_data) - 2))

            f0 = orig_data[src_ki][0]
            f1 = orig_data[src_ki + 1][0]
            blend_t = (source_frame - f0) / max(f1 - f0, 0.001)
            blend_t = max(0.0, min(1.0, blend_t))

            vals0 = orig_data[src_ki][1]
            vals1 = orig_data[src_ki + 1][1]

            new_vals = {}
            if is_quat and all(i in vals0 and i in vals1 for i in range(4)):
                q0 = Quaternion((vals0[0], vals0[1], vals0[2], vals0[3]))
                q1_q = Quaternion((vals1[0], vals1[1], vals1[2], vals1[3]))
                qi = slerp_quat(q0, q1_q, blend_t)
                new_vals = {0: qi.w, 1: qi.x, 2: qi.y, 3: qi.z}
            else:
                for idx in vals0:
                    if idx in vals1:
                        new_vals[idx] = lerp(vals0[idx], vals1[idx], blend_t)
            new_kfs.append((new_frame, new_vals))

        for idx, fc in idx_fcs.items():
            while len(fc.keyframe_points) > 0:
                fc.keyframe_points.remove(fc.keyframe_points[0])
            for new_frame, new_vals in new_kfs:
                if idx in new_vals:
                    fc.keyframe_points.insert(new_frame, new_vals[idx], options={'FAST'})

    for fc in fc_list:
        fc.update()
    return orig_count, new_count


def apply_keyframe_decimation(action):
    """Change 5b: Remove redundant keyframes where SLERP interpolation error < threshold.

    For each bone's quaternion channels, tests removing each keyframe and checks if
    SLERP from neighbors stays within DECIMATION_THRESHOLD_DEG. Typically removes
    30-50% of keyframes on slow-moving bones, few on fast-moving ones.
    """
    fcurves = get_fcurves_collection(action)
    if not fcurves:
        return 0, 0

    threshold_rad = math.radians(DECIMATION_THRESHOLD_DEG)

    # Group rotation fcurves by bone
    bone_rot = {}
    for fc in fcurves:
        dp = fc.data_path
        if 'pose.bones["' in dp and 'rotation_quaternion' in dp:
            try:
                bone_name = dp.split('"')[1]
            except IndexError:
                continue
            bone_rot.setdefault(bone_name, {})[fc.array_index] = fc

    total_before = 0
    total_after = 0

    for bone_name, rot_fcs in bone_rot.items():
        if len(rot_fcs) < 4:
            continue

        num_kps = len(rot_fcs[0].keyframe_points)
        if num_kps < 3:
            total_before += num_kps
            total_after += num_kps
            continue

        # Extract all keyframe data: (frame, w, x, y, z)
        keyframes = []
        for ki in range(num_kps):
            frame = rot_fcs[0].keyframe_points[ki].co[0]
            w = rot_fcs[0].keyframe_points[ki].co[1]
            x = rot_fcs[1].keyframe_points[ki].co[1]
            y = rot_fcs[2].keyframe_points[ki].co[1]
            z = rot_fcs[3].keyframe_points[ki].co[1]
            keyframes.append((frame, w, x, y, z))

        total_before += len(keyframes)

        # Mark which keyframes to keep (always keep first and last)
        keep = [True] * len(keyframes)

        # Iterate middle keyframes and test removal
        for ki in range(1, len(keyframes) - 1):
            if not keep[ki]:
                continue

            # Find previous kept keyframe
            prev_ki = ki - 1
            while prev_ki > 0 and not keep[prev_ki]:
                prev_ki -= 1

            # Find next kept keyframe
            next_ki = ki + 1
            while next_ki < len(keyframes) - 1 and not keep[next_ki]:
                next_ki += 1

            prev_f, pw, px, py, pz = keyframes[prev_ki]
            next_f, nw, nx, ny, nz = keyframes[next_ki]
            curr_f, cw, cx, cy, cz = keyframes[ki]

            # Compute SLERP interpolation at this frame's position
            f_range = next_f - prev_f
            if f_range < 0.001:
                continue
            t = (curr_f - prev_f) / f_range

            q_prev = Quaternion((pw, px, py, pz))
            q_next = Quaternion((nw, nx, ny, nz))
            q_interp = slerp_quat(q_prev, q_next, t)
            q_actual = Quaternion((cw, cx, cy, cz))

            # Compute angular difference
            q_diff = q_actual.rotation_difference(q_interp)
            angle = q_diff.angle
            if angle > math.pi:
                angle = 2 * math.pi - angle

            if angle < threshold_rad:
                keep[ki] = False

        # Rebuild keyframes with only kept ones
        kept_kfs = [keyframes[ki] for ki in range(len(keyframes)) if keep[ki]]
        total_after += len(kept_kfs)

        # Rewrite fcurves
        for idx in range(4):
            fc = rot_fcs[idx]
            while len(fc.keyframe_points) > 0:
                fc.keyframe_points.remove(fc.keyframe_points[0])
            for kf in kept_kfs:
                fc.keyframe_points.insert(kf[0], kf[1 + idx], options={'FAST'})
            fc.update()

    for fc in fcurves:
        fc.update()
    return total_before, total_after


def prune_bones(arm_obj, meshes, action):
    """Change 6: Remove cosmetic bones from armature, fcurves, and vertex groups."""
    fcurves = get_fcurves_collection(action)
    fc_removed = 0
    if fcurves:
        to_remove = []
        for fc in fcurves:
            dp = fc.data_path
            if 'pose.bones["' in dp:
                try:
                    bone_name = dp.split('"')[1]
                except IndexError:
                    continue
                if bone_name in BONES_TO_PRUNE:
                    to_remove.append(fc)
        for fc in to_remove:
            fcurves.remove(fc)
            fc_removed += 1

    vg_removed = 0
    for mesh_obj in meshes:
        to_remove = []
        for vg in mesh_obj.vertex_groups:
            if vg.name in BONES_TO_PRUNE:
                to_remove.append(vg)
        for vg in to_remove:
            mesh_obj.vertex_groups.remove(vg)
            vg_removed += 1

    bpy.context.view_layer.objects.active = arm_obj
    bpy.ops.object.mode_set(mode='EDIT')
    bone_removed = 0
    for ebone in list(arm_obj.data.edit_bones):
        if ebone.name in BONES_TO_PRUNE:
            parent = ebone.parent
            for child in ebone.children:
                child.parent = parent
            arm_obj.data.edit_bones.remove(ebone)
            bone_removed += 1
    bpy.ops.object.mode_set(mode='OBJECT')

    return bone_removed, fc_removed, vg_removed


def perturb_rest_pose(arm_obj):
    """Change 7 + 7b: Rest pose rotation offsets AND bone length proportion changes."""
    bpy.context.view_layer.objects.active = arm_obj
    bpy.ops.object.mode_set(mode='EDIT')

    modified = 0
    length_modified = 0
    for ebone in arm_obj.data.edit_bones:
        # Change 7: Rotation perturbation
        if ebone.name in REST_POSE_OFFSETS_DEG:
            offset_deg = REST_POSE_OFFSETS_DEG[ebone.name]
            offset_rad = (math.radians(offset_deg[0]),
                          math.radians(offset_deg[1]),
                          math.radians(offset_deg[2]))
            offset_mat = Euler(offset_rad, 'XYZ').to_matrix().to_4x4()

            bone_mat = ebone.matrix.copy()
            new_mat = bone_mat @ offset_mat

            head = ebone.head.copy()
            length = ebone.length
            new_y = new_mat.col[1].to_3d().normalized()
            ebone.tail = head + new_y * length
            ebone.align_roll(new_mat.col[2].to_3d())
            modified += 1

        # Change 7b: Bone length scaling
        if ebone.name in BONE_LENGTH_SCALE:
            scale = BONE_LENGTH_SCALE[ebone.name]
            head = ebone.head.copy()
            direction = (ebone.tail - ebone.head)
            new_length = direction.length * scale
            if direction.length > 0.0001:
                direction.normalize()
                ebone.tail = head + direction * new_length
                length_modified += 1

    bpy.ops.object.mode_set(mode='OBJECT')
    return modified, length_modified


def apply_bone_rename(arm_obj, meshes, action):
    """Change 8: Rename bones from FLVER to UE5 Mannequin convention."""
    fc_count = 0
    fcurves = get_fcurves_collection(action)
    if fcurves:
        for fc in fcurves:
            dp = fc.data_path
            if 'pose.bones["' in dp:
                try:
                    old_name = dp.split('"')[1]
                except IndexError:
                    continue
                if old_name in BONE_RENAME:
                    fc.data_path = dp.replace(f'"{old_name}"', f'"{BONE_RENAME[old_name]}"')
                    fc_count += 1

    bpy.context.view_layer.objects.active = arm_obj
    bpy.ops.object.mode_set(mode='EDIT')
    bone_count = 0
    for ebone in arm_obj.data.edit_bones:
        if ebone.name in BONE_RENAME:
            ebone.name = BONE_RENAME[ebone.name]
            bone_count += 1
    bpy.ops.object.mode_set(mode='OBJECT')

    vg_count = 0
    for m in meshes:
        for vg in m.vertex_groups:
            if vg.name in BONE_RENAME:
                vg.name = BONE_RENAME[vg.name]
                vg_count += 1

    arm_obj.name = "Sentinel_Skeleton"
    arm_obj.data.name = "Sentinel_Skeleton"

    action.name = action.name.replace("FLVER_armature|", "")

    return bone_count, fc_count, vg_count


def phase3_forensic_transforms(output_dir):
    """Apply all 13 forensic transforms to retargeted FBXes, export final FBXes."""
    print("\n" + "=" * 70)
    print(" PHASE 3: FORENSIC TRANSFORMS (13 total)")
    print("=" * 70)

    phase2_dir = os.path.join(output_dir, "phase2", "retargeted")
    phase3_dir = os.path.join(output_dir, "phase3", "final")
    os.makedirs(phase3_dir, exist_ok=True)

    anim_list = list(ANIM_MAP.values())

    # C3: Build source FBX swap map (output_name -> which phase2 FBX to load)
    source_fbx_map = {}
    for anim_name in anim_list:
        source_fbx_map[anim_name] = anim_name  # Default: load own FBX
    for name_a, name_b in OUTPUT_SWAP_PAIRS.items():
        if name_a in source_fbx_map and name_b in source_fbx_map:
            source_fbx_map[name_a] = name_b
            source_fbx_map[name_b] = name_a
            print(f"  C3 Swap: {name_a} <-> {name_b}")

    # Pre-compute speed factors
    speed_factors = {}
    print("\n  Per-animation speed factors:")
    for anim_name in anim_list:
        sf = compute_speed_factor(anim_name)
        speed_factors[anim_name] = sf
        print(f"    {anim_name}: {sf:.3f}x ({(sf-1)*100:+.1f}%)")

    results = []
    failures = []

    for i, anim_name in enumerate(anim_list):
        # C3: Load swapped source FBX if applicable
        source_name = source_fbx_map.get(anim_name, anim_name)
        fbx_path = os.path.join(phase2_dir, f"{source_name}.fbx")
        swap_note = f" (from {source_name})" if source_name != anim_name else ""
        print(f"\n  [{i+1}/{len(anim_list)}] {anim_name}{swap_note}")

        if not os.path.exists(fbx_path):
            print(f"    SKIP: not found")
            failures.append((anim_name, "not found"))
            continue

        # Fresh scene
        bpy.ops.wm.read_homefile(use_empty=True)
        bpy.ops.import_scene.fbx(filepath=fbx_path, **{**FBX_IMPORT, 'ignore_leaf_bones': False})

        arm = None
        meshes = []
        for obj in bpy.data.objects:
            if obj.type == 'ARMATURE':
                arm = obj
            elif obj.type == 'MESH':
                meshes.append(obj)

        if not arm:
            failures.append((anim_name, "no armature"))
            continue

        action = None
        if arm.animation_data and arm.animation_data.action:
            action = arm.animation_data.action
        if not action:
            failures.append((anim_name, "no action"))
            continue

        try:
            # Transform order: 1 -> 8b -> 2 -> 2b -> 3 -> 4 -> 5 -> 5b -> 6 -> 7+7b -> 3b(swap) -> 8
            offset_count = apply_static_offset(action)                          # 1
            phase_shift_count = apply_phase_shift(action)                       # 8b (NEW)
            anim_seed = hash(anim_name) & 0xFFFFFFFF
            noise_count = apply_noise(action, anim_seed)                        # 2
            lowpass_count = apply_lowpass_filter(action, SOURCE_FPS)             # 2b (NEW)
            warp_count = apply_timewarp(action, TIMEWARP_STRENGTH)              # 3
            speed_factor = speed_factors[anim_name]
            speed_orig, speed_new = apply_speed_variation(action, speed_factor)  # 4
            orig_frames, new_frames = apply_resample(action, SOURCE_FPS, TARGET_FPS)  # 5
            kf_before, kf_after = apply_keyframe_decimation(action)             # 5b (NEW)
            bones_pruned, fcs_pruned, vgs_pruned = prune_bones(arm, meshes, action)  # 6
            rest_modified, length_modified = perturb_rest_pose(arm)              # 7+7b (EXTENDED)
            bones_rn, fcs_rn, vgs_rn = apply_bone_rename(arm, meshes, action)   # 8

            remaining_bones = len(arm.data.bones)

            print(f"    1.Offset:{offset_count} 8b.PhaseShift:{phase_shift_count} "
                  f"2.Noise:{noise_count} 2b.LowPass:{lowpass_count} 3.Warp:{warp_count} "
                  f"4.Speed:{speed_factor:.2f}x({speed_orig}->{speed_new}) "
                  f"5.Resample:{orig_frames}->{new_frames} "
                  f"5b.Decimate:{kf_before}->{kf_after} "
                  f"6.Prune:{bones_pruned}(->{remaining_bones}) "
                  f"7.RestPose:{rest_modified}+{length_modified}lengths 8.Rename:{bones_rn}")

            # Set frame range for export
            frame_start = int(action.curve_frame_range[0])
            frame_end = int(action.curve_frame_range[1])
            bpy.context.scene.frame_start = frame_start
            bpy.context.scene.frame_end = frame_end

            bpy.ops.object.select_all(action='DESELECT')
            arm.select_set(True)
            for m in meshes:
                m.select_set(True)
            bpy.context.view_layer.objects.active = arm

            out_path = os.path.join(phase3_dir, f"{anim_name}.fbx")
            bpy.ops.export_scene.fbx(filepath=out_path, bake_anim_step=1.0, **FBX_EXPORT)
            results.append(anim_name)
            print(f"    -> {out_path}")

        except Exception as e:
            print(f"    ERROR: {e}")
            import traceback
            traceback.print_exc()
            failures.append((anim_name, str(e)))
            try:
                bpy.ops.object.mode_set(mode='OBJECT')
            except:
                pass

        gc.collect()

    # Export mesh-only FBX with forensic bone changes
    print(f"\n  [MESH] Exporting forensic mesh FBX...")
    mesh_src = os.path.join(phase2_dir, "SKM_Mesh.fbx")
    if os.path.exists(mesh_src):
        bpy.ops.wm.read_homefile(use_empty=True)
        bpy.ops.import_scene.fbx(filepath=mesh_src, **{**FBX_IMPORT, 'use_anim': False, 'ignore_leaf_bones': False})

        arm = None
        meshes = []
        for obj in bpy.data.objects:
            if obj.type == 'ARMATURE':
                arm = obj
            elif obj.type == 'MESH':
                meshes.append(obj)

        if arm:
            # Prune bones (no action for mesh-only)
            for mesh_obj in meshes:
                to_remove = [vg for vg in mesh_obj.vertex_groups if vg.name in BONES_TO_PRUNE]
                for vg in to_remove:
                    mesh_obj.vertex_groups.remove(vg)

            bpy.context.view_layer.objects.active = arm
            bpy.ops.object.mode_set(mode='EDIT')
            for ebone in list(arm.data.edit_bones):
                if ebone.name in BONES_TO_PRUNE:
                    parent = ebone.parent
                    for child in ebone.children:
                        child.parent = parent
                    arm.data.edit_bones.remove(ebone)
            bpy.ops.object.mode_set(mode='OBJECT')

            perturb_rest_pose(arm)

            bpy.context.view_layer.objects.active = arm
            bpy.ops.object.mode_set(mode='EDIT')
            for ebone in arm.data.edit_bones:
                if ebone.name in BONE_RENAME:
                    ebone.name = BONE_RENAME[ebone.name]
            bpy.ops.object.mode_set(mode='OBJECT')

            arm.name = "Sentinel_Skeleton"
            arm.data.name = "Sentinel_Skeleton"

            for m in meshes:
                m.name = "SKM_Sentinel"
                for vg in m.vertex_groups:
                    if vg.name in BONE_RENAME:
                        vg.name = BONE_RENAME[vg.name]

            remaining = len(arm.data.bones)
            print(f"    Bones: {remaining} (after prune + rename)")

            bpy.ops.object.select_all(action='DESELECT')
            arm.select_set(True)
            for m in meshes:
                m.select_set(True)
            bpy.context.view_layer.objects.active = arm

            mesh_out = os.path.join(phase3_dir, "SKM_Sentinel.fbx")
            export_opts = dict(FBX_EXPORT)
            export_opts['bake_anim'] = False
            bpy.ops.export_scene.fbx(filepath=mesh_out, **export_opts)
            print(f"    -> {mesh_out}")
            results.append("SKM_Sentinel")

    print(f"\n  PHASE 3 COMPLETE: {len(results)}/{len(anim_list) + 1} succeeded")
    if failures:
        for name, reason in failures:
            print(f"    FAIL {name}: {reason}")

    print(f"\n  Forensic changes applied (13 total):")
    print(f"    1.  Static offset: spine +5deg yaw, shoulders +/-3deg roll")
    print(f"    8b. Per-bone phase shift: 0-{PHASE_SHIFT_MAX_FRAMES} frames per bone")
    print(f"    2.  Noise: {NOISE_AMPLITUDE_DEG}deg amplitude, bone-weighted")
    print(f"    2b. Low-pass Butterworth: {LOWPASS_CUTOFF_HZ}Hz cutoff, order {LOWPASS_ORDER}")
    print(f"    3.  Time warp: {TIMEWARP_STRENGTH*100:.0f}% non-linear")
    print(f"    3b. Direction swap: {len(OUTPUT_SWAP_PAIRS)} pairs swapped")
    print(f"    4.  Speed variation: {SPEED_VAR_MIN:.0%}-{SPEED_VAR_MAX:.0%} per-anim")
    print(f"    5.  Resample: {SOURCE_FPS}fps -> {TARGET_FPS}fps")
    print(f"    5b. Keyframe decimation: {DECIMATION_THRESHOLD_DEG}deg threshold")
    print(f"    6.  Bone prune: {len(BONES_TO_PRUNE)} cosmetic bones removed (91->68)")
    print(f"    7.  Rest pose: {len(REST_POSE_OFFSETS_DEG)} bones perturbed")
    print(f"    7b. Bone lengths: {len(BONE_LENGTH_SCALE)} bones scaled")
    print(f"    8.  Bone rename: {len(BONE_RENAME)} bones -> UE5 Mannequin")

    return len(failures) == 0


# ============================================================
# PHASE 4: Comparison Renders
# ============================================================

def setup_render_scene():
    """Configure EEVEE render settings and 3-point lighting."""
    scene = bpy.context.scene
    # Blender 4.2+ renamed EEVEE; try both names
    try:
        scene.render.engine = 'BLENDER_EEVEE_NEXT'
    except TypeError:
        scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x = 960
    scene.render.resolution_y = 540
    scene.render.film_transparent = True

    # Remove existing lights and cameras
    for obj in list(bpy.data.objects):
        if obj.type in ('LIGHT', 'CAMERA'):
            bpy.data.objects.remove(obj, do_unlink=True)

    # Add camera at 3/4 front view
    cam_data = bpy.data.cameras.new("RenderCam")
    cam_obj = bpy.data.objects.new("RenderCam", cam_data)
    bpy.context.collection.objects.link(cam_obj)
    cam_obj.location = (2.5, -3.0, 1.2)
    cam_obj.rotation_euler = (math.radians(75), 0, math.radians(40))
    cam_data.lens = 50
    scene.camera = cam_obj

    # Key light
    key_data = bpy.data.lights.new("KeyLight", 'SUN')
    key_data.energy = 3.0
    key_obj = bpy.data.objects.new("KeyLight", key_data)
    bpy.context.collection.objects.link(key_obj)
    key_obj.rotation_euler = (math.radians(45), math.radians(15), math.radians(30))

    # Fill light
    fill_data = bpy.data.lights.new("FillLight", 'SUN')
    fill_data.energy = 1.0
    fill_obj = bpy.data.objects.new("FillLight", fill_data)
    bpy.context.collection.objects.link(fill_obj)
    fill_obj.rotation_euler = (math.radians(60), math.radians(-30), math.radians(-45))

    # Rim light
    rim_data = bpy.data.lights.new("RimLight", 'SUN')
    rim_data.energy = 2.0
    rim_obj = bpy.data.objects.new("RimLight", rim_data)
    bpy.context.collection.objects.link(rim_obj)
    rim_obj.rotation_euler = (math.radians(30), math.radians(45), math.radians(150))


def render_animation_frames(arm, anim_action, output_prefix, render_dir):
    """Render keyframes at 0%, 25%, 50%, 75%, 100% of animation."""
    if not anim_action:
        return []

    if arm.animation_data is None:
        arm.animation_data_create()
    arm.animation_data.action = anim_action

    # Blender 5.0 slot handling
    if hasattr(anim_action, 'slots'):
        for slot in anim_action.slots:
            try:
                arm.animation_data.action_slot = slot
                break
            except Exception:
                continue

    frame_start = int(anim_action.frame_range[0])
    frame_end = int(anim_action.frame_range[1])
    duration = frame_end - frame_start

    rendered = []
    for pct in [0, 25, 50, 75, 100]:
        frame = frame_start + int(duration * pct / 100)
        bpy.context.scene.frame_set(frame)

        filepath = os.path.join(render_dir, f"{output_prefix}_frame{pct:03d}.png")
        bpy.context.scene.render.filepath = filepath
        bpy.ops.render.render(write_still=True)
        rendered.append(filepath)

    return rendered


def phase4_comparison_renders(reference_blend, output_dir):
    """Render side-by-side comparison PNGs for key animations."""
    print("\n" + "=" * 70)
    print(" PHASE 4: COMPARISON RENDERS")
    print("=" * 70)

    phase2_dir = os.path.join(output_dir, "phase2", "retargeted")
    render_dir = os.path.join(output_dir, "phase4", "renders")
    os.makedirs(render_dir, exist_ok=True)

    total_rendered = 0

    for anim_name in RENDER_ANIMS:
        print(f"\n  Rendering: {anim_name}")

        # --- Render FLVER reference ---
        print(f"    [REF] Loading reference blend...")
        bpy.ops.wm.open_mainfile(filepath=reference_blend)
        setup_render_scene()

        # Find FLVER armature and reference mesh
        ref_arm = None
        ref_mesh = None
        for obj in bpy.data.objects:
            if obj.type == 'ARMATURE':
                ref_arm = obj
            elif obj.type == 'MESH' and obj.name == 'Sentinel_mesh':
                ref_mesh = obj

        if not ref_arm:
            print(f"    [REF] SKIP: no armature")
            continue

        # Show only the reference mesh
        for obj in bpy.data.objects:
            if obj.type == 'MESH':
                if obj == ref_mesh:
                    obj.hide_set(False)
                    obj.hide_viewport = False
                    obj.hide_render = False
                else:
                    obj.hide_render = True

        # Find and import the source anim FBX for reference
        # Use the retargeted FBX (Phase 2) since it uses FLVER bone names
        anim_fbx = os.path.join(phase2_dir, f"{anim_name}.fbx")
        if not os.path.exists(anim_fbx):
            print(f"    [REF] SKIP: anim FBX not found")
            continue

        _, _, ref_action = import_fbx(anim_fbx)

        if ref_action:
            ref_renders = render_animation_frames(
                ref_arm, ref_action, f"{anim_name}_ref", render_dir)
            print(f"    [REF] Rendered {len(ref_renders)} frames")
            total_rendered += len(ref_renders)

            # Clean up imported objects
            clean_orphans()

        # --- Render new mesh ---
        print(f"    [NEW] Loading rigged blend...")
        phase1_blend = os.path.join(output_dir, "phase1", "rigged_mesh.blend")
        if not os.path.exists(phase1_blend):
            print(f"    [NEW] SKIP: rigged blend not found")
            continue

        bpy.ops.wm.open_mainfile(filepath=phase1_blend)
        setup_render_scene()

        new_arm = None
        new_mesh = None
        for obj in bpy.data.objects:
            if obj.type == 'ARMATURE':
                new_arm = obj
            elif obj.type == 'MESH' and obj.name == 'Pipeline_mesh':
                new_mesh = obj

        if not new_arm:
            print(f"    [NEW] SKIP: no armature")
            continue

        # Show only the pipeline mesh
        for obj in bpy.data.objects:
            if obj.type == 'MESH':
                if obj == new_mesh:
                    obj.hide_set(False)
                    obj.hide_viewport = False
                    obj.hide_render = False
                else:
                    obj.hide_render = True

        _, _, new_action = import_fbx(anim_fbx)

        if new_action:
            new_renders = render_animation_frames(
                new_arm, new_action, f"{anim_name}_new", render_dir)
            print(f"    [NEW] Rendered {len(new_renders)} frames")
            total_rendered += len(new_renders)

    print(f"\n  PHASE 4 COMPLETE: {total_rendered} frames rendered")
    print(f"  Output: {render_dir}")
    return True


# ============================================================
# PHASE 5: Validation Report
# ============================================================

def phase5_validation_report(output_dir):
    """Count outputs, check file sizes, print summary."""
    print("\n" + "=" * 70)
    print(" PHASE 5: VALIDATION REPORT")
    print("=" * 70)

    report_lines = []

    def rpt(line):
        print(f"  {line}")
        report_lines.append(line)

    rpt(f"Pipeline Output: {output_dir}")
    rpt(f"Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}")
    rpt("")

    # Phase 2: Retargeted FBXes
    phase2_dir = os.path.join(output_dir, "phase2", "retargeted")
    rpt("=== Phase 2: Retargeted Animations ===")
    p2_anims = []
    p2_mesh = None
    if os.path.exists(phase2_dir):
        for f in sorted(os.listdir(phase2_dir)):
            if not f.endswith('.fbx'):
                continue
            fpath = os.path.join(phase2_dir, f)
            size_kb = os.path.getsize(fpath) / 1024
            if f == "SKM_Mesh.fbx":
                p2_mesh = size_kb
                rpt(f"  MESH  {f:40s} {size_kb:8.1f} KB")
            else:
                p2_anims.append((f, size_kb))
                rpt(f"  ANIM  {f:40s} {size_kb:8.1f} KB")
    rpt(f"  Total: {len(p2_anims)} anims" + (f" + 1 mesh" if p2_mesh else ""))
    rpt("")

    # Phase 3: Final FBXes
    phase3_dir = os.path.join(output_dir, "phase3", "final")
    rpt("=== Phase 3: Forensic Final FBXes ===")
    p3_anims = []
    p3_mesh = None
    if os.path.exists(phase3_dir):
        for f in sorted(os.listdir(phase3_dir)):
            if not f.endswith('.fbx'):
                continue
            fpath = os.path.join(phase3_dir, f)
            size_kb = os.path.getsize(fpath) / 1024
            if f == "SKM_Sentinel.fbx":
                p3_mesh = size_kb
                rpt(f"  MESH  {f:40s} {size_kb:8.1f} KB")
            else:
                p3_anims.append((f, size_kb))
                rpt(f"  ANIM  {f:40s} {size_kb:8.1f} KB")
    rpt(f"  Total: {len(p3_anims)} anims" + (f" + 1 mesh" if p3_mesh else ""))
    rpt("")

    # Phase 4: Renders
    render_dir = os.path.join(output_dir, "phase4", "renders")
    rpt("=== Phase 4: Comparison Renders ===")
    png_count = 0
    if os.path.exists(render_dir):
        pngs = [f for f in os.listdir(render_dir) if f.endswith('.png')]
        png_count = len(pngs)
        for f in sorted(pngs):
            fpath = os.path.join(render_dir, f)
            size_kb = os.path.getsize(fpath) / 1024
            rpt(f"  PNG   {f:40s} {size_kb:8.1f} KB")
    rpt(f"  Total: {png_count} PNGs")
    rpt("")

    # Summary table
    rpt("=== Summary ===")
    anim_list = list(ANIM_MAP.values())
    rpt(f"  {'Animation':<35s} {'Speed':>6s} {'Phase2':>8s} {'Phase3':>8s}")
    rpt(f"  {'-'*35} {'-'*6} {'-'*8} {'-'*8}")
    for anim_name in anim_list:
        sf = compute_speed_factor(anim_name)
        p2_ok = "OK" if any(f[0] == f"{anim_name}.fbx" for f in p2_anims) else "MISS"
        p3_ok = "OK" if any(f[0] == f"{anim_name}.fbx" for f in p3_anims) else "MISS"
        rpt(f"  {anim_name:<35s} {sf:5.2f}x  {p2_ok:>8s} {p3_ok:>8s}")
    rpt("")

    # Bone verification hint
    rpt("=== Verification Checklist ===")
    expected_anim_count = len(ANIM_MAP)
    p3_ok = len(p3_anims) == expected_anim_count
    rpt(f"  [{'X' if p3_ok else ' '}] {len(p3_anims)}/{expected_anim_count} animation FBXes in phase3/final/")
    rpt(f"  [{'X' if p3_mesh else ' '}] Mesh FBX (SKM_Sentinel.fbx) in phase3/final/")
    rpt(f"  [{'X' if png_count > 0 else ' '}] {png_count} comparison PNGs in phase4/renders/")
    rpt(f"  [ ] Open any phase3/final/*.fbx in Blender: 68 bones with UE5 names")
    rpt(f"  [ ] No FLVER bone names remain (Master, L_Thigh, etc.)")
    rpt("")

    # Write report
    report_path = os.path.join(output_dir, "pipeline_report.txt")
    with open(report_path, 'w') as f:
        f.write('\n'.join(report_lines))
    print(f"\n  Report saved: {report_path}")

    # Overall pass/fail
    all_ok = (len(p3_anims) == expected_anim_count and p3_mesh is not None)
    rpt(f"\n  PIPELINE {'PASSED' if all_ok else 'INCOMPLETE'}")
    return all_ok


# ============================================================
# Main Entry Point
# ============================================================

def parse_args():
    """Parse command-line arguments after Blender's '--' separator."""
    argv = sys.argv
    if '--' in argv:
        argv = argv[argv.index('--') + 1:]
    else:
        argv = []

    parser = argparse.ArgumentParser(description="Mesh Animation Pipeline")
    parser.add_argument('--zip', default=DEFAULT_ZIP,
                        help="Path to AI mesh zip file")
    parser.add_argument('--source-anims', default=DEFAULT_SOURCE_ANIMS,
                        help="Directory containing source animation FBXes")
    parser.add_argument('--reference-blend', default=DEFAULT_REFERENCE_BLEND,
                        help="Path to reference .blend with FLVER armature")
    parser.add_argument('--output-dir', default=DEFAULT_OUTPUT_DIR,
                        help="Output directory for all pipeline products")
    parser.add_argument('--phase', type=int, action='append',
                        help="Run specific phase(s): 1-5. Can be repeated.")
    parser.add_argument('--all', action='store_true',
                        help="Run all 5 phases")
    return parser.parse_args(argv)


def main():
    args = parse_args()

    # Determine which phases to run
    if args.all:
        phases = [1, 2, 3, 4, 5]
    elif args.phase:
        phases = sorted(set(args.phase))
    else:
        print("ERROR: Specify --all or --phase N")
        print("  Example: blender --background --python mesh_animation_pipeline.py -- --all")
        print("  Example: blender --background --python mesh_animation_pipeline.py -- --phase 1")
        sys.exit(1)

    print("\n" + "=" * 70)
    print(" MESH ANIMATION PIPELINE")
    print(f" Phases: {phases}")
    print(f" Zip: {os.path.basename(args.zip)}")
    print(f" Source anims: {args.source_anims}")
    print(f" Reference: {os.path.basename(args.reference_blend)}")
    print(f" Output: {args.output_dir}")
    print("=" * 70)

    os.makedirs(args.output_dir, exist_ok=True)
    start_time = time.time()

    success = True

    if 1 in phases:
        if not phase1_mesh_prep(args.zip, args.reference_blend, args.output_dir):
            print("\n  PHASE 1 FAILED - stopping")
            success = False
            if len(phases) > 1:
                sys.exit(1)

    if 2 in phases and success:
        if not phase2_arp_retarget(args.source_anims, args.output_dir):
            print("\n  PHASE 2 had failures")
            # Continue anyway - partial results are useful

    if 3 in phases and success:
        if not phase3_forensic_transforms(args.output_dir):
            print("\n  PHASE 3 had failures")

    if 4 in phases and success:
        phase4_comparison_renders(args.reference_blend, args.output_dir)

    if 5 in phases:
        phase5_validation_report(args.output_dir)

    elapsed = time.time() - start_time
    print(f"\n  Total time: {elapsed:.1f}s ({elapsed/60:.1f}min)")
    print("=" * 70)


if __name__ == "__main__":
    main()
