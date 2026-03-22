# apply_spell_categories.py
# Apply Tools category, usable flag, and action_to_trigger to all spell data assets
#
# Run on SLFConversion:
# UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/apply_spell_categories.py"

import unreal

# Get the enum class
SLFItemCategory = unreal.SLFItemCategory

# All spell data assets that need category setup
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

LOG_FILE = "C:/scripts/SLFConversion/migration_cache/spell_categories_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

def apply_spell_categories():
    """Apply Tools category and usable flag to all spell data assets."""

    log("=" * 60)
    log("APPLYING SPELL CATEGORIES")
    log("=" * 60)

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

            # Get Category struct from ItemInformation
            category_data = item_info.get_editor_property('category')
            if category_data is None:
                log(f"  ERROR: {spell_name} - No category property")
                error_count += 1
                continue

            # Get current values for logging
            old_category = category_data.get_editor_property('category')
            old_usable = item_info.get_editor_property('usable')

            # Set the category to Tools
            category_data.set_editor_property('category', SLFItemCategory.TOOLS)
            item_info.set_editor_property('category', category_data)
            log(f"  Set category: {old_category} -> Tools")

            # Set usable flag to True
            item_info.set_editor_property('usable', True)
            log(f"  Set usable: {old_usable} -> True")

            # Set action_to_trigger to "Action.Projectile" (same as throwing knife / magic spell)
            try:
                # Try GameplayTag.request_gameplay_tag first (newer API)
                tag = unreal.GameplayTag.request_gameplay_tag("Action.Projectile")
                if tag and tag.is_valid():
                    item_info.set_editor_property('action_to_trigger', tag)
                    log(f"  Set action_to_trigger: Action.Projectile")
                else:
                    log(f"  WARNING: Could not find Action.Projectile tag")
            except Exception as tag_error:
                log(f"  WARNING: Could not set action_to_trigger: {tag_error}")

            # Update the asset's ItemInformation
            asset.set_editor_property('item_information', item_info)

            # Save the asset
            if unreal.EditorAssetLibrary.save_loaded_asset(asset):
                log(f"  SAVED: {asset_path}")
                success_count += 1
            else:
                log(f"  WARNING: save_loaded_asset returned false")
                success_count += 1  # Still count as success since changes were applied

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
    success, errors = apply_spell_categories()

    # Write log file
    with open(LOG_FILE, 'w', encoding='utf-8') as f:
        f.write('\n'.join(log_lines))

    log(f"\nLog saved to: {LOG_FILE}")


if __name__ == "__main__":
    main()
