"""
Extract embedded settings from both bp_only and current W_Settings,
then compare and apply the bp_only values.

This uses the C++ SLFAutomationLibrary functions to properly access WidgetTree.
"""
import unreal
import os

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/"

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("EXTRACTING AND COMPARING SETTINGS WIDGETS")
    unreal.log_warning("=" * 60)

    # Ensure output directory exists
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    # Extract from current W_Settings
    unreal.log_warning("\n=== Extracting from CURRENT W_Settings ===")
    current_output = OUTPUT_DIR + "settings_embedded_current.json"
    current_result = unreal.SLFAutomationLibrary.extract_embedded_settings_widgets(
        current_output,
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
    )
    unreal.log_warning(current_result)

    # Extract from bp_only W_Settings (copied to Temp)
    unreal.log_warning("\n=== Extracting from BP_ONLY W_Settings ===")
    bponly_output = OUTPUT_DIR + "settings_embedded_bponly.json"
    bponly_result = unreal.SLFAutomationLibrary.extract_embedded_settings_widgets(
        bponly_output,
        "/Game/Temp/W_Settings_BPOnly"
    )
    unreal.log_warning(bponly_result)

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning("Extraction complete!")
    unreal.log_warning(f"Current settings: {current_output}")
    unreal.log_warning(f"BP_Only settings: {bponly_output}")
    unreal.log_warning("=" * 60)

main()
