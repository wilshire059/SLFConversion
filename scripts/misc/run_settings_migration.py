"""
Full Settings Widget Migration using C++ Automation

CRITICAL: Process in DEPENDENCY ORDER:
1. Clear W_Settings_Entry FIRST (leaf - no dependencies)
2. Clear W_Settings_CategoryEntry (leaf)
3. Clear PDA_CustomSettings (leaf)
4. Clear W_Settings LAST (depends on Entry and CategoryEntry)

This prevents cascade compile errors from loading dependent Blueprints.
"""
import unreal
import sys

# Determine which step to run based on project
project_path = unreal.Paths.get_project_file_path()
is_bp_only = "bp_only" in project_path.lower()

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/settings_data.json"

if is_bp_only:
    # STEP 1: Extract from bp_only
    unreal.log_warning("=== SETTINGS EXTRACTION (bp_only) ===")
    result = unreal.SLFAutomationLibrary.extract_settings_widget_data(CACHE_FILE)
    unreal.log_warning(result)
    unreal.log_warning("=== EXTRACTION COMPLETE ===")
else:
    # STEP 2: Migrate on SLFConversion - SKIP DIAGNOSIS (triggers compile errors)
    unreal.log_warning("=== SETTINGS MIGRATION (SLFConversion) ===")
    unreal.log_warning("Skipping diagnosis to avoid cascade compile errors")

    # Run full migration
    unreal.log_warning("\n--- RUNNING FULL MIGRATION ---")
    migrate_result = unreal.SLFAutomationLibrary.migrate_settings_widgets(CACHE_FILE)
    unreal.log_warning(migrate_result)

    unreal.log_warning("=== MIGRATION COMPLETE ===")
