"""
Diagnose Settings widget compilation errors
"""
import unreal

widgets = [
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry",
]

for path in widgets:
    unreal.log_warning(f"\n=== {path.split('/')[-1]} ===")

    bp = unreal.load_asset(path)
    if not bp:
        unreal.log_warning("  ERROR: Could not load")
        continue

    # Get compile errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    unreal.log_warning(f"Compile errors:\n{errors}")

    # Check function graphs
    funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
    unreal.log_warning(f"Function graphs: {funcs}")

    # Check variables
    vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    unreal.log_warning(f"Variables: {vars}")

    # Check event dispatchers
    dispatchers = unreal.SLFAutomationLibrary.get_blueprint_event_dispatchers(bp)
    unreal.log_warning(f"Event dispatchers: {dispatchers}")
