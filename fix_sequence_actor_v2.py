import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

log("=" * 70)
log("FIXING B_SequenceActor - USING C++ AUTOMATION")
log("=" * 70)

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
new_parent = "/Script/SLFConversion.B_SequenceActor"
sequence_path = "/Game/SoulslikeFramework/Cinematics/LS_ShowcaseRoom"

# Get the automation library
automation = unreal.SLFAutomationLibrary

# Load assets
bp = unreal.EditorAssetLibrary.load_asset(bp_path)
sequence = unreal.EditorAssetLibrary.load_asset(sequence_path)

if not bp:
    log(f"ERROR: Could not load Blueprint: {bp_path}")
elif not sequence:
    log(f"ERROR: Could not load sequence: {sequence_path}")
else:
    log(f"Loaded Blueprint: {bp.get_name()}")
    log(f"Loaded Sequence: {sequence.get_name()}")

    # Step 1: Get current state
    log("")
    log("Step 1: Checking current Blueprint state...")
    parent = automation.get_blueprint_parent_class(bp)
    log(f"  Current parent: {parent}")

    variables = automation.get_blueprint_variables(bp)
    log(f"  Variables ({len(variables)}): {variables}")

    dispatchers = automation.get_blueprint_event_dispatchers(bp)
    log(f"  Dispatchers ({len(dispatchers)}): {dispatchers}")

    # Step 2: Clear all Blueprint logic (removes variables, functions, graphs)
    log("")
    log("Step 2: Clearing all Blueprint logic...")
    try:
        result = automation.clear_all_blueprint_logic(bp)
        log(f"  clear_all_blueprint_logic result: {result}")
    except Exception as e:
        log(f"  ERROR: {e}")

    # Step 3: Reparent to C++ class
    log("")
    log("Step 3: Reparenting to C++ class...")
    try:
        result = automation.reparent_blueprint(bp, new_parent)
        log(f"  reparent_blueprint result: {result}")
    except Exception as e:
        log(f"  ERROR: {e}")

    # Step 4: Compile and save
    log("")
    log("Step 4: Compile and save...")
    try:
        result = automation.compile_and_save(bp)
        log(f"  compile_and_save result: {result}")
    except Exception as e:
        log(f"  ERROR: {e}")

    # Step 5: Reload and set SequenceToPlay property
    log("")
    log("Step 5: Setting SequenceToPlay property...")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                log(f"  Got CDO: {cdo.get_name()}")
                try:
                    cdo.set_editor_property("sequence_to_play", sequence)
                    log(f"  SUCCESS: Set sequence_to_play to {sequence.get_name()}")
                except Exception as e:
                    log(f"  ERROR: {e}")

                # Verify the set
                try:
                    seq = cdo.get_editor_property("sequence_to_play")
                    log(f"  Verification: sequence_to_play = {seq.get_name() if seq else 'None'}")
                except Exception as e:
                    log(f"  ERROR verifying: {e}")

    # Step 6: Final save
    log("")
    log("Step 6: Final save...")
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path)
        log(f"  Saved: {bp_path}")
    except Exception as e:
        log(f"  ERROR: {e}")

    # Step 7: Check compilation errors
    log("")
    log("Step 7: Checking for compilation errors...")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if bp:
        try:
            errors = automation.get_blueprint_compile_errors(bp)
            if errors:
                log(f"  Errors: {errors}")
            else:
                log("  No errors!")
        except Exception as e:
            log(f"  ERROR: {e}")

log("")
log("=" * 70)
log("Done!")
log("=" * 70)

# Write output to file
with open("C:/scripts/SLFConversion/migration_cache/sequence_actor_fix_v2.txt", 'w') as f:
    f.write('\n'.join(output))
