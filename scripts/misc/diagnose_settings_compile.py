"""
Diagnose Settings widget compilation errors in detail
"""
import unreal

widgets = [
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
]

for path in widgets:
    name = path.split('/')[-1]
    unreal.log_warning(f"\n=== {name} ===")

    bp = unreal.load_asset(path)
    if not bp:
        unreal.log_warning(f"  ERROR: Could not load")
        continue

    # Get detailed compile errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    unreal.log_warning(f"COMPILE ERRORS:\n{errors}")

    # Check parent class
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    unreal.log_warning(f"Parent: {parent}")

    # Check remaining functions
    funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
    unreal.log_warning(f"Functions ({len(funcs)}):")
    for f in funcs[:10]:
        unreal.log_warning(f"  - {f}")

    # Check remaining variables
    vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    unreal.log_warning(f"Variables ({len(vars)}):")
    for v in vars[:10]:
        unreal.log_warning(f"  - {v}")

    # Check event dispatchers
    dispatchers = unreal.SLFAutomationLibrary.get_blueprint_event_dispatchers(bp)
    unreal.log_warning(f"Event Dispatchers ({len(dispatchers)}):")
    for d in dispatchers[:10]:
        unreal.log_warning(f"  - {d}")
