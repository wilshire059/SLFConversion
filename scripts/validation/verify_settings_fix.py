"""
Verify that settings embedded data was applied correctly.
Re-extract and compare to bp_only data.
"""
import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("VERIFYING SETTINGS FIX - RE-EXTRACTING CURRENT PROJECT")
    unreal.log_warning("=" * 60)

    # Re-extract current project settings
    output_path = "C:/scripts/SLFConversion/migration_cache/settings_embedded_current_after_fix.json"

    result = unreal.SLFAutomationLibrary.extract_embedded_settings_widgets(
        output_path,
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
    )

    unreal.log_warning(result)
    unreal.log_warning("=" * 60)

main()
