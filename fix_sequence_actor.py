import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

log("=" * 70)
log("FIXING B_SequenceActor - REPARENT AND SET SEQUENCE")
log("=" * 70)

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
new_parent = "/Script/SLFConversion.B_SequenceActor"
sequence_path = "/Game/SoulslikeFramework/Cinematics/LS_ShowcaseRoom"

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

    # Step 1: Reparent to C++ class
    log("")
    log("Step 1: Reparenting to C++ class...")

    new_parent_class = unreal.load_class(None, new_parent)
    if new_parent_class:
        log(f"  Found parent class: {new_parent_class.get_name()}")

        try:
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent_class)
            if result:
                log("  SUCCESS: Reparented Blueprint")
            else:
                log("  FAILED: Reparent returned False")
        except Exception as e:
            log(f"  ERROR reparenting: {e}")
    else:
        log(f"  ERROR: Could not load parent class: {new_parent}")

    # Step 2: Clear EventGraph
    log("")
    log("Step 2: Clearing EventGraph...")

    try:
        # Get all graphs
        graphs = unreal.BlueprintEditorLibrary.get_all_graphs(bp)
        log(f"  Found {len(graphs)} graphs")

        for graph in graphs:
            graph_name = graph.get_name()
            log(f"    Graph: {graph_name}")

            # Clear EventGraph only
            if "EventGraph" in graph_name:
                nodes = unreal.BlueprintEditorLibrary.get_all_nodes(graph)
                log(f"    Found {len(nodes)} nodes in EventGraph")

                for node in nodes:
                    try:
                        unreal.BlueprintEditorLibrary.remove_node(node)
                    except:
                        pass

                log("    Cleared EventGraph nodes")
    except Exception as e:
        log(f"  ERROR clearing EventGraph: {e}")

    # Step 3: Compile Blueprint
    log("")
    log("Step 3: Compiling Blueprint...")

    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log("  Compiled Blueprint")
    except Exception as e:
        log(f"  ERROR compiling: {e}")

    # Step 4: Set SequenceToPlay property
    log("")
    log("Step 4: Setting SequenceToPlay property...")

    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            log(f"  Got CDO: {cdo.get_name()}")

            # List available properties
            log("  Available properties:")
            for prop_name in dir(cdo):
                if not prop_name.startswith('_'):
                    try:
                        val = cdo.get_editor_property(prop_name)
                        if "sequence" in prop_name.lower() or "play" in prop_name.lower():
                            log(f"    {prop_name}: {type(val).__name__}")
                    except:
                        pass

            # Try setting the property
            property_names = [
                "sequence_to_play",
                "SequenceToPlay",
                "level_sequence",
                "LevelSequence"
            ]

            success = False
            for prop_name in property_names:
                try:
                    cdo.set_editor_property(prop_name, sequence)
                    log(f"  SUCCESS: Set {prop_name}")
                    success = True
                    break
                except Exception as e:
                    log(f"  Failed {prop_name}: {e}")

            if not success:
                log("  WARNING: Could not set sequence property")
        else:
            log("  ERROR: Could not get CDO")
    else:
        log("  ERROR: Could not get generated class")

    # Step 5: Save
    log("")
    log("Step 5: Saving Blueprint...")

    try:
        unreal.EditorAssetLibrary.save_asset(bp_path)
        log(f"  Saved: {bp_path}")
    except Exception as e:
        log(f"  ERROR saving: {e}")

log("")
log("=" * 70)
log("Done!")
log("=" * 70)

# Write output to file
with open("C:/scripts/SLFConversion/migration_cache/sequence_actor_fix.txt", 'w') as f:
    f.write('\n'.join(output))
