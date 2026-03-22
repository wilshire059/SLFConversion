import unreal

bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_QuitConfirmation"

bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_warning("CLEAR: Could not load: {}".format(bp_path))
else:
    # Clear event graphs (removes all EventGraph logic)
    try:
        result = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
        unreal.log_warning("CLEAR: clear_event_graphs result: {}".format(result))
    except Exception as e:
        unreal.log_warning("CLEAR: clear_event_graphs failed: {}".format(str(e)))

    # Clear widget delegate bindings (removes BndEvt bound event nodes)
    try:
        result2 = unreal.SLFAutomationLibrary.clear_widget_delegate_bindings(bp)
        unreal.log_warning("CLEAR: clear_widget_delegate_bindings result: {}".format(result2))
    except Exception as e:
        unreal.log_warning("CLEAR: clear_widget_delegate_bindings failed: {}".format(str(e)))

    # Also clear function graphs (bound event functions)
    try:
        result3 = unreal.SLFAutomationLibrary.clear_function_graphs(bp)
        unreal.log_warning("CLEAR: clear_function_graphs result: {}".format(result3))
    except Exception as e:
        unreal.log_warning("CLEAR: clear_function_graphs failed: {}".format(str(e)))

    # Compile and save
    try:
        result4 = unreal.SLFAutomationLibrary.compile_and_save(bp)
        unreal.log_warning("CLEAR: compile_and_save result: {}".format(result4))
    except Exception as e:
        unreal.log_warning("CLEAR: compile_and_save failed: {}".format(str(e)))

    # Check for compile errors
    try:
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        unreal.log_warning("CLEAR: Compile errors: {}".format(errors))
    except Exception as e:
        unreal.log_warning("CLEAR: get_blueprint_compile_errors failed: {}".format(str(e)))

unreal.log_warning("CLEAR: Done!")
