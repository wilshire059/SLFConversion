"""
Apply CharacterClassName and other data to DA_Quinn and DA_Manny after reparenting.
bp_only values: DA_Quinn = "Quinn", DA_Manny = "Manny"
"""
import unreal

CHARACTER_DATA = {
    "DA_Quinn": {
        "character_class_name": "Quinn",
        "display_name": "Quinn",
    },
    "DA_Manny": {
        "character_class_name": "Manny",
        "display_name": "Manny",
    },
}

for da_name, data in CHARACTER_DATA.items():
    da_path = f"/Game/SoulslikeFramework/Data/BaseCharacters/{da_name}"
    da_asset = unreal.EditorAssetLibrary.load_asset(da_path)
    if not da_asset:
        unreal.log_warning(f"ERROR: Could not load {da_path}")
        continue

    # Set CharacterClassName
    try:
        da_asset.set_editor_property("character_class_name", unreal.Text(data["character_class_name"]))
        unreal.log_warning(f"{da_name}: Set CharacterClassName='{data['character_class_name']}'")
    except Exception as e:
        unreal.log_warning(f"{da_name}: Failed to set CharacterClassName: {e}")

    # Set DisplayName
    try:
        da_asset.set_editor_property("display_name", unreal.Text(data["display_name"]))
        unreal.log_warning(f"{da_name}: Set DisplayName='{data['display_name']}'")
    except Exception as e:
        unreal.log_warning(f"{da_name}: Failed to set DisplayName: {e}")

    # Save
    unreal.EditorAssetLibrary.save_loaded_asset(da_asset, False)
    unreal.log_warning(f"{da_name}: Saved")

    # Verify
    try:
        cn = da_asset.get_editor_property("character_class_name")
        unreal.log_warning(f"{da_name}: VERIFY CharacterClassName='{cn}'")
    except:
        unreal.log_warning(f"{da_name}: VERIFY CharacterClassName FAILED")

unreal.log_warning("Apply complete")
