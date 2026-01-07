"""
Fix IA_Crouch key mapping in IMC_Gameplay
First clears existing mappings, then adds C and LeftControl
"""
import unreal

log_file = "C:/scripts/SLFConversion/crouch_fix_log.txt"

def log_to_file(msg):
    with open(log_file, "a") as f:
        f.write(str(msg) + "\n")
    unreal.log_warning(str(msg))

def fix_crouch_mapping():
    # Clear previous log
    with open(log_file, "w") as f:
        f.write("=== CROUCH MAPPING FIX LOG ===\n")

    log_to_file("Starting crouch mapping fix...")

    # Load the Input Mapping Context
    imc_path = "/Game/SoulslikeFramework/Input/IMC_Gameplay.IMC_Gameplay"
    imc = unreal.load_asset(imc_path)

    if not imc:
        log_to_file(f"FAILED: Could not load IMC_Gameplay")
        return False

    log_to_file(f"Loaded IMC: {imc.get_name()}")

    # Load the IA_Crouch input action
    crouch_path = "/Game/SoulslikeFramework/Input/Actions/IA_Crouch.IA_Crouch"
    crouch_action = unreal.load_asset(crouch_path)

    if not crouch_action:
        log_to_file(f"FAILED: Could not load IA_Crouch")
        return False

    log_to_file(f"Loaded IA_Crouch: {crouch_action.get_name()}")

    # First, unmap any existing crouch mappings
    try:
        log_to_file("Clearing existing crouch mappings...")
        imc.unmap_action(crouch_action)
        log_to_file("Cleared existing mappings")
    except Exception as e:
        log_to_file(f"unmap_action error (may be OK): {e}")

    # Now add fresh mappings
    added_mappings = []

    # Add C key mapping
    try:
        key_c = unreal.Key()
        key_c.import_text("C")
        result_c = imc.map_key(crouch_action, key_c)
        log_to_file(f"Added C key")
        added_mappings.append(("C", result_c))
    except Exception as e:
        log_to_file(f"Failed to add C key: {e}")

    # Add LeftControl key mapping
    try:
        key_ctrl = unreal.Key()
        key_ctrl.import_text("LeftControl")
        result_ctrl = imc.map_key(crouch_action, key_ctrl)
        log_to_file(f"Added LeftControl key")
        added_mappings.append(("LeftControl", result_ctrl))
    except Exception as e:
        log_to_file(f"Failed to add LeftControl key: {e}")

    # Verify by re-reading the mappings
    log_to_file("Verifying mappings...")
    mappings = imc.get_editor_property('mappings')
    log_to_file(f"Total mappings: {len(mappings)}")
    for i, m in enumerate(mappings):
        action = m.get_editor_property('action')
        key = m.get_editor_property('key')
        action_name = action.get_name() if action else "None"
        key_name = key.export_text() if key else "None"
        log_to_file(f"  [{i}] {action_name} -> {key_name}")

    # Try multiple save approaches
    log_to_file("Attempting save...")

    # Approach 1: Mark dirty and save
    try:
        imc.modify()
        log_to_file("Marked asset dirty")
    except Exception as e:
        log_to_file(f"modify() not available: {e}")

    # Approach 2: Try checkout first (for source control)
    try:
        unreal.EditorAssetLibrary.checkout_asset(imc_path)
        log_to_file("Checked out asset")
    except Exception as e:
        log_to_file(f"checkout failed (OK if no source control): {e}")

    # Approach 3: Force save
    result = unreal.EditorAssetLibrary.save_asset(imc_path, only_if_is_dirty=False)
    log_to_file(f"Save result: {result}")

    if not result:
        # Approach 4: Save loaded packages
        try:
            unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
            log_to_file("Saved dirty packages")
        except Exception as e:
            log_to_file(f"save_dirty_packages failed: {e}")

    log_to_file("Script complete")
    log_to_file("Test with C or Left Ctrl key in PIE")

    return True

# Run immediately
try:
    fix_crouch_mapping()
except Exception as e:
    with open(log_file, "a") as f:
        f.write(f"Script exception: {e}\n")
        import traceback
        f.write(traceback.format_exc())
    unreal.log_error(f"Script failed: {e}")
