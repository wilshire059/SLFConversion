"""
Fix TriggeredText on status effect data assets.
DA_StatusEffect_Poison has wrong text "DECAYED" instead of "POISONED".
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/fix_statuseffect_triggeredtext_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

# Only Poison needs fixing (others already have correct text)
STATUS_EFFECT_TEXTS = {
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison": "POISONED",
}

def fix_triggered_text():
    log("=" * 60)
    log("Fixing Status Effect TriggeredText")
    log("=" * 60)

    success_count = 0
    fail_count = 0

    for asset_path, triggered_text in STATUS_EFFECT_TEXTS.items():
        log(f"\nProcessing: {asset_path}")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log(f"  ERROR: Could not load asset")
            fail_count += 1
            continue

        log(f"  Class: {asset.get_class().get_name()}")

        try:
            # Get current triggered text
            current_text = asset.get_editor_property("triggered_text")
            log(f"  Current TriggeredText: {current_text}")

            # Set new triggered text - just pass string directly for FText
            asset.set_editor_property("triggered_text", triggered_text)
            log(f"  Set TriggeredText to: {triggered_text}")

            # Verify
            verify_text = asset.get_editor_property("triggered_text")
            log(f"  Verified TriggeredText: {verify_text}")

            # Save
            if unreal.EditorAssetLibrary.save_asset(asset_path):
                log(f"  SAVED!")
                success_count += 1
            else:
                log(f"  ERROR: Save failed")
                fail_count += 1

        except Exception as e:
            log(f"  ERROR: {e}")
            fail_count += 1

    log(f"\n" + "=" * 60)
    log(f"Results: {success_count} succeeded, {fail_count} failed")
    log("=" * 60)

# Run
fix_triggered_text()

# Save log
with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog: {OUTPUT_FILE}")
