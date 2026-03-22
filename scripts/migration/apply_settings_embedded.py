"""
Apply embedded settings widget data from bp_only to current project.
Uses C++ SLFAutomationLibrary.ApplyEmbeddedSettingsWidgets.
"""
import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("APPLYING EMBEDDED SETTINGS FROM BP_ONLY TO CURRENT PROJECT")
    unreal.log_warning("=" * 60)

    # Path to the bp_only extracted data
    json_path = "C:/scripts/SLFConversion/migration_cache/settings_embedded_bponly.json"

    unreal.log_warning(f"Source JSON: {json_path}")

    # Call the C++ function to apply the settings
    result = unreal.SLFAutomationLibrary.apply_embedded_settings_widgets(json_path)

    unreal.log_warning(result)
    unreal.log_warning("=" * 60)

main()
