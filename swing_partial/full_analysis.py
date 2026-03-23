"""Full swing direction analysis — pure Python, no Blender.
Reads HKX directly from ANIBNDs including div files with per-div compendiums."""
import sys, os, re, math, time
import numpy as np
from scipy.spatial.transform import Rotation

sys.path.insert(0, 'C:/scripts/elden_ring_tools/soulstruct-havok/src')
sys.path.insert(0, 'C:/scripts/elden_ring_tools/soulstruct/src')

# Suppress soulstruct's rich console logger that crashes on Windows cp1252
import logging
logging.disable(logging.CRITICAL)

from soulstruct.containers import Binder
from soulstruct.havok.core import HKX
from soulstruct.havok.fromsoft.eldenring import AnimationHKX, SkeletonHKX

# Re-enable our own print output
logging.disable(logging.NOTSET)
logging.getLogger('soulstruct').setLevel(logging.FATAL + 1)

UNPACKED = 'C:/scripts/elden_ring_tools/unpacked/chr'
PARTIAL_DIR = 'C:/scripts/SLFConversion/swing_partial'
PROGRESS_FILE = 'C:/scripts/SLFConversion/swing_progress.txt'

CHAR_NAMES = {
    "c0000": "Player", "c2030": "Godrick", "c2031": "Godrick Phase 2",
    "c2040": "Rennala", "c2041": "Rennala Phase 2", "c2050": "Rykard",
    "c2060": "Radahn", "c2100": "Black Knife", "c2110": "Crucible Knight Boss",
    "c2120": "Malenia", "c2130": "Mohg", "c2131": "Mohg Phase 2",
    "c2140": "Fire Giant", "c2190": "Morgott", "c2500": "Crucible Knight Alt",
    "c3000": "Exile Soldier", "c3010": "Banished Knight", "c3050": "Commander Niall",
    "c3100": "Crucible Knight", "c3150": "Redmane Knight", "c3160": "Carian Knight",
    "c3180": "Red Wolf", "c3181": "Red Wolf Alt", "c3200": "Battlemage",
    "c3210": "Fire Prelate", "c3250": "Crucible Tree Knight",
    "c3300": "Nox Swordstress", "c3400": "Grave Warden", "c3450": "Misbegotten",
    "c3500": "Skeleton", "c3550": "Sanguine Noble", "c3560": "Giant Crab",
    "c3610": "Oracle Envoy", "c3620": "Oracle Envoy Large",
    "c3630": "Abnormal Stone Cluster", "c3660": "Commoner",
    "c3670": "Demi-Human", "c3700": "Raya Lucaria Scholar",
    "c3800": "Cleanrot Knight", "c3900": "Fire Monk", "c3910": "Blackflame Monk",
    "c3950": "Man-Serpent", "c3970": "Azula Beastman",
    "c4050": "Kaiden Sellsword", "c4060": "Horse", "c4080": "Lion Guardian",
    "c4090": "Grafted Scion", "c4100": "Imp", "c4110": "Ancestral Follower",
    "c4160": "Prisoner", "c4270": "Leyndell Knight", "c4290": "Bloodhound Knight",
    "c4300": "Godrick Soldier", "c4310": "Lordsworn Soldier",
    "c4320": "Vulgar Militia", "c4340": "Pumpkin Head",
    "c4350": "Lordsworn Knight", "c4370": "Haligtree Knight",
    "c4377": "Haligtree Soldier", "c4380": "Starcaller",
    "c4470": "Inquisitor Ghiza", "c4810": "Erdtree Avatar",
    "c4820": "Ulcerated Tree Spirit", "c5830": "Messmer Soldier",
    "c5840": "Black Knight",
}


def classify_swing(hand_positions):
    if len(hand_positions) < 3:
        return "static", 0.0
    max_v = 0
    peak_idx = 0
    for i in range(1, len(hand_positions)):
        dx = hand_positions[i][0] - hand_positions[i-1][0]
        dy = hand_positions[i][1] - hand_positions[i-1][1]
        dz = hand_positions[i][2] - hand_positions[i-1][2]
        v = math.sqrt(dx*dx + dy*dy + dz*dz)
        if v > max_v:
            max_v = v
            peak_idx = i - 1
    if max_v < 0.001:
        return "static", 0.0

    start = max(0, peak_idx - 2)
    end = min(len(hand_positions) - 1, peak_idx + 3)
    total_dx = hand_positions[end][0] - hand_positions[start][0]
    total_dy = hand_positions[end][1] - hand_positions[start][1]
    total_dz = hand_positions[end][2] - hand_positions[start][2]
    mag = math.sqrt(total_dx**2 + total_dy**2 + total_dz**2)
    if mag < 0.001:
        return "static", 0.0
    nx, ny, nz = total_dx/mag, total_dy/mag, total_dz/mag
    abs_x, abs_y, abs_z = abs(nx), abs(ny), abs(nz)

    if abs_z > 0.6:
        if nz < 0:
            if abs_x > 0.3:
                return ("diag_r_down" if nx > 0 else "diag_l_down"), max_v
            return "overhead", max_v
        else:
            if abs_x > 0.3:
                return ("diag_r_up" if nx > 0 else "diag_l_up"), max_v
            return "uppercut", max_v
    elif abs_x > 0.5:
        return ("right_to_left" if nx > 0 else "left_to_right"), max_v
    elif abs_y > 0.5:
        return ("thrust" if ny > 0 else "pull_back"), max_v
    else:
        if abs_x > abs_y:
            return ("right_to_left" if nx > 0 else "left_to_right"), max_v
        return "thrust", max_v


def find_r_hand_bone(skel):
    """Find R_Hand bone and build parent chain."""
    r_hand = None
    for b in skel.bones:
        if b.name in ('R_Hand', 'r_Hand', 'R_hand'):
            r_hand = b
            break
    if r_hand is None:
        for b in skel.bones:
            if 'hand' in b.name.lower() and b.name.lower().startswith('r'):
                r_hand = b
                break
    if r_hand is None:
        return None, []

    # Build parent chain
    chain = []
    b = r_hand
    while b is not None:
        chain.append(b.index)
        b = b.parent
    chain.reverse()
    return r_hand.index, chain


def compute_hand_positions(local_data, track_indices, chain):
    """Compute R_Hand world positions for each frame using parent chain."""
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


def process_binder(binder, skel, chain, comp):
    results = []
    for entry in binder.entries:
        if not entry.name.endswith('.hkx'):
            continue
        name = entry.name.split('.')[0]
        m = re.match(r'a(\d+)_(\d+)', name)
        if not m:
            continue
        anim_id = int(m.group(2))
        if not (3000 <= anim_id <= 4999):
            continue
        try:
            ah = AnimationHKX.from_bytes(entry.get_uncompressed_data(), compendium=comp)
            if not ah.animation_container.is_interleaved:
                ah = ah.to_interleaved_hkx()
            local_data = ah.animation_container.interleaved_data
            track_indices = list(ah.animation_container.get_track_bone_indices())
            duration = ah.animation_container.hkx_animation.duration

            hand_positions = compute_hand_positions(local_data, track_indices, chain)
            if not hand_positions:
                continue
            direction, velocity = classify_swing(hand_positions)
            results.append({
                'anim_id': name,
                'direction': direction,
                'velocity': round(velocity, 4),
                'frames': len(hand_positions),
                'duration': round(duration, 3),
            })
        except Exception as ex:
            import traceback
            sys.stderr.write(f"  {entry.name}: {ex}\n")
            continue
    return results


def process_character(char_id):
    """Process one character: base ANIBND + all div files."""
    base_path = os.path.join(UNPACKED, f'{char_id}.anibnd.dcx')
    if not os.path.exists(base_path):
        return []

    # Load base binder for skeleton
    try:
        base = Binder.from_path(base_path)
    except:
        return []

    # Get compendium from base
    base_comp = None
    try:
        ce = base.find_entry_matching_name(r'.*\.compendium')
        base_comp = HKX.from_binder_entry(ce)
    except:
        pass

    # Get skeleton
    skel = None
    chain = []
    try:
        se = base.find_entry_matching_name(r'skeleton\.hkx', flags=re.IGNORECASE)
        comp_for_skel = base_comp
        # If no base compendium, try first div's compendium
        if comp_for_skel is None:
            for f in sorted(os.listdir(UNPACKED)):
                if f.startswith(f'{char_id}_div') and f.endswith('.anibnd.dcx'):
                    try:
                        div_b = Binder.from_path(os.path.join(UNPACKED, f))
                        div_ce = div_b.find_entry_matching_name(r'.*\.compendium')
                        comp_for_skel = HKX.from_binder_entry(div_ce)
                        break
                    except:
                        continue

        skel_hkx = SkeletonHKX.from_bytes(se.get_uncompressed_data(), compendium=comp_for_skel)
        skel = skel_hkx.skeleton
        r_hand_idx, chain = find_r_hand_bone(skel)
    except:
        pass

    if skel is None or not chain:
        return []

    # Process base binder attacks
    all_results = process_binder(base, skel, chain, base_comp)

    # Process div binders
    for f in sorted(os.listdir(UNPACKED)):
        if f.startswith(f'{char_id}_div') and f.endswith('.anibnd.dcx'):
            try:
                div_b = Binder.from_path(os.path.join(UNPACKED, f))
                # Each div may have its own compendium
                div_comp = None
                try:
                    div_ce = div_b.find_entry_matching_name(r'.*\.compendium')
                    div_comp = HKX.from_binder_entry(div_ce)
                except:
                    div_comp = base_comp  # Fallback to base compendium

                results = process_binder(div_b, skel, chain, div_comp)
                all_results.extend(results)
            except:
                continue

    # Deduplicate by anim_id (divs may have overlapping anims)
    seen = set()
    deduped = []
    for r in all_results:
        if r['anim_id'] not in seen:
            seen.add(r['anim_id'])
            deduped.append(r)

    return deduped


def main():
    # Find ALL character base ANIBNDs (skip c0000 player — too large, not useful for enemies)
    all_chars = []
    for f in sorted(os.listdir(UNPACKED)):
        m = re.match(r'(c\d+)\.anibnd\.dcx$', f)
        if m and '_div' not in f and '_a0' not in f and m.group(1) != 'c0000':
            all_chars.append(m.group(1))

    total = len(all_chars)
    total_attacks = 0
    completed = []
    start_time = time.time()

    print(f"Processing {total} characters...", flush=True)
    sys.stdout.flush()

    for idx, cid in enumerate(all_chars):
      try:
        name = CHAR_NAMES.get(cid, "")
        elapsed = time.time() - start_time
        rate = (idx + 1) / max(elapsed, 1)
        eta = int((total - idx - 1) / max(rate, 0.01))

        # Update progress
        pct = int((idx + 1) / total * 30)
        bar = '#' * pct + '-' * (30 - pct)
        pct_num = int((idx + 1) / total * 100)

        completed_str = "\n".join(f"  {c}: {n} attacks" for c, n in completed[-15:])
        with open(PROGRESS_FILE, 'w') as pf:
            pf.write(f"SWING DIRECTION ANALYSIS (FULL RERUN)\n")
            pf.write(f"=====================================\n")
            pf.write(f"[{bar}] {pct_num}% ({idx+1}/{total})\n\n")
            pf.write(f"Current: {cid} ({name})\n")
            pf.write(f"Attacks found: {total_attacks}\n")
            pf.write(f"ETA: {eta//60}m{eta%60}s\n\n")
            if completed:
                pf.write(f"Recent:\n{completed_str}\n")

        print(f"[{idx+1}/{total}] {cid} ({name})...", end=" ", flush=True)

        results = process_character(cid)
        total_attacks += len(results)

        # Write partial
        partial_path = os.path.join(PARTIAL_DIR, f"swing_{cid}.yaml")
        with open(partial_path, 'w') as pf:
            if results:
                pf.write(f"# {cid} ({name}): {len(results)} attack animations\n")
                for r in sorted(results, key=lambda x: x['anim_id']):
                    pf.write(f"{r['anim_id']}:\n")
                    pf.write(f"  direction: {r['direction']}\n")
                    pf.write(f"  velocity: {r['velocity']}\n")
                    pf.write(f"  frames: {r['frames']}\n")
                    pf.write(f"  duration: {r['duration']}\n")
            else:
                pf.write(f"# {cid} ({name}): 0 attack animations found\n")

        completed.append((cid, len(results)))
        print(f"{len(results)} attacks (ETA: {eta//60}m{eta%60}s)", flush=True)
      except Exception as ex:
        print(f"CHAR ERROR {cid}: {ex}", flush=True)
        continue

    # Write final catalog
    print(f"\nMerging into swing_catalog.yaml...", flush=True)
    merge_all(total, total_attacks)

    with open(PROGRESS_FILE, 'w') as pf:
        pf.write(f"COMPLETE!\n=========\n")
        pf.write(f"Characters: {total}\n")
        pf.write(f"Attacks: {total_attacks}\n")
        pf.write(f"Output: C:/scripts/SLFConversion/swing_catalog.yaml\n")

    print(f"DONE! {total_attacks} attacks across {total} characters", flush=True)


def merge_all(total_chars, total_attacks):
    all_data = {}
    dir_counts = {}

    for f in sorted(os.listdir(PARTIAL_DIR)):
        if not f.startswith('swing_c') or not f.endswith('.yaml'):
            continue
        m = re.match(r'swing_(c\d+)\.yaml', f)
        if not m:
            continue
        cid = m.group(1)
        name = CHAR_NAMES.get(cid, "")

        with open(os.path.join(PARTIAL_DIR, f), 'r') as fh:
            content = fh.read()

        attacks = {}
        current_anim = None
        for line in content.split('\n'):
            if line.startswith('#') or not line.strip():
                continue
            if not line.startswith(' ') and line.endswith(':'):
                current_anim = line.rstrip(':')
                attacks[current_anim] = {}
            elif current_anim and ':' in line:
                key, val = line.strip().split(':', 1)
                val = val.strip()
                try:
                    val = float(val)
                    if val == int(val):
                        val = int(val)
                except:
                    pass
                attacks[current_anim][key] = val

        if attacks:
            all_data[cid] = {'name': name, 'attacks': attacks}
            for anim_data in attacks.values():
                d = anim_data.get('direction', 'unknown')
                dir_counts[d] = dir_counts.get(d, 0) + 1

    out_path = 'C:/scripts/SLFConversion/swing_catalog.yaml'
    with open(out_path, 'w', encoding='utf-8') as f:
        f.write("# ============================================================================\n")
        f.write("# ELDEN RING — Attack Swing Direction Catalog (ALL CHARACTERS)\n")
        f.write("# ============================================================================\n")
        f.write("# Auto-generated from R_Hand bone trajectory during peak velocity\n")
        f.write("# Directions: right_to_left, left_to_right, overhead, uppercut, thrust,\n")
        f.write("#   diag_r_down, diag_l_down, diag_r_up, diag_l_up, spin, static\n")
        f.write("# ============================================================================\n\n")

        total_anims = sum(len(v['attacks']) for v in all_data.values())
        f.write(f"# Total: {total_anims} attack animations across {len(all_data)} characters\n")
        f.write(f"# Direction distribution:\n")
        for d, c in sorted(dir_counts.items(), key=lambda x: -x[1]):
            f.write(f"#   {d}: {c}\n")
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
                    f.write(f"  {k}: {v}\n")
                f.write("\n")

    print(f"Wrote {total_anims} entries to {out_path}")


if __name__ == '__main__':
    main()
