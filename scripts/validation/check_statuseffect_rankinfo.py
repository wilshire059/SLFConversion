"""
Check RankInfo configuration on status effect data assets.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/check_statuseffect_rankinfo_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

STATUS_EFFECTS = [
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn",
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

        try:
            rank_info = asset.get_editor_property("rank_info")
            log(f"  RankInfo Type: {type(rank_info)}")
            log(f"  RankInfo Length: {len(rank_info) if rank_info else 0}")

            if rank_info:
                for rank, info in rank_info.items():
                    log(f"  Rank {rank}:")
                    # Try to access VFX info
                    try:
                        trigger_vfx = info.get_editor_property("trigger_vfx") if hasattr(info, 'get_editor_property') else None
                        loop_vfx = info.get_editor_property("loop_vfx") if hasattr(info, 'get_editor_property') else None
                        log(f"    TriggerVFX: {trigger_vfx}")
                        log(f"    LoopVFX: {loop_vfx}")
                    except:
                        log(f"    Info: {info}")
            else:
                log("  No RankInfo configured!")

        except Exception as e:
            log(f"  ERROR: {e}")

    log(f"\n" + "=" * 60)

# Run
check_rankinfo()

# Save log
with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog: {OUTPUT_FILE}")
