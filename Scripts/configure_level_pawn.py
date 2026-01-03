"""
Configure GM_SoulslikeFramework to use B_Soulslike_Character as DefaultPawnClass.

This script:
1. Loads the existing GM_SoulslikeFramework GameMode Blueprint
2. Sets its DefaultPawnClass to B_Soulslike_Character
3. Saves the GameMode Blueprint
"""

import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("CONFIGURING GAMEMODE PAWN CLASS")
    unreal.log_warning("=" * 60)

    # Load B_Soulslike_Character Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    bp_asset = unreal.load_asset(bp_path)

    if not bp_asset:
        unreal.log_error("Failed to load B_Soulslike_Character!")
        return False

    # Get the generated class from the Blueprint
    bp_class = bp_asset.generated_class()
    if not bp_class:
        unreal.log_error("B_Soulslike_Character has no generated class!")
        return False

    unreal.log_warning("B_Soulslike_Character class: " + str(bp_class.get_name()))

    # Load the existing GM_SoulslikeFramework GameMode Blueprint
    gm_bp_path = "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework"
    gm_bp = unreal.load_asset(gm_bp_path)

    if not gm_bp:
        unreal.log_error("Failed to load GM_SoulslikeFramework!")
        return False

    unreal.log_warning("Found GM_SoulslikeFramework Blueprint")

    # Get its generated class
    gm_class = gm_bp.generated_class()
    if not gm_class:
        unreal.log_error("GM_SoulslikeFramework has no generated class!")
        return False

    unreal.log_warning("GameMode class: " + str(gm_class.get_name()))

    # Get the CDO (Class Default Object)
    cdo = unreal.get_default_object(gm_class)
    if not cdo:
        unreal.log_error("Failed to get CDO for GM_SoulslikeFramework!")
        return False

    # Check current DefaultPawnClass
    try:
        current_pawn = cdo.get_editor_property("default_pawn_class")
        if current_pawn:
            unreal.log_warning("Current DefaultPawnClass: " + str(current_pawn.get_name()))
        else:
            unreal.log_warning("Current DefaultPawnClass: None")
    except:
        unreal.log_warning("Could not read current DefaultPawnClass")

    # Set the DefaultPawnClass to B_Soulslike_Character
    try:
        cdo.set_editor_property("default_pawn_class", bp_class)
        unreal.log_warning("Set DefaultPawnClass to: " + str(bp_class.get_name()))
    except Exception as e:
        unreal.log_error("Failed to set DefaultPawnClass: " + str(e))
        return False

    # Mark the Blueprint as modified and save
    try:
        # Save the Blueprint asset
        unreal.EditorAssetLibrary.save_asset(gm_bp_path, only_if_is_dirty=False)
        unreal.log_warning("Saved GM_SoulslikeFramework Blueprint")
    except Exception as e:
        unreal.log_error("Failed to save GM_SoulslikeFramework: " + str(e))
        return False

    # Verify the change
    try:
        verify_pawn = cdo.get_editor_property("default_pawn_class")
        if verify_pawn:
            unreal.log_warning("Verified DefaultPawnClass: " + str(verify_pawn.get_name()))
        else:
            unreal.log_warning("Verification: DefaultPawnClass is None")
    except:
        unreal.log_warning("Could not verify DefaultPawnClass")

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("CONFIGURATION COMPLETE")
    unreal.log_warning("=" * 60)

    return True

if __name__ == "__main__":
    main()
