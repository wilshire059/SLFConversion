import unreal

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

automation = unreal.SLFAutomationLibrary

if bp:
    print(f"Blueprint: {bp.get_name()}")

    # Get parent class
    parent = automation.get_blueprint_parent_class(bp)
    print(f"Parent class: {parent}")

    # Check variables
    variables = automation.get_blueprint_variables(bp)
    print(f"Variables: {variables}")

    # Get CDO and check properties
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            print(f"CDO: {cdo.get_name()}")

            # Check sequence_to_play
            try:
                seq = cdo.get_editor_property("sequence_to_play")
                print(f"sequence_to_play: {seq.get_name() if seq else 'None'}")
                print(f"sequence_to_play path: {seq.get_path_name() if seq else 'None'}")
            except Exception as e:
                print(f"Error: {e}")

            # Check can_be_skipped
            try:
                skip = cdo.get_editor_property("can_be_skipped")
                print(f"can_be_skipped: {skip}")
            except Exception as e:
                print(f"Error: {e}")

    # Check compile errors
    errors = automation.get_blueprint_compile_errors(bp)
    print(f"\nCompile errors:\n{errors}")
else:
    print(f"ERROR: Could not load: {bp_path}")
