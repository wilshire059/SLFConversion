# configure_input_actions.py
# Configure Input Actions and PlayerMappingContext in B_Soulslike_Character

import unreal

def run():
    unreal.log_warning("=" * 60)
    unreal.log_warning("CONFIGURING INPUT ACTIONS")
    unreal.log_warning("=" * 60)

    # Load B_Soulslike_Character
    char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    char_bp = unreal.EditorAssetLibrary.load_asset(char_path)
    if not char_bp:
        unreal.log_error("Could not load B_Soulslike_Character")
        return

    gen_class = char_bp.generated_class()
    if not gen_class:
        unreal.log_error("Could not get generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_error("Could not get CDO")
        return

    # Input action mappings
    input_mappings = {
        "PlayerMappingContext": "/Game/SoulslikeFramework/Input/IMC_Gameplay",
        "IA_Move": "/Game/SoulslikeFramework/Input/Actions/IA_Move",
        "IA_Look": "/Game/SoulslikeFramework/Input/Actions/IA_Look",
        "IA_Jump": "/Game/SoulslikeFramework/Input/Actions/IA_Jump",
        "IA_Sprint": "/Game/SoulslikeFramework/Input/Actions/IA_Sprint_Dodge",
        "IA_Dodge": "/Game/SoulslikeFramework/Input/Actions/IA_Sprint_Dodge",
        "IA_Attack": "/Game/SoulslikeFramework/Input/Actions/IA_RightHandAttack",
        "IA_Guard": "/Game/SoulslikeFramework/Input/Actions/IA_Guard_LeftHandAttack_DualWieldAttack",
        "IA_Interact": "/Game/SoulslikeFramework/Input/Actions/IA_Interact",
        "IA_TargetLock": "/Game/SoulslikeFramework/Input/Actions/IA_TargetLock"
    }

    # Configure each input
    for prop_name, asset_path in input_mappings.items():
        try:
            asset = unreal.EditorAssetLibrary.load_asset(asset_path)
            if asset:
                cdo.set_editor_property(prop_name, asset)
                unreal.log_warning(f"Set {prop_name} = {asset_path}")
            else:
                unreal.log_warning(f"Could not load {asset_path}")
        except Exception as e:
            unreal.log_warning(f"Failed to set {prop_name}: {e}")

    # Save the Blueprint
    try:
        # Mark modified
        char_bp.modify()

        # Compile
        unreal.BlueprintEditorLibrary.compile_blueprint(char_bp)

        # Save
        saved = unreal.EditorAssetLibrary.save_asset(char_path)
        if saved:
            unreal.log_warning("B_Soulslike_Character saved successfully")
        else:
            unreal.log_warning("Failed to save B_Soulslike_Character")

    except Exception as e:
        unreal.log_warning(f"Error saving: {e}")

    unreal.log_warning("=" * 60)
    unreal.log_warning("INPUT CONFIGURATION COMPLETE")
    unreal.log_warning("=" * 60)

run()
