"""Enhanced swing direction analysis v2 — extracts combat context from bone trajectories.
Signals: hit count, wind-up time, recovery time, range, speed class, multi-hit detection,
root motion (distance + direction), L_Hand tracking for blocks, dodge/guard classification."""
import sys, os, re, math, time
import numpy as np
from scipy.spatial.transform import Rotation

sys.path.insert(0, 'C:/scripts/elden_ring_tools/soulstruct-havok/src')
sys.path.insert(0, 'C:/scripts/elden_ring_tools/soulstruct/src')

import logging
logging.disable(logging.CRITICAL)

from soulstruct.containers import Binder
from soulstruct.havok.core import HKX
from soulstruct.havok.fromsoft.eldenring import AnimationHKX, SkeletonHKX

logging.disable(logging.NOTSET)
logging.getLogger('soulstruct').setLevel(logging.FATAL + 1)

UNPACKED = 'C:/scripts/elden_ring_tools/unpacked/chr'
PARTIAL_DIR = 'C:/scripts/SLFConversion/swing_partial'
PROGRESS_FILE = 'C:/scripts/SLFConversion/swing_progress.txt'

CHAR_NAMES = {
    "c0000": "Player", "c2010": "Godrick Summon", "c2030": "Godrick",
    "c2031": "Godrick Phase 2", "c2040": "Rennala", "c2050": "Rykard",
    "c2060": "Radahn", "c2100": "Black Knife Assassin",
    "c2110": "Crucible Knight Boss", "c2120": "Malenia",
    "c2130": "Mohg", "c2140": "Fire Giant", "c2180": "Godfrey",
    "c2190": "Morgott", "c2200": "Radagon/Elden Beast",
    "c2500": "Crucible Knight Alt", "c3000": "Exile Soldier",
    "c3010": "Banished Knight", "c3050": "Commander Niall",
    "c3100": "Crucible Knight", "c3150": "Redmane Knight",
    "c3160": "Carian Knight", "c3180": "Red Wolf", "c3200": "Battlemage",
    "c3210": "Fire Prelate", "c3250": "Crucible Tree Knight",
    "c3300": "Nox Swordstress", "c3400": "Grave Warden",
    "c3450": "Misbegotten", "c3500": "Skeleton", "c3550": "Sanguine Noble",
    "c3560": "Giant Crab", "c3610": "Oracle Envoy",
    "c3670": "Demi-Human", "c3700": "Raya Lucaria Scholar",
    "c3800": "Cleanrot Knight", "c3900": "Fire Monk",
    "c3910": "Blackflame Monk", "c3950": "Man-Serpent",
    "c3970": "Azula Beastman", "c4050": "Kaiden Sellsword",
    "c4080": "Lion Guardian", "c4090": "Grafted Scion",
    "c4100": "Imp", "c4110": "Ancestral Follower",
    "c4270": "Leyndell Knight", "c4290": "Bloodhound Knight",
    "c4300": "Godrick Soldier", "c4310": "Lordsworn Soldier",
    "c4320": "Vulgar Militia", "c4340": "Pumpkin Head",
    "c4350": "Lordsworn Knight", "c4370": "Haligtree Knight",
    "c4380": "Starcaller", "c4470": "Inquisitor Ghiza",
    "c4810": "Erdtree Avatar", "c4820": "Ulcerated Tree Spirit",
    "c5830": "Messmer Soldier", "c5840": "Black Knight",
}


def analyze_root_motion(root_positions, fps=30.0):
    """Analyze root bone movement during animation."""
    if not root_positions or len(root_positions) < 2:
        return {'root_distance': 0.0, 'root_direction': 'stationary'}

    positions = np.array(root_positions)
    start = positions[0]
    end = positions[-1]
    total_delta = end - start

    # XY displacement only (ignore vertical)
    xy_dist = float(np.sqrt(total_delta[0]**2 + total_delta[1]**2))
    z_dist = float(abs(total_delta[2]))

    if xy_dist < 0.05 and z_dist < 0.05:
        return {'root_distance': 0.0, 'root_direction': 'stationary'}

    # Max displacement from start at any point
    displacements = np.linalg.norm(positions - start, axis=1)
    max_displacement = float(np.max(displacements))

    # Direction classification
    if xy_dist < 0.05:
        direction = 'vertical'
    else:
        angle = math.atan2(total_delta[0], total_delta[1])  # X=lateral, Y=forward
        deg = math.degrees(angle)
        if -45 <= deg <= 45:
            direction = 'forward_lunge'
        elif 45 < deg <= 135:
            direction = 'sidestep_right'
        elif -135 <= deg < -45:
            direction = 'sidestep_left'
        else:
            direction = 'backstep'

    return {
        'root_distance': round(max_displacement, 3),
        'root_direction': direction,
    }


def analyze_l_hand(l_hand_positions, fps=30.0):
    """Analyze L_Hand for block/shield behavior."""
    if not l_hand_positions or len(l_hand_positions) < 3:
        return {}

    positions = np.array(l_hand_positions)

    # Check if L_Hand stays relatively stationary in front of body (guard pose)
    # vs moving dynamically (attack/cast)
    movement_range = float(np.max(np.linalg.norm(positions - positions[0], axis=1)))

    # Compute average Z position relative to start — raised hand = guard
    avg_z = float(np.mean(positions[:, 2]))
    start_z = float(positions[0, 2])
    z_raise = avg_z - start_z

    # Compute speeds
    deltas = np.diff(positions, axis=0)
    speeds = np.linalg.norm(deltas, axis=1) * fps
    avg_speed = float(np.mean(speeds))
    max_speed = float(np.max(speeds))

    result = {
        'l_hand_range': round(movement_range, 3),
        'l_hand_avg_speed': round(avg_speed, 3),
    }

    # Classify L_Hand behavior
    if movement_range < 0.15 and avg_speed < 0.3:
        result['l_hand_behavior'] = 'guard_pose'  # Hand stays still — likely blocking
    elif z_raise > 0.1 and movement_range < 0.3:
        result['l_hand_behavior'] = 'raised_guard'  # Hand raised — shield up
    elif max_speed > 1.5:
        result['l_hand_behavior'] = 'active_strike'  # Fast movement — L_Hand attacking
    elif movement_range > 0.5:
        result['l_hand_behavior'] = 'casting'  # Wide movement — spell gesture
    else:
        result['l_hand_behavior'] = 'passive'  # Normal idle hand

    return result


def analyze_attack(hand_positions, fps=30.0):
    """Full combat analysis of R_Hand trajectory."""
    n = len(hand_positions)
    if n < 3:
        return None

    positions = np.array(hand_positions)

    # Compute per-frame velocities
    deltas = np.diff(positions, axis=0)
    speeds = np.linalg.norm(deltas, axis=1) * fps  # units/sec

    if len(speeds) == 0 or np.max(speeds) < 0.01:
        return None

    # --- HIT DETECTION: find velocity peaks ---
    # A "hit" is a local maximum in speed above a threshold
    threshold = np.max(speeds) * 0.4  # 40% of peak
    peaks = []
    in_peak = False
    peak_start = 0
    for i in range(len(speeds)):
        if speeds[i] > threshold and not in_peak:
            in_peak = True
            peak_start = i
        elif speeds[i] < threshold * 0.5 and in_peak:
            # Peak ended — find the max within this peak
            peak_idx = peak_start + np.argmax(speeds[peak_start:i])
            peaks.append(peak_idx)
            in_peak = False
    if in_peak:
        peak_idx = peak_start + np.argmax(speeds[peak_start:])
        peaks.append(peak_idx)

    # Merge peaks that are too close (< 5 frames apart)
    merged_peaks = []
    for p in peaks:
        if merged_peaks and (p - merged_peaks[-1]) < 5:
            # Keep the higher velocity one
            if speeds[p] > speeds[merged_peaks[-1]]:
                merged_peaks[-1] = p
        else:
            merged_peaks.append(p)

    hit_count = len(merged_peaks)
    if hit_count == 0:
        return None

    # --- TIMING ---
    first_peak = merged_peaks[0]
    last_peak = merged_peaks[-1]
    total_frames = n - 1

    windup_frames = first_peak
    recovery_frames = total_frames - last_peak
    active_frames = last_peak - first_peak if hit_count > 1 else 1

    windup_time = windup_frames / fps
    recovery_time = recovery_frames / fps
    total_time = total_frames / fps

    # --- SPEED CLASS ---
    peak_speed = np.max(speeds)
    avg_peak_speed = np.mean([speeds[p] for p in merged_peaks])

    if windup_time < 0.3 and total_time < 2.0:
        speed_class = "very_fast"
    elif windup_time < 0.6 and total_time < 3.0:
        speed_class = "fast"
    elif windup_time < 1.2 and total_time < 4.5:
        speed_class = "medium"
    elif windup_time < 2.0:
        speed_class = "slow"
    else:
        speed_class = "very_slow"

    # --- RANGE ---
    # Max displacement from starting position
    start_pos = positions[0]
    displacements = np.linalg.norm(positions - start_pos, axis=1)
    max_range = float(np.max(displacements))

    # Range at peak velocity (effective attack range)
    peak_range = float(displacements[merged_peaks[0]])

    if max_range < 0.3:
        range_class = "close"
    elif max_range < 0.6:
        range_class = "medium"
    elif max_range < 1.0:
        range_class = "long"
    else:
        range_class = "very_long"

    # --- SWING DIRECTION (improved) ---
    # Use the primary peak's movement direction
    p = merged_peaks[0]
    window_start = max(0, p - 3)
    window_end = min(n - 1, p + 3)
    swing_delta = positions[window_end] - positions[window_start]
    swing_mag = np.linalg.norm(swing_delta)

    if swing_mag < 0.001:
        direction = "static"
    else:
        norm = swing_delta / swing_mag
        nx, ny, nz = norm
        abs_x, abs_y, abs_z = abs(nx), abs(ny), abs(nz)

        # Vertical component analysis
        vertical_ratio = abs_z / (abs_x + abs_y + abs_z + 0.001)
        horizontal_ratio = abs_x / (abs_x + abs_y + abs_z + 0.001)
        forward_ratio = abs_y / (abs_x + abs_y + abs_z + 0.001)

        if vertical_ratio > 0.5:
            if nz < 0:
                if horizontal_ratio > 0.25:
                    direction = "diag_r_down" if nx > 0 else "diag_l_down"
                else:
                    direction = "overhead"
            else:
                if horizontal_ratio > 0.25:
                    direction = "diag_r_up" if nx > 0 else "diag_l_up"
                else:
                    direction = "uppercut"
        elif forward_ratio > 0.45:
            direction = "thrust" if ny > 0 else "pull_back"
        elif horizontal_ratio > 0.35:
            direction = "right_to_left" if nx > 0 else "left_to_right"
        else:
            # Ambiguous — use dominant
            if abs_x >= abs_y and abs_x >= abs_z:
                direction = "right_to_left" if nx > 0 else "left_to_right"
            elif abs_z >= abs_x and abs_z >= abs_y:
                direction = "overhead" if nz < 0 else "uppercut"
            else:
                direction = "thrust" if ny > 0 else "pull_back"

    # --- SECOND HIT DIRECTION (for multi-hit) ---
    second_direction = None
    if hit_count >= 2:
        p2 = merged_peaks[1]
        w2s = max(0, p2 - 3)
        w2e = min(n - 1, p2 + 3)
        d2 = positions[w2e] - positions[w2s]
        m2 = np.linalg.norm(d2)
        if m2 > 0.001:
            n2 = d2 / m2
            abs_x2, abs_y2, abs_z2 = abs(n2[0]), abs(n2[1]), abs(n2[2])
            if abs_z2 > max(abs_x2, abs_y2):
                second_direction = "overhead" if n2[2] < 0 else "uppercut"
            elif abs_x2 > max(abs_y2, abs_z2):
                second_direction = "right_to_left" if n2[0] > 0 else "left_to_right"
            else:
                second_direction = "thrust" if n2[1] > 0 else "pull_back"

    # --- VERTICAL ARC ---
    z_range = float(np.max(positions[:, 2]) - np.min(positions[:, 2]))
    x_range = float(np.max(positions[:, 0]) - np.min(positions[:, 0]))
    y_range = float(np.max(positions[:, 1]) - np.min(positions[:, 1]))

    if z_range > x_range * 1.5 and z_range > y_range * 1.5:
        arc_type = "vertical"
    elif x_range > z_range * 1.5 and x_range > y_range * 1.5:
        arc_type = "horizontal"
    elif y_range > x_range * 1.5 and y_range > z_range * 1.5:
        arc_type = "forward"
    else:
        arc_type = "mixed"

    return {
        'direction': direction,
        'second_direction': second_direction,
        'hit_count': hit_count,
        'speed_class': speed_class,
        'range_class': range_class,
        'arc_type': arc_type,
        'peak_velocity': round(float(peak_speed), 3),
        'windup_time': round(windup_time, 3),
        'recovery_time': round(recovery_time, 3),
        'total_duration': round(total_time, 3),
        'max_range': round(max_range, 3),
        'frames': n,
    }


def find_bone_chain(skel, target_names):
    """Find a bone by name list and build parent chain. Returns (bone_index, chain) or (None, [])."""
    target = None
    for b in skel.bones:
        if b.name in target_names:
            target = b
            break
    if target is None:
        # Fuzzy match
        for b in skel.bones:
            for tn in target_names:
                if tn.lower() in b.name.lower():
                    target = b
                    break
            if target:
                break
    if target is None:
        return None, []
    chain = []
    b = target
    while b is not None:
        chain.append(b.index)
        b = b.parent
    chain.reverse()
    return target.index, chain


def find_r_hand_chain(skel):
    _, chain = find_bone_chain(skel, ['R_Hand', 'r_Hand', 'R_hand'])
    return chain


def find_l_hand_chain(skel):
    _, chain = find_bone_chain(skel, ['L_Hand', 'l_Hand', 'L_hand'])
    return chain


def find_root_index(skel):
    """Find root/master bone index for root motion tracking."""
    for b in skel.bones:
        if b.name in ('Master', 'Root', 'root', 'master', 'Hips', 'Pelvis'):
            return b.index
    # Fallback to first bone
    return 0


# Elden Ring animation ID ranges
ANIM_RANGES = {
    'idle':       (0, 999),
    'movement':   (1000, 2999),
    'attack':     (3000, 3999),
    'heavy':      (4000, 4999),
    'dodge':      (5000, 5999),
    'hit_react':  (6000, 6999),
    'guard':      (7000, 7999),
    'throw':      (8000, 8999),
    'special':    (9000, 9999),
    'death':      (10000, 11999),
}


def classify_anim_type(anim_id_num):
    """Classify animation type based on Elden Ring anim ID number."""
    for atype, (lo, hi) in ANIM_RANGES.items():
        if lo <= anim_id_num <= hi:
            return atype
    return 'other'


## compute_hand_positions removed — replaced by compute_bone_positions above


def compute_bone_positions(local_data, track_indices, chain):
    """Compute world positions for a bone chain across all frames."""
    if not chain:
        return []
    bone_to_track = {tbi: ti for ti, tbi in enumerate(track_indices)}
    positions = []
    for frame in local_data:
        pos = np.zeros(3)
        rot = Rotation.identity()
        for bi in chain:
            ti = bone_to_track.get(bi)
            if ti is not None and ti < len(frame):
                t = frame[ti]
                lp = np.array([t.translation.x, t.translation.y, t.translation.z], dtype=np.float64)
                lr = Rotation.from_quat(np.array(
                    [t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w], dtype=np.float64))
                pos = pos + rot.apply(lp)
                rot = rot * lr
        positions.append(pos.tolist())
    return positions


def compute_root_positions(local_data, track_indices, root_idx):
    """Track root bone translation only (no chain needed)."""
    bone_to_track = {tbi: ti for ti, tbi in enumerate(track_indices)}
    ti = bone_to_track.get(root_idx)
    if ti is None:
        return []
    positions = []
    for frame in local_data:
        if ti < len(frame):
            t = frame[ti]
            positions.append([t.translation.x, t.translation.y, t.translation.z])
        else:
            positions.append([0, 0, 0])
    return positions


def process_binder(binder, skel, r_chain, l_chain, root_idx, comp):
    results = []
    for entry in binder.entries:
        if not entry.name.endswith('.hkx'):
            continue
        name = entry.name.split('.')[0]
        m = re.match(r'a(\d+)_(\d+)', name)
        if not m:
            continue
        anim_id = int(m.group(2))

        # Expanded range: attacks (3000-4999), dodges (5000-5999), guards (7000-7999)
        if not (3000 <= anim_id <= 5999 or 7000 <= anim_id <= 7999):
            continue

        anim_type = classify_anim_type(anim_id)

        try:
            ah = AnimationHKX.from_bytes(entry.get_uncompressed_data(), compendium=comp)
            if not ah.animation_container.is_interleaved:
                ah = ah.to_interleaved_hkx()
            local_data = ah.animation_container.interleaved_data
            track_indices = list(ah.animation_container.get_track_bone_indices())

            # R_Hand analysis (primary)
            r_hand_pos = compute_bone_positions(local_data, track_indices, r_chain)

            # Root motion
            root_pos = compute_root_positions(local_data, track_indices, root_idx)
            root_info = analyze_root_motion(root_pos)

            # L_Hand analysis
            l_hand_pos = compute_bone_positions(local_data, track_indices, l_chain)
            l_hand_info = analyze_l_hand(l_hand_pos)

            # Main analysis from R_Hand
            if r_hand_pos:
                analysis = analyze_attack(r_hand_pos)
            else:
                analysis = None

            if analysis is None and anim_type in ('dodge', 'guard'):
                # For dodges/guards, create a basic entry even without R_Hand movement
                duration = ah.animation_container.hkx_animation.duration
                analysis = {
                    'direction': 'n/a',
                    'hit_count': 0,
                    'speed_class': 'medium',
                    'range_class': 'close',
                    'arc_type': 'n/a',
                    'peak_velocity': 0.0,
                    'windup_time': 0.0,
                    'recovery_time': 0.0,
                    'total_duration': round(duration, 3),
                    'max_range': 0.0,
                    'frames': len(local_data),
                }
            elif analysis is None:
                continue

            analysis['anim_id'] = name
            analysis['anim_type'] = anim_type
            analysis.update(root_info)
            analysis.update(l_hand_info)
            results.append(analysis)
        except Exception:
            continue
    return results


def process_character(char_id):
    base_path = os.path.join(UNPACKED, f'{char_id}.anibnd.dcx')
    if not os.path.exists(base_path):
        return []
    try:
        base = Binder.from_path(base_path)
    except:
        return []

    # Get compendium
    base_comp = None
    try:
        base_comp = HKX.from_binder_entry(base.find_entry_matching_name(r'.*\.compendium'))
    except:
        for f in sorted(os.listdir(UNPACKED)):
            if f.startswith(f'{char_id}_div') and f.endswith('.anibnd.dcx'):
                try:
                    div_b = Binder.from_path(os.path.join(UNPACKED, f))
                    base_comp = HKX.from_binder_entry(div_b.find_entry_matching_name(r'.*\.compendium'))
                    break
                except:
                    continue

    # Get skeleton
    try:
        se = base.find_entry_matching_name(r'skeleton\.hkx', flags=re.IGNORECASE)
        skel_hkx = SkeletonHKX.from_bytes(se.get_uncompressed_data(), compendium=base_comp)
        skel = skel_hkx.skeleton
        r_chain = find_r_hand_chain(skel)
        l_chain = find_l_hand_chain(skel)
        root_idx = find_root_index(skel)
    except:
        return []
    if not r_chain:
        return []

    # Process base + divs
    all_results = process_binder(base, skel, r_chain, l_chain, root_idx, base_comp)
    for f in sorted(os.listdir(UNPACKED)):
        if f.startswith(f'{char_id}_div') and f.endswith('.anibnd.dcx'):
            try:
                div_b = Binder.from_path(os.path.join(UNPACKED, f))
                div_comp = base_comp
                try:
                    div_comp = HKX.from_binder_entry(div_b.find_entry_matching_name(r'.*\.compendium'))
                except:
                    pass
                all_results.extend(process_binder(div_b, skel, r_chain, l_chain, root_idx, div_comp))
            except:
                continue

    # Deduplicate
    seen = set()
    deduped = []
    for r in all_results:
        if r['anim_id'] not in seen:
            seen.add(r['anim_id'])
            deduped.append(r)
    return deduped


def write_partial(char_id, name, results):
    path = os.path.join(PARTIAL_DIR, f"enhanced_{char_id}.yaml")
    with open(path, 'w') as f:
        if not results:
            f.write(f"# {char_id} ({name}): 0 animations\n")
            return

        # Count by type
        type_counts = {}
        for r in results:
            t = r.get('anim_type', 'attack')
            type_counts[t] = type_counts.get(t, 0) + 1
        type_str = ", ".join(f"{v} {k}" for k, v in sorted(type_counts.items()))
        f.write(f"# {char_id} ({name}): {len(results)} animations ({type_str})\n")

        for r in sorted(results, key=lambda x: x['anim_id']):
            f.write(f"{r['anim_id']}:\n")
            f.write(f"  anim_type: {r.get('anim_type', 'attack')}\n")
            f.write(f"  direction: {r['direction']}\n")
            if r.get('second_direction'):
                f.write(f"  second_direction: {r['second_direction']}\n")
            f.write(f"  hit_count: {r['hit_count']}\n")
            f.write(f"  speed_class: {r['speed_class']}\n")
            f.write(f"  range_class: {r['range_class']}\n")
            f.write(f"  arc_type: {r['arc_type']}\n")
            f.write(f"  peak_velocity: {r['peak_velocity']}\n")
            f.write(f"  windup_time: {r['windup_time']}\n")
            f.write(f"  recovery_time: {r['recovery_time']}\n")
            f.write(f"  total_duration: {r['total_duration']}\n")
            f.write(f"  max_range: {r['max_range']}\n")
            f.write(f"  frames: {r['frames']}\n")
            # Root motion
            f.write(f"  root_distance: {r.get('root_distance', 0.0)}\n")
            f.write(f"  root_direction: {r.get('root_direction', 'stationary')}\n")
            # L_Hand
            if r.get('l_hand_behavior'):
                f.write(f"  l_hand_behavior: {r['l_hand_behavior']}\n")
                f.write(f"  l_hand_range: {r.get('l_hand_range', 0.0)}\n")


def merge_catalog(total_chars, total_attacks):
    all_data = {}
    stat_counts = {'anim_type': {}, 'direction': {}, 'speed_class': {}, 'range_class': {},
                    'hit_count': {}, 'arc_type': {}, 'root_direction': {}, 'l_hand_behavior': {}}

    for f in sorted(os.listdir(PARTIAL_DIR)):
        if not f.startswith('enhanced_c') or not f.endswith('.yaml'):
            continue
        m = re.match(r'enhanced_(c\d+)\.yaml', f)
        if not m:
            continue
        cid = m.group(1)
        name = CHAR_NAMES.get(cid, "")

        with open(os.path.join(PARTIAL_DIR, f), 'r') as fh:
            content = fh.read()

        attacks = {}
        current = None
        for line in content.split('\n'):
            if line.startswith('#') or not line.strip():
                continue
            if not line.startswith(' ') and line.endswith(':'):
                current = line.rstrip(':')
                attacks[current] = {}
            elif current and ':' in line:
                key, val = line.strip().split(':', 1)
                val = val.strip()
                try:
                    val = float(val)
                    if val == int(val):
                        val = int(val)
                except:
                    pass
                attacks[current][key] = val

        if attacks:
            all_data[cid] = {'name': name, 'attacks': attacks}
            for ad in attacks.values():
                for stat_key in stat_counts:
                    v = ad.get(stat_key)
                    if v is not None:
                        stat_counts[stat_key][v] = stat_counts[stat_key].get(v, 0) + 1

    out_path = 'C:/scripts/SLFConversion/swing_catalog.yaml'
    with open(out_path, 'w', encoding='utf-8') as f:
        f.write("# ============================================================================\n")
        f.write("# ELDEN RING — Enhanced Attack Animation Catalog (ALL CHARACTERS)\n")
        f.write("# ============================================================================\n")
        f.write("# Per-animation combat context: direction, hit count, speed class, range,\n")
        f.write("# arc type, wind-up time, recovery time, peak velocity\n")
        f.write("# ============================================================================\n\n")

        total_anims = sum(len(v['attacks']) for v in all_data.values())
        f.write(f"# Total: {total_anims} attacks across {len(all_data)} characters\n#\n")

        for stat_key, counts in stat_counts.items():
            f.write(f"# {stat_key} distribution:\n")
            for v, c in sorted(counts.items(), key=lambda x: -x[1]):
                f.write(f"#   {v}: {c}\n")
            f.write("#\n")
        f.write("\n")

        for cid in sorted(all_data.keys()):
            info = all_data[cid]
            name = info['name']
            attacks = info['attacks']
            f.write(f"# --- {cid}: {name} ({len(attacks)} attacks) ---\n")
            for anim_id, data in sorted(attacks.items()):
                f.write(f"{cid}_{anim_id}:\n")
                f.write(f"  source: {cid}\n")
                f.write(f"  source_name: \"{name}\"\n")
                f.write(f"  anim_id: {anim_id}\n")
                for k, v in data.items():
                    if k == 'anim_id':
                        continue
                    f.write(f"  {k}: {v}\n")
                f.write("\n")

    print(f"Wrote {total_anims} entries to {out_path}")


def main():
    all_chars = []
    for f in sorted(os.listdir(UNPACKED)):
        m = re.match(r'(c\d+)\.anibnd\.dcx$', f)
        if m and '_div' not in f and '_a0' not in f and m.group(1) != 'c0000':
            all_chars.append(m.group(1))

    total = len(all_chars)
    total_attacks = 0
    completed = []
    start_time = time.time()

    print(f"Enhanced analysis: {total} characters...", flush=True)

    for idx, cid in enumerate(all_chars):
      try:
        name = CHAR_NAMES.get(cid, "")
        elapsed = time.time() - start_time
        rate = (idx + 1) / max(elapsed, 1)
        eta = int((total - idx - 1) / max(rate, 0.01))

        pct = int((idx + 1) / total * 30)
        bar = '#' * pct + '-' * (30 - pct)
        pct_num = int((idx + 1) / total * 100)

        completed_str = "\n".join(f"  {c}: {n} attacks" for c, n in completed[-15:])
        with open(PROGRESS_FILE, 'w') as pf:
            pf.write(f"ENHANCED SWING ANALYSIS\n")
            pf.write(f"=======================\n")
            pf.write(f"[{bar}] {pct_num}% ({idx+1}/{total})\n\n")
            pf.write(f"Current: {cid} ({name})\n")
            pf.write(f"Attacks found: {total_attacks}\n")
            pf.write(f"ETA: {eta//60}m{eta%60}s\n\n")
            if completed:
                pf.write(f"Recent:\n{completed_str}\n")

        print(f"[{idx+1}/{total}] {cid} ({name})...", end=" ", flush=True)

        results = process_character(cid)
        total_attacks += len(results)
        write_partial(cid, name, results)

        completed.append((cid, len(results)))
        print(f"{len(results)} attacks (ETA: {eta//60}m{eta%60}s)", flush=True)
      except Exception as ex:
        print(f"ERROR {cid}: {ex}", flush=True)
        continue

    print(f"\nMerging into swing_catalog.yaml...", flush=True)
    merge_catalog(total, total_attacks)

    with open(PROGRESS_FILE, 'w') as pf:
        pf.write(f"COMPLETE!\n=========\n")
        pf.write(f"Characters: {total}\n")
        pf.write(f"Attacks: {total_attacks}\n")
        pf.write(f"Output: C:/scripts/SLFConversion/swing_catalog.yaml\n")

    print(f"DONE! {total_attacks} attacks across {total} characters", flush=True)


if __name__ == '__main__':
    main()
