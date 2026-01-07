"""
Add IA_Crouch mapping to IMC_Gameplay Input Mapping Context
"""
import unreal

def log(msg):
    unreal.log(msg)
    print(msg)

def add_crouch_mapping():
    log("="*60)
    log("Adding IA_Crouch to IMC_Gameplay")
    log("="*60)

    # Load the Input Mapping Context
    imc_path = "/Game/SoulslikeFramework/Input/IMC_Gameplay.IMC_Gameplay"
    imc = unreal.load_asset(imc_path)

    if not imc:
        log(f"ERROR: Failed to load IMC_Gameplay from {imc_path}")
        return False

    log(f"Loaded IMC: {imc.get_name()}")

    # Load the IA_Crouch input action
    crouch_action_path = "/Game/SoulslikeFramework/Input/Actions/IA_Crouch.IA_Crouch"
    crouch_action = unreal.load_asset(crouch_action_path)

    if not crouch_action:
        log(f"ERROR: Failed to load IA_Crouch from {crouch_action_path}")
        return False

    log(f"Loaded IA_Crouch: {crouch_action.get_name()}")

    # Use the map_key function on InputMappingContext
    log("Calling map_key with LEFT_CONTROL...")
    try:
        mapping = imc.map_key(crouch_action, unreal.Keys.LEFT_CONTROL)
        log(f"map_key returned: {mapping}")
        log(f"Mapping type: {type(mapping)}")
    except Exception as e:
        log(f"map_key failed: {e}")
        return False

    # Also add C key
    log("Calling map_key with C...")
    try:
        mapping_c = imc.map_key(crouch_action, unreal.Keys.C)
        log(f"map_key for C returned: {mapping_c}")
    except Exception as e:
        log(f"map_key for C failed: {e}")

    # Save the asset
    log("Saving IMC_Gameplay...")
    try:
        saved = unreal.EditorAssetLibrary.save_asset(imc_path, only_if_is_dirty=False)
        log(f"Save result: {saved}")
    except Exception as e:
        log(f"Save error: {e}")

    log("="*60)
    log("DONE - Crouch mappings added")
    log("="*60)

    return True

# Run the script
try:
    add_crouch_mapping()
except Exception as e:
    unreal.log_error(f"Script failed: {e}")
