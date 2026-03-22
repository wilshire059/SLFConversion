# fix_spell_class.py
# Updates DA_MagicSpell to use SLFSpellProjectile class

import unreal

LOG_FILE = "C:/scripts/SLFConversion/fix_spell_log.txt"

def log(msg):
    """Write to file since print() doesn't show in commandlet"""
    with open(LOG_FILE, "a") as f:
        f.write(msg + "\n")
    unreal.log(msg)

def fix_spell_item_class():
    """Update the spell item to use the correct projectile class"""

    # Clear log file
    with open(LOG_FILE, "w") as f:
        f.write("=== Fixing Magic Spell ItemClass ===\n")

    spell_path = "/Game/SoulslikeFramework/Data/Items/DA_MagicSpell"

    # Load the spell asset
    spell_asset = unreal.EditorAssetLibrary.load_asset(spell_path)
    if not spell_asset:
        log(f"ERROR: Could not load spell asset at {spell_path}")
        return False

    log(f"Loaded spell asset: {spell_asset.get_name()}")

    # Load the SLFSpellProjectile class
    spell_class = unreal.load_class(None, "/Script/SLFConversion.SLFSpellProjectile")
    if not spell_class:
        log("ERROR: Could not load SLFSpellProjectile class!")
        return False

    log(f"Loaded SLFSpellProjectile class: {spell_class}")

    # Get ItemInformation struct
    try:
        item_info = spell_asset.get_editor_property("item_information")
        log(f"Got item_information struct")

        # Check current ItemClass
        current_class = item_info.get_editor_property("item_class")
        log(f"Current ItemClass: {current_class}")

        # Set the new ItemClass
        item_info.set_editor_property("item_class", spell_class)
        spell_asset.set_editor_property("item_information", item_info)

        log(f"Set ItemClass to SLFSpellProjectile")

        # Verify
        item_info2 = spell_asset.get_editor_property("item_information")
        new_class = item_info2.get_editor_property("item_class")
        log(f"Verified new ItemClass: {new_class}")

    except Exception as e:
        log(f"ERROR setting ItemClass: {e}")
        import traceback
        with open(LOG_FILE, "a") as f:
            traceback.print_exc(file=f)
        return False

    # Save the asset
    success = unreal.EditorAssetLibrary.save_asset(spell_path)
    if success:
        log(f"SAVED: {spell_path}")
    else:
        log(f"WARNING: Could not save {spell_path}")

    return True


def main():
    if fix_spell_item_class():
        log("SUCCESS: DA_MagicSpell now uses SLFSpellProjectile")
    else:
        log("FAILED to update DA_MagicSpell")


if __name__ == "__main__":
    main()
