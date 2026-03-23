"""Find animations with real root motion using reference_frame_samples."""
import sys, os, re, logging, time
import numpy as np
logging.disable(logging.CRITICAL)
sys.path.insert(0, 'C:/scripts/elden_ring_tools/soulstruct-havok/src')
sys.path.insert(0, 'C:/scripts/elden_ring_tools/soulstruct/src')
from soulstruct.containers import Binder
from soulstruct.havok.core import HKX
from soulstruct.havok.fromsoft.eldenring import AnimationHKX, SkeletonHKX
logging.disable(logging.NOTSET)
logging.getLogger('soulstruct').setLevel(logging.FATAL + 1)

UNPACKED = 'C:/scripts/elden_ring_tools/unpacked/chr'
OUT_FILE = 'C:/scripts/SLFConversion/root_movement_results.txt'
PROGRESS = 'C:/scripts/SLFConversion/swing_progress.txt'
NAMES = {
    'c3000': 'Exile Soldier', 'c3010': 'Banished Knight', 'c3050': 'Commander Niall',
    'c3100': 'Crucible Knight', 'c3150': 'Redmane Knight', 'c3160': 'Carian Knight',
    'c3200': 'Battlemage', 'c3250': 'Crucible Tree Knight',
    'c3300': 'Nox Swordstress', 'c3400': 'Grave Warden',
    'c3450': 'Misbegotten', 'c3500': 'Skeleton', 'c3550': 'Sanguine Noble',
    'c3670': 'Demi-Human', 'c3700': 'Raya Lucaria Scholar',
    'c3800': 'Cleanrot Knight', 'c3900': 'Fire Monk', 'c3910': 'Blackflame Monk',
    'c3950': 'Man-Serpent', 'c3970': 'Azula Beastman',
    'c4050': 'Kaiden Sellsword', 'c4090': 'Grafted Scion',
    'c4110': 'Ancestral Follower', 'c4270': 'Leyndell Knight',
    'c4290': 'Bloodhound Knight', 'c4300': 'Godrick Soldier',
    'c4310': 'Lordsworn Soldier', 'c4320': 'Vulgar Militia',
    'c4340': 'Pumpkin Head', 'c4350': 'Lordsworn Knight',
    'c4370': 'Haligtree Knight', 'c4380': 'Starcaller',
    'c5830': 'Messmer Soldier', 'c5840': 'Black Knight',
}

results = []
start = time.time()
for idx, src in enumerate(sorted(NAMES.keys())):
    elapsed = time.time() - start
    rate = (idx + 1) / max(elapsed, 1)
    eta = int((len(NAMES) - idx - 1) / max(rate, 0.01))
    with open(PROGRESS, 'w') as pf:
        pf.write("ROOT MOTION SCAN (reference_frame_samples)\n")
        pf.write(f"[{idx+1}/{len(NAMES)}] {src} ({NAMES[src]})\n")
        pf.write(f"Found: {len(results)} with 2.5+ displacement\n")
        pf.write(f"ETA: {eta//60}m{eta%60}s\n")

    try:
        base = Binder.from_path(os.path.join(UNPACKED, f'{src}.anibnd.dcx'))
        comp = None
        try:
            comp = HKX.from_binder_entry(base.find_entry_matching_name(r'.*\.compendium'))
        except Exception:
            for f in sorted(os.listdir(UNPACKED)):
                if f.startswith(f'{src}_div') and f.endswith('.anibnd.dcx'):
                    try:
                        div = Binder.from_path(os.path.join(UNPACKED, f))
                        comp = HKX.from_binder_entry(div.find_entry_matching_name(r'.*\.compendium'))
                        break
                    except Exception:
                        continue

        seen = set()
        for f in sorted(os.listdir(UNPACKED)):
            if (f == f'{src}.anibnd.dcx' or f.startswith(f'{src}_div')) and f.endswith('.anibnd.dcx'):
                try:
                    binder = Binder.from_path(os.path.join(UNPACKED, f))
                    bc = comp
                    try:
                        bc = HKX.from_binder_entry(binder.find_entry_matching_name(r'.*\.compendium'))
                    except Exception:
                        pass
                    for e in binder.entries:
                        if not e.name.endswith('.hkx'):
                            continue
                        name = e.name.split('.')[0]
                        if name in seen:
                            continue
                        m = re.match(r'a\d+_(\d+)', name)
                        if not m:
                            continue
                        num = int(m.group(1))
                        if not (1800 <= num <= 7999):
                            continue
                        try:
                            ah = AnimationHKX.from_bytes(e.get_uncompressed_data(), compendium=bc)
                            if not ah.animation_container.is_interleaved:
                                ah = ah.to_interleaved_hkx()
                            rm = ah.animation_container.get_reference_frame_samples()
                            if rm is None:
                                seen.add(name)
                                continue
                            rm = np.array(rm)
                            if rm.shape[0] < 2:
                                seen.add(name)
                                continue

                            dur = ah.animation_container.hkx_animation.duration
                            net = rm[-1] - rm[0]
                            net_x = float(net[0])
                            net_z = float(net[2])
                            total = float(np.sqrt(net_x**2 + net_z**2))
                            max_back_z = float(np.max(rm[:, 2] - rm[0, 2]))
                            max_fwd_z = float(np.min(rm[:, 2] - rm[0, 2]))
                            max_lat = float(np.max(np.abs(rm[:, 0] - rm[0, 0])))

                            if total >= 2.5 or max_back_z >= 2.5 or abs(max_fwd_z) >= 2.5 or max_lat >= 2.5:
                                if 3000 <= num <= 3999: atype = 'attack'
                                elif 4000 <= num <= 4999: atype = 'heavy'
                                elif 5000 <= num <= 5999: atype = 'dodge'
                                elif 1800 <= num <= 1999: atype = 'backstep'
                                elif 2000 <= num <= 2999: atype = 'movement'
                                elif 6000 <= num <= 6999: atype = 'hit_react'
                                elif 7000 <= num <= 7999: atype = 'guard'
                                else: atype = 'other'

                                if net_z > 2.5 and abs(net_x) < abs(net_z) * 0.5:
                                    direction = 'BACKSTEP'
                                elif net_z < -2.5 and abs(net_x) < abs(net_z) * 0.5:
                                    direction = 'LUNGE_FWD'
                                elif abs(net_x) > 2.5 and abs(net_z) < abs(net_x) * 0.5:
                                    direction = 'SIDESTEP'
                                elif max_back_z > 2.5 and net_z < 0:
                                    direction = 'BACK_THEN_LUNGE'
                                else:
                                    direction = 'DIAGONAL'

                                results.append({
                                    'src': src, 'name': NAMES[src], 'aid': name,
                                    'atype': atype, 'direction': direction,
                                    'net_x': net_x, 'net_z': net_z, 'total': total,
                                    'max_back': max_back_z, 'max_fwd': max_fwd_z,
                                    'max_lat': max_lat, 'dur': dur
                                })
                            seen.add(name)
                        except Exception:
                            seen.add(name)
                            continue
                except Exception:
                    continue
    except Exception:
        continue

results.sort(key=lambda x: (x['direction'], -x['total']))
with open(OUT_FILE, 'w') as f:
    f.write(f'Found {len(results)} animations with 2.5+ root motion displacement\n\n')
    current_dir = ''
    for r in results:
        if r['direction'] != current_dir:
            current_dir = r['direction']
            count = sum(1 for x in results if x['direction'] == current_dir)
            f.write(f"\n=== {current_dir} ({count}) ===\n")
        f.write(f"  {r['src']:6s} {r['name']:22s} {r['aid']:20s} [{r['atype']:8s}] "
                f"netX={r['net_x']:+7.2f} netZ={r['net_z']:+7.2f} total={r['total']:5.1f} "
                f"maxBack={r['max_back']:+5.1f} maxLat={r['max_lat']:4.1f} dur={r['dur']:4.1f}s\n")

with open(PROGRESS, 'w') as pf:
    pf.write(f'COMPLETE!\nFound: {len(results)}\nOutput: {OUT_FILE}\n')
