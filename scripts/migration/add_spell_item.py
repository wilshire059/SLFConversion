# add_spell_item.py
# Creates a magic spell item by duplicating the throwing knife
# The user can then edit ItemClass in the editor

import unreal

def create_spell_data_asset():
    """Create a new spell item data asset based on throwing knife"""

    # Load the throwing knife as a template
    knife_path = "/Game/SoulslikeFramework/Data/Items/DA_ThrowingKnife"
    knife_asset = unreal.EditorAssetLibrary.load_asset(knife_path)

    if not knife_asset:
        unreal.log_error(f"Could not load template: {knife_path}")
        return None

    # Create new asset path
    spell_path = "/Game/SoulslikeFramework/Data/Items/DA_MagicSpell"

    # Check if spell already exists
    if unreal.EditorAssetLibrary.does_asset_exist(spell_path):
        unreal.log(f"Spell asset already exists at {spell_path}")
        return unreal.EditorAssetLibrary.load_asset(spell_path)

    # Duplicate the knife to create spell
    success = unreal.EditorAssetLibrary.duplicate_asset(knife_path, spell_path)
    if not success:
        unreal.log_error(f"Failed to duplicate asset to {spell_path}")
        return None

    # Load the new spell asset
    spell_asset = unreal.EditorAssetLibrary.load_asset(spell_path)
    if not spell_asset:
        unreal.log_error(f"Failed to load created spell asset")
        return None

    unreal.log(f"Created spell asset: {spell_path}")

    # Set basic properties on the spell
    spell_asset.set_editor_property("display_name", unreal.Text("Magic Spell"))
    spell_asset.set_editor_property("description", unreal.Text("A magical spell projectile with C++ generated VFX. Equip as a tool and use to cast."))

    # Get ItemInformation struct and update display name
    try:
        item_info = spell_asset.get_editor_property("item_information")
        item_info.set_editor_property("display_name", unreal.Text("Magic Spell"))
        spell_asset.set_editor_property("item_information", item_info)
    except Exception as e:
        unreal.log_warning(f"Could not update item_information: {e}")

    # Try to set ItemClass to SLFSpellProjectile
    try:
        item_info = spell_asset.get_editor_property("item_information")

        # Load the SLFSpellProjectile class
        spell_class = unreal.load_class(None, "/Script/SLFConversion.SLFSpellProjectile")
        if spell_class:
            item_info.set_editor_property("item_class", spell_class)
            spell_asset.set_editor_property("item_information", item_info)
            unreal.log(f"Set ItemClass to SLFSpellProjectile")
        else:
            unreal.log_warning("Could not load SLFSpellProjectile class - you'll need to set ItemClass manually in the editor")
    except Exception as e:
        unreal.log_warning(f"Could not set ItemClass automatically: {e}")
        unreal.log_warning("Please open DA_MagicSpell in the editor and set ItemInformation.ItemClass to SLFSpellProjectile")

    # Save the asset
    unreal.EditorAssetLibrary.save_asset(spell_path)

    return spell_asset


def main():
    """Main entry point"""
    unreal.log("=== Creating Magic Spell Item ===")

    # Create the spell data asset
    spell_asset = create_spell_data_asset()

    if spell_asset:
        unreal.log("SUCCESS: Spell data asset created!")
        unreal.log("  Path: /Game/SoulslikeFramework/Data/Items/DA_MagicSpell")
        unreal.log("")
        unreal.log("NEXT STEPS:")
        unreal.log("  1. Open the editor")
        unreal.log("  2. Find DA_MagicSpell in Content/SoulslikeFramework/Data/Items/")
        unreal.log("  3. Set ItemInformation -> ItemClass to SLFSpellProjectile")
        unreal.log("  4. Add DA_MagicSpell to your Tools inventory slot")
        unreal.log("  5. Use the tool to cast the spell!")
    else:
        unreal.log_error("Failed to create spell asset")


if __name__ == "__main__":
    main()
