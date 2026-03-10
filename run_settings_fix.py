"""
Run settings fix and validation:
1. Apply embedded settings widget data from bp_only cache
2. Run diagnostic to verify the data was applied
"""
import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("FIXING AND VALIDATING SETTINGS WIDGETS")
    unreal.log_warning("=" * 60)

    # Step 1: Apply embedded settings data
    unreal.log_warning("\n--- Step 1: Applying embedded settings data ---")
    json_path = "C:/scripts/SLFConversion/migration_cache/settings_embedded_bponly.json"
    result = unreal.SLFAutomationLibrary.apply_embedded_settings_widgets(json_path)
    unreal.log_warning(result)

    # Step 2: Run diagnostic
    unreal.log_warning("\n--- Step 2: Running diagnostic ---")
    diag_result = unreal.SLFAutomationLibrary.diagnose_settings_widgets()
    unreal.log_warning(diag_result)

    # Step 3: Extract current embedded state for verification
    unreal.log_warning("\n--- Step 3: Extracting current embedded state ---")
    current_json_path = "C:/scripts/SLFConversion/migration_cache/settings_embedded_current.json"
    extract_result = unreal.SLFAutomationLibrary.extract_embedded_settings_widgets(current_json_path)
    unreal.log_warning(extract_result)

    unreal.log_warning("=" * 60)
    unreal.log_warning("FIX AND VALIDATION COMPLETE")
    unreal.log_warning("=" * 60)
    unreal.log_warning("Check migration_cache/settings_embedded_current.json to compare with bp_only version")

main()
