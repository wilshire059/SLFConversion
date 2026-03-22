"""Validate reskinned mesh against original FLVER mesh.

Opens the reskinned blend file (which contains BOTH the hidden original
FLVER mesh and the visible reskinned custom mesh). Computes per-bone
weight quality metrics, renders side-by-side comparison frames, and
outputs a diagnostic report identifying problem areas.

This is the iterative validation tool: run reskin -> validate -> fix -> repeat.

Usage:
    blender --background --python validate_reskin.py
    blender --background --python validate_reskin.py -- /path/to/reskinned.blend

Outputs:
    output/c3100_validation/
        comparison_frame_NNNN.png   - Side-by-side rendered frames
        weight_report.txt           - Per-bone weight quality report
        problem_bones.txt           - Ranked list of problematic bones
"""
import bpy
import bmesh
import os
import sys
import json
import math
from mathutils import Vector, Euler, Matrix
from collections import defaultdict

# ── Config ──────────────────────────────────────────────────────────
DEFAULT_BLEND = "C:/scripts/elden_ring_tools/output/c3100_mesh/reskinned/SKM_Manny_export_c3100.blend"
DEFAULT_OUTPUT_DIR = "C:/scripts/elden_ring_tools/output/c3100_validation"

custom_args = []
if "--" in sys.argv:
    custom_args = sys.argv[sys.argv.index("--") + 1:]

# Support both --flag and positional args
_has_flags = any(a.startswith("--") for a in custom_args)
if _has_flags:
    import argparse
    _vparser = argparse.ArgumentParser(description="Validate reskin quality")
    _vparser.add_argument("--blend", default=DEFAULT_BLEND, help="Reskinned blend file")
    _vparser.add_argument("--output", default=DEFAULT_OUTPUT_DIR, help="Output directory")
    _vargs = _vparser.parse_args(custom_args)
    BLEND_PATH = _vargs.blend
    OUTPUT_DIR = _vargs.output
else:
    BLEND_PATH = custom_args[0] if custom_args else DEFAULT_BLEND
    OUTPUT_DIR = custom_args[1] if len(custom_args) > 1 else DEFAULT_OUTPUT_DIR

os.makedirs(OUTPUT_DIR, exist_ok=True)


def log(msg):
    print(f"[VALIDATE] {msg}")


# ── Helper: compute weight statistics for a mesh ────────────────────
def compute_bone_weight_stats(mesh_obj, arm_obj):
    """For each bone, compute spatial statistics of influenced vertices.

    Returns dict: bone_name -> {
        vert_count, total_weight, centroid, spread, max_dist,
        min_bounds, max_bounds, avg_weight
    }
    """
    stats = {}
    armature_bones = set(bone.name for bone in arm_obj.data.bones)

    # Build vertex group index -> name mapping
    vg_index_to_name = {}
    for vg in mesh_obj.vertex_groups:
        if vg.name in armature_bones:
            vg_index_to_name[vg.index] = vg.name

    # Collect per-bone vertex data
    bone_verts = defaultdict(list)  # bone_name -> [(vert_pos, weight)]
    mesh_data = mesh_obj.data

    for v in mesh_data.vertices:
        v_world = mesh_obj.matrix_world @ v.co
        for g in v.groups:
            if g.group in vg_index_to_name and g.weight > 0.01:
                bone_name = vg_index_to_name[g.group]
                bone_verts[bone_name].append((v_world.copy(), g.weight))

    # Compute statistics per bone
    for bone_name in armature_bones:
        verts = bone_verts.get(bone_name, [])
        if not verts:
            stats[bone_name] = {
                'vert_count': 0, 'total_weight': 0.0,
                'centroid': Vector((0, 0, 0)), 'spread': 0.0,
                'max_dist': 0.0, 'avg_weight': 0.0,
                'min_bounds': Vector((0, 0, 0)), 'max_bounds': Vector((0, 0, 0)),
            }
            continue

        positions = [v[0] for v in verts]
        weights = [v[1] for v in verts]
        total_w = sum(weights)
        avg_w = total_w / len(weights)

        # Weighted centroid
        centroid = Vector((0, 0, 0))
        for pos, w in verts:
            centroid += pos * w
        centroid /= total_w if total_w > 0 else 1.0

        # Spread (weighted standard deviation of distance from centroid)
        spread = 0.0
        max_dist = 0.0
        for pos, w in verts:
            d = (pos - centroid).length
            spread += w * d * d
            max_dist = max(max_dist, d)
        spread = math.sqrt(spread / total_w) if total_w > 0 else 0.0

        # Bounding box
        min_b = Vector((min(p.x for p in positions),
                         min(p.y for p in positions),
                         min(p.z for p in positions)))
        max_b = Vector((max(p.x for p in positions),
                         max(p.y for p in positions),
                         max(p.z for p in positions)))

        stats[bone_name] = {
            'vert_count': len(verts),
            'total_weight': total_w,
            'centroid': centroid,
            'spread': spread,
            'max_dist': max_dist,
            'avg_weight': avg_w,
            'min_bounds': min_b,
            'max_bounds': max_b,
        }

    return stats


def compute_deformation_quality(mesh_obj, arm_obj, sample_frames):
    """Compute per-frame deformation metrics.

    At each frame, evaluates the mesh with the armature modifier applied
    and checks for:
    - Self-intersection approximation (nearby vertices moving apart)
    - Volume preservation (bounding box stability)
    - Bone influence consistency

    Returns per-frame metrics dict.
    """
    frame_metrics = {}

    # Get rest pose bounding box
    arm_obj.data.pose_position = 'REST'
    bpy.context.scene.frame_set(sample_frames[0])
    bpy.context.view_layer.update()

    # Evaluate mesh at rest
    depsgraph = bpy.context.evaluated_depsgraph_get()
    eval_obj = mesh_obj.evaluated_get(depsgraph)
    eval_mesh = eval_obj.to_mesh()

    rest_positions = {}
    for v in eval_mesh.vertices:
        rest_positions[v.index] = (mesh_obj.matrix_world @ v.co).copy()

    rest_bbox_min = Vector((min(p.x for p in rest_positions.values()),
                             min(p.y for p in rest_positions.values()),
                             min(p.z for p in rest_positions.values())))
    rest_bbox_max = Vector((max(p.x for p in rest_positions.values()),
                             max(p.y for p in rest_positions.values()),
                             max(p.z for p in rest_positions.values())))
    rest_volume = ((rest_bbox_max.x - rest_bbox_min.x) *
                   (rest_bbox_max.y - rest_bbox_min.y) *
                   (rest_bbox_max.z - rest_bbox_min.z))

    eval_obj.to_mesh_clear()

    # Now evaluate at each animation frame
    arm_obj.data.pose_position = 'POSE'

    for frame in sample_frames:
        bpy.context.scene.frame_set(frame)
        bpy.context.view_layer.update()

        depsgraph = bpy.context.evaluated_depsgraph_get()
        eval_obj = mesh_obj.evaluated_get(depsgraph)
        eval_mesh = eval_obj.to_mesh()

        # Compute frame positions
        frame_positions = {}
        for v in eval_mesh.vertices:
            frame_positions[v.index] = (mesh_obj.matrix_world @ v.co).copy()

        # Bounding box
        bbox_min = Vector((min(p.x for p in frame_positions.values()),
                            min(p.y for p in frame_positions.values()),
                            min(p.z for p in frame_positions.values())))
        bbox_max = Vector((max(p.x for p in frame_positions.values()),
                            max(p.y for p in frame_positions.values()),
                            max(p.z for p in frame_positions.values())))
        frame_volume = ((bbox_max.x - bbox_min.x) *
                        (bbox_max.y - bbox_min.y) *
                        (bbox_max.z - bbox_min.z))

        # Displacement stats
        displacements = []
        for idx, pos in frame_positions.items():
            if idx in rest_positions:
                d = (pos - rest_positions[idx]).length
                displacements.append(d)

        avg_disp = sum(displacements) / len(displacements) if displacements else 0
        max_disp = max(displacements) if displacements else 0
        volume_ratio = frame_volume / rest_volume if rest_volume > 0 else 1.0

        frame_metrics[frame] = {
            'avg_displacement': avg_disp,
            'max_displacement': max_disp,
            'volume_ratio': volume_ratio,
            'bbox_min': [bbox_min.x, bbox_min.y, bbox_min.z],
            'bbox_max': [bbox_max.x, bbox_max.y, bbox_max.z],
        }

        eval_obj.to_mesh_clear()

    return frame_metrics


# ── Main ────────────────────────────────────────────────────────────
log("=" * 70)
log("Validate Reskinned Mesh vs Original FLVER")
log("=" * 70)

log(f"Opening: {BLEND_PATH}")
bpy.ops.wm.open_mainfile(filepath=BLEND_PATH)

# ── Identify objects ────────────────────────────────────────────────
arm_obj = None
flver_mesh = None
custom_mesh = None

for obj in bpy.data.objects:
    log(f"  {obj.name} ({obj.type}) hidden={obj.hide_get()}"
        f" verts={len(obj.data.vertices) if obj.type == 'MESH' else 'N/A'}")

    if obj.type == 'ARMATURE':
        arm_obj = obj
    elif obj.type == 'MESH':
        name_lower = obj.name.lower()
        if 'custom' in name_lower or 'manny' in name_lower or 'export' in name_lower:
            custom_mesh = obj
        elif 'c3100' in name_lower or 'flver' in name_lower:
            flver_mesh = obj
        else:
            # Heuristic: hidden mesh is likely the FLVER original
            if obj.hide_get():
                if flver_mesh is None:
                    flver_mesh = obj
            else:
                if custom_mesh is None:
                    custom_mesh = obj

if not arm_obj:
    log("ERROR: No armature found!")
    sys.exit(1)

# Handle case where only one mesh exists (source of truth blend)
if flver_mesh and not custom_mesh:
    log("\nOnly one mesh found - running single-mesh deformation analysis")
    custom_mesh = flver_mesh
    flver_mesh = None

if custom_mesh:
    log(f"\nCustom mesh: {custom_mesh.name} ({len(custom_mesh.data.vertices)} verts, "
        f"{len(custom_mesh.vertex_groups)} groups)")
if flver_mesh:
    log(f"FLVER mesh: {flver_mesh.name} ({len(flver_mesh.data.vertices)} verts, "
        f"{len(flver_mesh.vertex_groups)} groups)")
log(f"Armature: {arm_obj.name} ({len(arm_obj.data.bones)} bones)")

# ── Part 1: Weight Distribution Analysis ────────────────────────────
log("\n" + "=" * 70)
log("PART 1: Weight Distribution Analysis")
log("=" * 70)

custom_stats = compute_bone_weight_stats(custom_mesh, arm_obj) if custom_mesh else {}
flver_stats = compute_bone_weight_stats(flver_mesh, arm_obj) if flver_mesh else {}

# Build comparison report
report_lines = []
report_lines.append("=" * 90)
report_lines.append("WEIGHT DISTRIBUTION COMPARISON REPORT")
report_lines.append("=" * 90)
report_lines.append("")

# Compare per bone
problems = []
bone_comparisons = []

for bone in arm_obj.data.bones:
    bn = bone.name
    cs = custom_stats.get(bn, {})
    fs = flver_stats.get(bn, {})

    c_count = cs.get('vert_count', 0)
    f_count = fs.get('vert_count', 0)
    c_spread = cs.get('spread', 0.0)
    f_spread = fs.get('spread', 0.0)
    c_max = cs.get('max_dist', 0.0)
    f_max = fs.get('max_dist', 0.0)
    c_avg_w = cs.get('avg_weight', 0.0)
    f_avg_w = fs.get('avg_weight', 0.0)

    # Score how problematic this bone is
    score = 0.0
    reasons = []

    if flver_stats:
        # Compare spread: if custom is much wider than original, bad weights
        if f_spread > 0.01 and c_spread > f_spread * 2.0:
            score += (c_spread / f_spread - 1.0) * 10
            reasons.append(f"spread {c_spread:.3f} vs orig {f_spread:.3f} ({c_spread/f_spread:.1f}x)")

        # Compare vertex count (scaled by mesh size ratio)
        mesh_ratio = (len(custom_mesh.data.vertices) / len(flver_mesh.data.vertices)
                      if flver_mesh and len(flver_mesh.data.vertices) > 0 else 1.0)
        expected_count = f_count * mesh_ratio
        if expected_count > 5 and c_count > expected_count * 3:
            score += (c_count / expected_count - 1.0) * 5
            reasons.append(f"verts {c_count} vs expected ~{expected_count:.0f}")

        # Compare max distance from centroid
        if f_max > 0.01 and c_max > f_max * 2.0:
            score += (c_max / f_max - 1.0) * 5
            reasons.append(f"max_dist {c_max:.3f} vs orig {f_max:.3f}")

        # Centroid offset (should be similar relative position)
        c_centroid = cs.get('centroid', Vector())
        f_centroid = fs.get('centroid', Vector())
        if c_centroid.length > 0 and f_centroid.length > 0:
            offset = (c_centroid - f_centroid).length
            if offset > 0.5:  # More than 50cm offset
                score += offset * 2
                reasons.append(f"centroid offset {offset:.3f}")
    else:
        # No FLVER to compare - just flag extreme spreads
        if c_spread > 1.0:
            score += c_spread
            reasons.append(f"wide spread {c_spread:.3f}")
        if c_max > 2.0:
            score += c_max * 0.5
            reasons.append(f"far reach {c_max:.3f}")

    bone_comparisons.append({
        'name': bn,
        'score': score,
        'reasons': reasons,
        'custom_verts': c_count,
        'flver_verts': f_count,
        'custom_spread': c_spread,
        'flver_spread': f_spread,
        'custom_avg_weight': c_avg_w,
    })

    if score > 1.0:
        problems.append((score, bn, reasons))

# Sort by score (worst first)
problems.sort(reverse=True)

# Write detailed report
report_lines.append(f"{'Bone':<30} {'Cust Verts':>10} {'Orig Verts':>10} "
                    f"{'Cust Spread':>12} {'Orig Spread':>12} {'Score':>8}")
report_lines.append("-" * 90)

for bc in sorted(bone_comparisons, key=lambda x: -x['score']):
    marker = " ***" if bc['score'] > 1.0 else ""
    report_lines.append(
        f"{bc['name']:<30} {bc['custom_verts']:>10} {bc['flver_verts']:>10} "
        f"{bc['custom_spread']:>12.4f} {bc['flver_spread']:>12.4f} "
        f"{bc['score']:>8.2f}{marker}"
    )

report_lines.append("")
report_lines.append("=" * 90)
report_lines.append(f"PROBLEM BONES (score > 1.0): {len(problems)}")
report_lines.append("=" * 90)

for score, bn, reasons in problems[:30]:
    report_lines.append(f"\n  {bn} (score: {score:.2f})")
    for r in reasons:
        report_lines.append(f"    - {r}")

# Write report
report_path = os.path.join(OUTPUT_DIR, "weight_report.txt")
with open(report_path, 'w') as f:
    f.write("\n".join(report_lines))
log(f"Weight report: {report_path}")
log(f"Problem bones: {len(problems)}")
for score, bn, reasons in problems[:10]:
    log(f"  {bn}: score={score:.2f} - {'; '.join(reasons)}")

# ── Part 2: Deformation Quality Analysis ────────────────────────────
log("\n" + "=" * 70)
log("PART 2: Deformation Quality Analysis")
log("=" * 70)

# Get animation range
action = arm_obj.animation_data.action if arm_obj.animation_data else None
if action:
    frame_start = int(action.frame_range[0])
    frame_end = int(action.frame_range[1])
else:
    frame_start = 1
    frame_end = 96

total_frames = frame_end - frame_start + 1
sample_interval = max(1, total_frames // 8)
sample_frames = list(range(frame_start, frame_end + 1, sample_interval))
if frame_end not in sample_frames:
    sample_frames.append(frame_end)

log(f"Analyzing {len(sample_frames)} frames: {sample_frames}")

# Unhide both meshes for deformation analysis
if flver_mesh:
    flver_mesh.hide_set(False)
    flver_mesh.hide_render = False
if custom_mesh:
    custom_mesh.hide_set(False)
    custom_mesh.hide_render = False

# Ensure armature is in pose mode
arm_obj.data.pose_position = 'POSE'

# Compute deformation metrics for custom mesh
log("\nAnalyzing custom mesh deformation...")
custom_deform = compute_deformation_quality(custom_mesh, arm_obj, sample_frames)

flver_deform = {}
if flver_mesh:
    log("Analyzing FLVER mesh deformation...")
    flver_deform = compute_deformation_quality(flver_mesh, arm_obj, sample_frames)

# Write deformation report
deform_lines = []
deform_lines.append("=" * 70)
deform_lines.append("DEFORMATION QUALITY REPORT")
deform_lines.append("=" * 70)
deform_lines.append("")
deform_lines.append(f"{'Frame':>6} {'Avg Disp':>10} {'Max Disp':>10} "
                    f"{'Vol Ratio':>10} {'Orig Avg':>10} {'Orig Max':>10} {'Orig Vol':>10}")
deform_lines.append("-" * 70)

for frame in sample_frames:
    cm = custom_deform.get(frame, {})
    fm = flver_deform.get(frame, {})
    deform_lines.append(
        f"{frame:>6} {cm.get('avg_displacement', 0):>10.4f} "
        f"{cm.get('max_displacement', 0):>10.4f} "
        f"{cm.get('volume_ratio', 0):>10.4f} "
        f"{fm.get('avg_displacement', 0):>10.4f} "
        f"{fm.get('max_displacement', 0):>10.4f} "
        f"{fm.get('volume_ratio', 0):>10.4f}"
    )

deform_path = os.path.join(OUTPUT_DIR, "deformation_report.txt")
with open(deform_path, 'w') as f:
    f.write("\n".join(deform_lines))
log(f"Deformation report: {deform_path}")

# ── Part 3: Side-by-Side Comparison Renders ─────────────────────────
log("\n" + "=" * 70)
log("PART 3: Side-by-Side Comparison Renders")
log("=" * 70)

if flver_mesh and custom_mesh and flver_mesh != custom_mesh:
    # Position meshes side by side
    # Get bounding box of custom mesh to determine offset
    bbox_corners = [custom_mesh.matrix_world @ Vector(c) for c in custom_mesh.bound_box]
    bbox_size_x = max(c.x for c in bbox_corners) - min(c.x for c in bbox_corners)
    offset = bbox_size_x * 1.5

    # Move FLVER mesh to the left, custom to the right
    flver_mesh.location.x -= offset / 2
    custom_mesh.location.x += offset / 2

    # Apply different wireframe materials for visual distinction
    # Original: blue-ish | Custom: orange-ish
    mat_orig = bpy.data.materials.new(name="OriginalMaterial")
    mat_orig.use_nodes = True
    bsdf = mat_orig.node_tree.nodes.get("Principled BSDF")
    if bsdf:
        bsdf.inputs["Base Color"].default_value = (0.2, 0.4, 0.8, 1.0)

    mat_custom = bpy.data.materials.new(name="CustomMaterial")
    mat_custom.use_nodes = True
    bsdf2 = mat_custom.node_tree.nodes.get("Principled BSDF")
    if bsdf2:
        bsdf2.inputs["Base Color"].default_value = (0.8, 0.5, 0.2, 1.0)

    flver_mesh.data.materials.clear()
    flver_mesh.data.materials.append(mat_orig)
    custom_mesh.data.materials.clear()
    custom_mesh.data.materials.append(mat_custom)

    render_msg = "Rendering FLVER (blue/left) vs Custom (orange/right)"
else:
    render_msg = "Rendering single mesh"

log(render_msg)

# Set up camera
cam_data = bpy.data.cameras.new("ValidationCam")
cam_obj = bpy.data.objects.new("ValidationCam", cam_data)
bpy.context.scene.collection.objects.link(cam_obj)
bpy.context.scene.camera = cam_obj

# Position camera to see both meshes
all_meshes = [m for m in [flver_mesh, custom_mesh] if m]
all_corners = []
for m in all_meshes:
    all_corners.extend([m.matrix_world @ Vector(c) for c in m.bound_box])

if all_corners:
    center = sum(all_corners, Vector()) / len(all_corners)
    total_size = max((max(c.x for c in all_corners) - min(c.x for c in all_corners),
                      max(c.y for c in all_corners) - min(c.y for c in all_corners),
                      max(c.z for c in all_corners) - min(c.z for c in all_corners)))
    cam_distance = total_size * 1.8
else:
    center = Vector((0, 0, 1))
    cam_distance = 5.0

cam_obj.location = center + Vector((cam_distance * 0.5, -cam_distance * 0.8, cam_distance * 0.3))
direction = center - cam_obj.location
rot_quat = direction.to_track_quat('-Z', 'Y')
cam_obj.rotation_euler = rot_quat.to_euler()

# Lighting
sun = bpy.data.lights.new("ValSun", type='SUN')
sun.energy = 3.0
sun_obj = bpy.data.objects.new("ValSun", sun)
bpy.context.scene.collection.objects.link(sun_obj)
sun_obj.rotation_euler = Euler((math.radians(45), 0, math.radians(30)), 'XYZ')

fill = bpy.data.lights.new("ValFill", type='SUN')
fill.energy = 1.5
fill_obj = bpy.data.objects.new("ValFill", fill)
bpy.context.scene.collection.objects.link(fill_obj)
fill_obj.rotation_euler = Euler((math.radians(60), 0, math.radians(-60)), 'XYZ')

# Render settings
scene = bpy.context.scene
scene.render.engine = 'BLENDER_EEVEE'
scene.render.resolution_x = 1200
scene.render.resolution_y = 600
scene.render.film_transparent = False
scene.render.image_settings.file_format = 'PNG'

# Ensure pose mode for animation playback
arm_obj.data.pose_position = 'POSE'

# Render comparison frames
log(f"\nRendering {len(sample_frames)} comparison frames...")
for frame in sample_frames:
    scene.frame_set(frame)
    bpy.context.view_layer.update()

    filepath = os.path.join(OUTPUT_DIR, f"comparison_frame_{frame:04d}.png")
    scene.render.filepath = filepath
    bpy.ops.render.render(write_still=True)
    log(f"  Frame {frame} -> {filepath}")

# Rest pose comparison
arm_obj.data.pose_position = 'REST'
bpy.context.view_layer.update()
scene.frame_set(frame_start)
filepath = os.path.join(OUTPUT_DIR, "comparison_rest_pose.png")
scene.render.filepath = filepath
bpy.ops.render.render(write_still=True)
log(f"  Rest pose -> {filepath}")

# ── Summary ─────────────────────────────────────────────────────────
log("\n" + "=" * 70)
log("VALIDATION COMPLETE")
log("=" * 70)
log(f"Output directory: {OUTPUT_DIR}")
log(f"  weight_report.txt       - Per-bone weight analysis")
log(f"  deformation_report.txt  - Per-frame deformation metrics")
log(f"  comparison_frame_*.png  - Side-by-side renders ({len(sample_frames)} frames)")
log(f"  comparison_rest_pose.png - Rest pose comparison")
log(f"\nProblem bones: {len(problems)}")
if problems:
    log("Top 5 worst bones:")
    for score, bn, reasons in problems[:5]:
        log(f"  {bn}: {'; '.join(reasons)}")
log("=" * 70)
