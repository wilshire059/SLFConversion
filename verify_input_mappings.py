# verify_input_mappings.py
# Verify input mapping contexts have proper gamepad mappings

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(str(msg))

log("=== Verifying Input Mappings ===")

# Check IMC_Dialog
imc_dialog_path = "/Game/SoulslikeFramework/Input/IMC_Dialog"
imc_dialog = unreal.EditorAssetLibrary.load_asset(imc_dialog_path)

if imc_dialog:
    log(f"IMC_Dialog loaded: {imc_dialog.get_class().get_name()}")

    # Try to list all properties to find the correct one
    log("Attempting to access mappings via different property names...")

    for prop_name in ['default_key_mappings', 'mappings', 'context_mappings', 'key_mappings']:
        try:
            mappings = imc_dialog.get_editor_property(prop_name)
            if mappings is not None:
                log(f"  {prop_name}: Found! Length = {len(mappings) if hasattr(mappings, '__len__') else 'N/A'}")
                if hasattr(mappings, '__len__') and len(mappings) > 0:
                    for i, m in enumerate(mappings[:5]):
                        log(f"    [{i}] {m}")
            else:
                log(f"  {prop_name}: None")
        except Exception as e:
            log(f"  {prop_name}: Error - {e}")

    # Try export_text to see the full structure
    log("\nExporting IMC_Dialog structure...")
    try:
        export = imc_dialog.export_text()
        # Find Mappings section
        if "Mappings" in export:
            log("Found 'Mappings' in export text")
            # Find lines containing key mappings
            lines = export.split('\n')
            for line in lines:
                if 'Key=' in line or 'Action=' in line or 'Gamepad' in line:
                    log(f"  {line.strip()[:100]}")
        else:
            log("'Mappings' not found in export text")
            log(f"Export preview: {export[:500]}")
    except Exception as e:
        log(f"Error exporting: {e}")
else:
    log("Failed to load IMC_Dialog")

# Check IMC_NavigableMenu
imc_nav_path = "/Game/SoulslikeFramework/Input/IMC_NavigableMenu"
imc_nav = unreal.EditorAssetLibrary.load_asset(imc_nav_path)

if imc_nav:
    log(f"\nIMC_NavigableMenu loaded: {imc_nav.get_class().get_name()}")

    for prop_name in ['default_key_mappings', 'mappings', 'context_mappings', 'key_mappings']:
        try:
            mappings = imc_nav.get_editor_property(prop_name)
            if mappings is not None:
                log(f"  {prop_name}: Found! Length = {len(mappings) if hasattr(mappings, '__len__') else 'N/A'}")
            else:
                log(f"  {prop_name}: None")
        except Exception as e:
            log(f"  {prop_name}: Error - {e}")

    # Try export_text
    log("\nExporting IMC_NavigableMenu structure...")
    try:
        export = imc_nav.export_text()
        if "Mappings" in export:
            log("Found 'Mappings' in export text")
            lines = export.split('\n')
            count = 0
            for line in lines:
                if 'Key=' in line or 'Action=' in line or 'Gamepad' in line:
                    log(f"  {line.strip()[:100]}")
                    count += 1
                    if count > 15:
                        log("  ... (truncated)")
                        break
        else:
            log("'Mappings' not found in export text")
    except Exception as e:
        log(f"Error exporting: {e}")
else:
    log("Failed to load IMC_NavigableMenu")

log("\n=== Done ===")
