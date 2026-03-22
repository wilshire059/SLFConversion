# check_backup_input.py
# Check what inputs are in the backup project

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(str(msg))

log("=== Checking Backup Input Mappings ===")

# Check IMC_Dialog
imc_dialog_path = "/Game/SoulslikeFramework/Input/IMC_Dialog"
imc_dialog = unreal.EditorAssetLibrary.load_asset(imc_dialog_path)

if imc_dialog:
    log(f"Loaded IMC_Dialog: {imc_dialog.get_class().get_name()}")
    try:
        # Use default_key_mappings instead of deprecated mappings
        mappings = imc_dialog.get_editor_property('default_key_mappings')
        if mappings:
            log(f"IMC_Dialog has {len(mappings)} default_key_mappings")
            for i, mapping in enumerate(mappings[:10]):  # First 10 only
                log(f"  {i}: {mapping}")
        else:
            log("IMC_Dialog default_key_mappings is None or empty")
    except Exception as e:
        log(f"Error reading IMC_Dialog: {e}")
else:
    log(f"Failed to load IMC_Dialog")

# Check IMC_NavigableMenu
imc_nav_path = "/Game/SoulslikeFramework/Input/IMC_NavigableMenu"
imc_nav = unreal.EditorAssetLibrary.load_asset(imc_nav_path)

if imc_nav:
    log(f"\nLoaded IMC_NavigableMenu: {imc_nav.get_class().get_name()}")
    try:
        mappings = imc_nav.get_editor_property('default_key_mappings')
        if mappings:
            log(f"IMC_NavigableMenu has {len(mappings)} default_key_mappings")
            for i, mapping in enumerate(mappings[:10]):
                log(f"  {i}: {mapping}")
        else:
            log("IMC_NavigableMenu default_key_mappings is None or empty")
    except Exception as e:
        log(f"Error reading IMC_NavigableMenu: {e}")
else:
    log(f"Failed to load IMC_NavigableMenu")

log("\n=== Done ===")
