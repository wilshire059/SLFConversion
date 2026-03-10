import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

automation = unreal.SLFAutomationLibrary

if bp:
    log(f"Blueprint: {bp.get_name()}")

    # Get parent class
    parent = automation.get_blueprint_parent_class(bp)
    log(f"Parent class: {parent}")

    # Check variables
    variables = automation.get_blueprint_variables(bp)
    log(f"Variables: {variables}")

    # Get CDO and check properties
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            log(f"CDO: {cdo.get_name()}")

            # Check sequence_to_play
            try:
                seq = cdo.get_editor_property("sequence_to_play")
                log(f"sequence_to_play: {seq.get_name() if seq else 'None'}")
                log(f"sequence_to_play path: {seq.get_path_name() if seq else 'None'}")
            except Exception as e:
                log(f"Error sequence_to_play: {e}")

            # Check can_be_skipped
            try:
                skip = cdo.get_editor_property("can_be_skipped")
                log(f"can_be_skipped: {skip}")
            except Exception as e:
                log(f"Error can_be_skipped: {e}")

    # Check compile errors
    errors = automation.get_blueprint_compile_errors(bp)
    log(f"\nCompile status:\n{errors}")
else:
    log(f"ERROR: Could not load: {bp_path}")

# Write output to file
with open("C:/scripts/SLFConversion/migration_cache/sequence_actor_check.txt", 'w') as f:
    f.write('\n'.join(output))
