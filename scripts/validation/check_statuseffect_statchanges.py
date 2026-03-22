"""
Check what stat change data is configured on status effect data assets.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/check_statuseffect_statchanges_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

STATUS_EFFECTS = [
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed",
]

def check_statchanges():
    log("=" * 60)
    log("Checking Status Effect Stat Change Configuration")
    log("=" * 60)

    for asset_path in STATUS_EFFECTS:
        log(f"\n{asset_path.split('/')[-1]}")
        log("-" * 40)

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log(f"  ERROR: Could not load asset")
            continue

        log(f"  Class: {asset.get_class().get_name()}")

        # Try to get all properties
        try:
            for prop_name in ['base_buildup_rate', 'base_decay_rate', 'decay_delay',
                              'tick_stat_change', 'one_shot_stat_change', 'tick_and_one_shot_stat_change',
                              'duration', 'interval', 'damage_stat_tag', 'damage_amount',
                              'resistive_stat', 'resistive_stat_curve']:
                try:
                    value = asset.get_editor_property(prop_name)
                    if value is not None:
                        log(f"  {prop_name}: {value}")
                except:
                    pass

            # Check for nested struct properties
            log("\n  Checking nested structs:")

            # tick_stat_change
            try:
                tick = asset.get_editor_property("tick_stat_change")
                if tick:
                    log(f"  tick_stat_change: {tick}")
                    try:
                        ticking = tick.get_editor_property("ticking_stat_adjustment")
                        log(f"    ticking_stat_adjustment: {ticking} (len={len(ticking) if ticking else 0})")
                    except Exception as e:
                        log(f"    Could not read ticking_stat_adjustment: {e}")
            except Exception as e:
                log(f"  tick_stat_change error: {e}")

            # one_shot_stat_change
            try:
                oneshot = asset.get_editor_property("one_shot_stat_change")
                if oneshot:
                    log(f"  one_shot_stat_change: {oneshot}")
                    try:
                        changes = oneshot.get_editor_property("stat_changes")
                        log(f"    stat_changes: {changes} (len={len(changes) if changes else 0})")
                    except Exception as e:
                        log(f"    Could not read stat_changes: {e}")
            except Exception as e:
                log(f"  one_shot_stat_change error: {e}")

            # tick_and_one_shot_stat_change
            try:
                combo = asset.get_editor_property("tick_and_one_shot_stat_change")
                if combo:
                    log(f"  tick_and_one_shot_stat_change: {combo}")
                    try:
                        instant = combo.get_editor_property("instant_stat_adjustment")
                        ticking = combo.get_editor_property("ticking_stat_adjustment")
                        duration = combo.get_editor_property("duration")
                        interval = combo.get_editor_property("interval")
                        log(f"    instant_stat_adjustment: {instant} (len={len(instant) if instant else 0})")
                        log(f"    ticking_stat_adjustment: {ticking} (len={len(ticking) if ticking else 0})")
                        log(f"    duration: {duration}")
                        log(f"    interval: {interval}")
                    except Exception as e:
                        log(f"    Could not read nested props: {e}")
            except Exception as e:
                log(f"  tick_and_one_shot_stat_change error: {e}")

        except Exception as e:
            log(f"  ERROR: {e}")

    log(f"\n" + "=" * 60)

# Run
check_statchanges()

# Save log
with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog: {OUTPUT_FILE}")
