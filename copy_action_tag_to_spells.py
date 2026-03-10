# copy_action_tag_to_spells.py
# Copy action_to_trigger from DA_MagicSpell to all other spell data assets
#
# Run on SLFConversion:
# UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/copy_action_tag_to_spells.py"

import unreal

# Source - the working magic spell
SOURCE_PATH = "/Game/SoulslikeFramework/Data/Items/DA_MagicSpell.DA_MagicSpell"

# All spell data assets that need the action tag
SPELLS_PATH = "/Game/SoulslikeFramework/Data/Items/Spells/"
SPELL_ASSETS = [
    "DA_FireballSpell",
    "DA_IceShardSpell",
    "DA_LightningBoltSpell",
    "DA_HolyOrbSpell",
    "DA_PoisonBlobSpell",
    "DA_ArcaneMissileSpell",
    "DA_BlackholeSpell",
    "DA_BeamSpell",
    "DA_AOEExplosionSpell",
    "DA_ShieldSpell",
    "DA_HealingSpell",
    "DA_SummoningCircleSpell",
]

LOG_FILE = "C:/scripts/SLFConversion/migration_cache/copy_action_tag_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

def copy_action_tag():
    """Copy action_to_trigger from DA_MagicSpell to all spell data assets."""

    log("=" * 60)
    log("COPYING ACTION TAG FROM DA_MagicSpell")
    log("=" * 60)

    # Load source asset
    source_asset = unreal.EditorAssetLibrary.load_asset(SOURCE_PATH)
    if not source_asset:
        log(f"ERROR: Could not load source asset {SOURCE_PATH}")
        return 0, 1

    # Get action tag from source
    source_info = source_asset.get_editor_property('item_information')
    if not source_info:
        log("ERROR: Source has no item_information")
        return 0, 1

    source_action_tag = source_info.get_editor_property('action_to_trigger')
    if not source_action_tag:
        log("ERROR: Source has no action_to_trigger")
        return 0, 1

    log(f"Source action_to_trigger: {source_action_tag}")
    log(f"Tag type: {type(source_action_tag)}")

    success_count = 0
    error_count = 0

    for spell_name in SPELL_ASSETS:
        asset_path = f"{SPELLS_PATH}{spell_name}.{spell_name}"

        log(f"\nProcessing: {spell_name}")

        # Load the asset
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log(f"  ERROR: Could not load {spell_name}")
            error_count += 1
            continue

        try:
            # Get ItemInformation struct
            item_info = asset.get_editor_property('item_information')
            if item_info is None:
                log(f"  ERROR: {spell_name} - No item_information property")
                error_count += 1
                continue

            # Get current action tag
            current_tag = item_info.get_editor_property('action_to_trigger')
            log(f"  Current action_to_trigger: {current_tag}")

            # Copy the action tag
            item_info.set_editor_property('action_to_trigger', source_action_tag)
            asset.set_editor_property('item_information', item_info)

            # Verify it was set
            verify_info = asset.get_editor_property('item_information')
            verify_tag = verify_info.get_editor_property('action_to_trigger')

            log(f"  Set action_to_trigger: {verify_tag}")

            # Save the asset
            if unreal.EditorAssetLibrary.save_loaded_asset(asset):
                log(f"  SAVED: {asset_path}")
                success_count += 1
            else:
                log(f"  WARNING: save_loaded_asset returned false")
                success_count += 1

        except Exception as e:
            log(f"  ERROR: {spell_name} - {e}")
            import traceback
            log(traceback.format_exc())
            error_count += 1

    log(f"\n" + "=" * 60)
    log(f"Results: {success_count} succeeded, {error_count} failed")
    log("=" * 60)

    return success_count, error_count


def main():
    success, errors = copy_action_tag()

    # Write log file
    with open(LOG_FILE, 'w', encoding='utf-8') as f:
        f.write('\n'.join(log_lines))

    log(f"\nLog saved to: {LOG_FILE}")


if __name__ == "__main__":
    main()
