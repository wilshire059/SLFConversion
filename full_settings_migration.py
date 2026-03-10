"""
Full Settings Migration - Single Process
Performs migration and verification in one session to ensure saves persist
"""
import unreal

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/settings_data.json"

unreal.log_warning("=== FULL SETTINGS MIGRATION (SINGLE PROCESS) ===")

# Step 1: Diagnose before
unreal.log_warning("\n--- BEFORE MIGRATION ---")
diag1 = unreal.SLFAutomationLibrary.diagnose_settings_widgets()
unreal.log_warning(diag1)

# Step 2: Run full migration
unreal.log_warning("\n--- RUNNING MIGRATION ---")
migrate_result = unreal.SLFAutomationLibrary.migrate_settings_widgets(CACHE_FILE)
unreal.log_warning(migrate_result)

# Force garbage collection to clear any cached objects
unreal.SystemLibrary.collect_garbage()

# Step 3: Now reload and verify IN THE SAME PROCESS
unreal.log_warning("\n--- RELOADING AND VERIFYING ---")

# Force reimport the assets
paths = [
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry",
    "/Game/SoulslikeFramework/Data/PDA_CustomSettings",
]

all_pass = True
for path in paths:
    name = path.split('/')[-1]

    # Load fresh copy
    bp = unreal.EditorAssetLibrary.load_asset(path)
    if not bp:
        unreal.log_warning(f"  [FAIL] {name} - Could not load")
        all_pass = False
        continue

    # Check if it compiles
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if "Error" in str(errors) or "error" in str(errors):
        unreal.log_warning(f"  [FAIL] {name} - Has compile errors")
        all_pass = False
    else:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
        vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        unreal.log_warning(f"  [PASS] {name}")
        unreal.log_warning(f"    Parent: {parent}")
        unreal.log_warning(f"    Funcs: {len(funcs)}, Vars: {len(vars)}")

# Step 4: Final diagnosis
unreal.log_warning("\n--- AFTER MIGRATION ---")
diag2 = unreal.SLFAutomationLibrary.diagnose_settings_widgets()
unreal.log_warning(diag2)

if all_pass:
    unreal.log_warning("\n=== SUCCESS - ALL WIDGETS COMPILE ===")
else:
    unreal.log_warning("\n=== FAILED - SOME WIDGETS HAVE ERRORS ===")
