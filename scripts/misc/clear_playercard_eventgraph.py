import unreal

bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_PlayerCard"

bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_warning("CLEAR: Could not load: {}".format(bp_path))
else:
    # Check for event graph logic
    has_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(bp)
    unreal.log_warning("CLEAR: W_Settings_PlayerCard has_event_graph_logic: {}".format(has_logic))

    if has_logic:
        result = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
        unreal.log_warning("CLEAR: clear_event_graphs result: {}".format(result))

        result4 = unreal.SLFAutomationLibrary.compile_and_save(bp)
        unreal.log_warning("CLEAR: compile_and_save result: {}".format(result4))

        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        unreal.log_warning("CLEAR: Compile errors: {}".format(errors))
    else:
        unreal.log_warning("CLEAR: No event graph logic to clear")

unreal.log_warning("CLEAR: Done!")
