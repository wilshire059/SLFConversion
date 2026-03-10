# check_dialog_input.py
# Check what inputs are mapped for dialog

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(str(msg))

log("=== Checking Dialog Input Mappings ===")

# Check IMC_Dialog
imc_dialog_path = "/Game/SoulslikeFramework/Input/IMC_Dialog"
imc_dialog = unreal.EditorAssetLibrary.load_asset(imc_dialog_path)

if imc_dialog:
    log(f"Loaded IMC_Dialog: {imc_dialog.get_class().get_name()}")
    try:
        mappings = imc_dialog.get_editor_property('mappings')
        log(f"IMC_Dialog has {len(mappings)} mappings")
        for i, mapping in enumerate(mappings):
            action = mapping.get_editor_property('action')
            action_name = action.get_name() if action else "None"
            log(f"  {i}: Action={action_name}")
            # Get triggers/modifiers if available
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
        mappings = imc_nav.get_editor_property('mappings')
        log(f"IMC_NavigableMenu has {len(mappings)} mappings")
        for i, mapping in enumerate(mappings):
            action = mapping.get_editor_property('action')
            action_name = action.get_name() if action else "None"
            log(f"  {i}: Action={action_name}")
    except Exception as e:
        log(f"Error reading IMC_NavigableMenu: {e}")
else:
    log(f"Failed to load IMC_NavigableMenu")

# Check IA_NavigableMenu_Ok input action
ia_ok_path = "/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Ok"
ia_ok = unreal.EditorAssetLibrary.load_asset(ia_ok_path)

if ia_ok:
    log(f"\nLoaded IA_NavigableMenu_Ok: {ia_ok.get_class().get_name()}")
    log(f"Export: {ia_ok.export_text()[:500]}")
else:
    log(f"Failed to load IA_NavigableMenu_Ok")

log("\n=== Done ===")
