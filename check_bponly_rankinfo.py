"""
Check RankInfo configuration on status effect data assets in bp_only backup.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/check_bponly_rankinfo_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

STATUS_EFFECTS = [
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison",
]

def check_rankinfo():
    log("=" * 60)
    log("Checking Status Effect RankInfo Configuration")
    log("=" * 60)

    for asset_path in STATUS_EFFECTS:
        log(f"\n{asset_path.split('/')[-1]}")
        log("-" * 40)

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log(f"  ERROR: Could not load asset")
            continue

        log(f"  Class: {asset.get_class().get_name()}")

        try:
            rank_info = asset.get_editor_property("rank_info")
            log(f"  RankInfo Length: {len(rank_info) if rank_info else 0}")

            if rank_info:
                for rank, info in rank_info.items():
                    log(f"\n  Rank {rank}:")
                    # Try to dump all properties
                    if hasattr(info, 'get_editor_property'):
                        for prop_name in ['trigger_vfx', 'loop_vfx', 'buildup_rate', 'decay_rate', 'damage', 'duration', 'interval']:
                            try:
                                value = info.get_editor_property(prop_name)
                                log(f"    {prop_name}: {value}")
                            except:
                                pass
                    else:
                        # It's a simple struct
                        log(f"    {info}")
            else:
                log("  No RankInfo configured!")

            # Also check for TMap iteration
            log(f"\n  All rank keys: {list(rank_info.keys()) if rank_info else 'None'}")

        except Exception as e:
            log(f"  ERROR: {e}")
            import traceback
            log(traceback.format_exc())

    log(f"\n" + "=" * 60)

# Run
check_rankinfo()

# Save log
with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog: {OUTPUT_FILE}")
