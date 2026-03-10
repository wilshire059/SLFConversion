"""
Verify Settings widgets compile without errors
"""
import unreal

widgets = [
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry",
    "/Game/SoulslikeFramework/Data/PDA_CustomSettings",
]

all_pass = True
for path in widgets:
    name = path.split('/')[-1]
    bp = unreal.load_asset(path)
    if not bp:
        unreal.log_warning(f"[FAIL] {name} - Could not load")
        all_pass = False
        continue

    # Check compile status
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if "Error" in errors or "error" in errors:
        unreal.log_warning(f"[FAIL] {name} - Has compile errors")
        unreal.log_warning(errors[:500])  # First 500 chars
        all_pass = False
    else:
        # Check parent class
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        unreal.log_warning(f"[PASS] {name}")
        unreal.log_warning(f"  Parent: {parent}")

        # Check remaining functions/variables
        funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
        vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        dispatchers = unreal.SLFAutomationLibrary.get_blueprint_event_dispatchers(bp)
        unreal.log_warning(f"  Remaining: {len(funcs)} funcs, {len(vars)} vars, {len(dispatchers)} dispatchers")

if all_pass:
    unreal.log_warning("\n=== ALL WIDGETS COMPILE SUCCESSFULLY ===")
else:
    unreal.log_warning("\n=== SOME WIDGETS HAVE ERRORS ===")
