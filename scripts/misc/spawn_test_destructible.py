"""
Spawn test for B_Destructible to verify visibility fix.
"""

import unreal

def spawn_test_destructible():
    """Spawn a B_Destructible instance and check if it's configured correctly."""

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible"

    output_lines = []
    def log(msg):
        output_lines.append(msg)
        print(msg)

    log("\n" + "="*80)
    log("B_Destructible Spawn Test")
    log("="*80)

    # Load the Blueprint class
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        log(f"ERROR: Could not load Blueprint: {bp_path}")
        return

    gen_class = bp_asset.generated_class()
    if not gen_class:
        log("ERROR: No generated class!")
        return

    log(f"\nBlueprint class: {gen_class.get_name()}")

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if cdo:
        log(f"\nCDO Properties:")
        try:
            gc = cdo.get_editor_property("geometry_collection")
            log(f"  GeometryCollection: {gc.get_path_name() if gc else 'None'}")
        except Exception as e:
            log(f"  GeometryCollection error: {e}")

    # Spawn an instance in the editor world
    log(f"\nAttempting to spawn instance...")
    try:
        # Get editor world (use newer API)
        try:
            editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
            editor_world = editor_subsystem.get_editor_world() if editor_subsystem else None
        except:
            editor_world = unreal.EditorLevelLibrary.get_editor_world()

        if editor_world:
            log(f"  Editor world: {editor_world.get_name()}")

            # Spawn at origin using EditorLevelLibrary
            spawn_location = unreal.Vector(0.0, 0.0, 100.0)

            # Spawn the actor (simpler API)
            spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_location)

            if spawned:
                log(f"\n  Spawned actor: {spawned.get_name()}")
                log(f"  Actor class: {spawned.get_class().get_name()}")

                # Check components on spawned actor
                log(f"\n  Spawned Actor Components:")
                try:
                    gc_components = spawned.get_components_by_class(unreal.GeometryCollectionComponent)
                    log(f"    GeometryCollectionComponents: {len(gc_components)}")
                    for comp in gc_components:
                        log(f"      - {comp.get_name()}")
                        try:
                            rest_coll = comp.get_editor_property("rest_collection")
                            if rest_coll:
                                log(f"          RestCollection: {rest_coll.get_path_name()}")
                            else:
                                log(f"          RestCollection: None <-- VISIBILITY PROBLEM!")
                        except Exception as e:
                            log(f"          RestCollection error: {e}")
                        try:
                            visible = comp.is_visible()
                            log(f"          Visible: {visible}")
                        except:
                            pass
                except Exception as e:
                    log(f"    Component check error: {e}")

                # Check actor properties
                try:
                    actor_gc = spawned.get_editor_property("geometry_collection")
                    log(f"\n  Actor GeometryCollection property: {actor_gc.get_path_name() if actor_gc else 'None'}")
                except Exception as e:
                    log(f"\n  Actor property error: {e}")

                # Delete spawned actor
                log(f"\n  Deleting test actor...")
                unreal.EditorLevelLibrary.destroy_actor(spawned)
                log(f"  Test actor deleted")
            else:
                log(f"  ERROR: Failed to spawn actor!")
        else:
            log(f"  ERROR: No editor world!")
    except Exception as e:
        log(f"  Spawn error: {e}")

    log("\n" + "="*80)
    log("Spawn Test Complete")
    log("="*80 + "\n")

    # Write to file
    output_path = "C:/scripts/SLFConversion/migration_cache/destructible_spawn_test.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))
    log(f"Output written to: {output_path}")

if __name__ == "__main__":
    spawn_test_destructible()
