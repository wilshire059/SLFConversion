import unreal

# Reparent W_GenericButton to UW_GenericButton C++ class
# More aggressive: remove all Blueprint variables and logic since C++ parent has everything

bp_path = "/Game/SoulslikeFramework/Widgets/_Generic/W_GenericButton"
new_parent_path = "/Script/SLFConversion.W_GenericButton"

bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_warning("REPARENT: Could not load: {}".format(bp_path))
else:
    # Check current parent
    gen = bp.generated_class()
    if gen:
        cdo = unreal.get_default_object(gen)
        unreal.log_warning("REPARENT: BEFORE - CDO type: {}".format(type(cdo).__name__))

    # Load new parent class
    new_parent = unreal.load_class(None, new_parent_path)
    if not new_parent:
        unreal.log_warning("REPARENT: Could not load new parent: {}".format(new_parent_path))
    else:
        unreal.log_warning("REPARENT: New parent class: {}".format(new_parent))

        # Step 1: Remove all Blueprint variables FIRST (C++ parent now owns them all)
        remove_vars = unreal.SLFAutomationLibrary.remove_all_variables(bp)
        unreal.log_warning("REPARENT: remove_all_variables result: {}".format(remove_vars))

        # Step 2: Clear ALL Blueprint logic (events, functions, macros)
        clear_all = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
        unreal.log_warning("REPARENT: clear_all_blueprint_logic result: {}".format(clear_all))

        # Step 3: Reparent
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent)
        unreal.log_warning("REPARENT: reparent_blueprint result: {}".format(result))

        # Step 4: Compile and save
        compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        unreal.log_warning("REPARENT: compile_and_save result: {}".format(compile_result))

        # Check compile errors
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        unreal.log_warning("REPARENT: Compile errors: {}".format(errors))

        # Verify CDO type
        gen2 = bp.generated_class()
        if gen2:
            cdo2 = unreal.get_default_object(gen2)
            unreal.log_warning("REPARENT: AFTER - CDO type: {}".format(type(cdo2).__name__))

unreal.log_warning("REPARENT: Done!")
