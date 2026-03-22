"""
Diagnose B_Destructible CDO - check if GeometryCollection is set properly.
"""

import unreal

def diagnose_destructible():
    """Check both CDO and spawned instance for GeometryCollection."""

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible"
    gc_barrel_path = "/Game/SoulslikeFramework/Meshes/SM/Barrel/Destructible/GC_Barrel"

    output_lines = []
    def log(msg):
        output_lines.append(msg)
        print(msg)

    log("\n" + "="*80)
    log("B_Destructible CDO Diagnosis")
    log("="*80)

    # Load the Blueprint
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        log(f"ERROR: Could not load Blueprint: {bp_path}")
        return

    gen_class = bp_asset.generated_class()
    if not gen_class:
        log("ERROR: No generated class")
        return

    log(f"Blueprint: {bp_asset.get_name()}")
    log(f"Generated Class: {gen_class.get_name()}")

    # Check CDO
    log("\n--- CDO Analysis ---")
    cdo = unreal.get_default_object(gen_class)
    if cdo:
        log(f"CDO: {cdo.get_name()}")

        # Check GeometryCollection property
        try:
            gc_prop = cdo.get_editor_property("geometry_collection")
            log(f"CDO GeometryCollection: {gc_prop.get_path_name() if gc_prop else 'None'}")
        except Exception as e:
            log(f"CDO GeometryCollection: Error - {e}")

        # Check DestructionSound
        try:
            sound = cdo.get_editor_property("destruction_sound")
            log(f"CDO DestructionSound: {sound}")
        except Exception as e:
            log(f"CDO DestructionSound: Error - {e}")
    else:
        log("ERROR: Could not get CDO")

    # Check parent class
    log("\n--- Parent Class ---")
    try:
        parent = bp_asset.get_editor_property("parent_class")
        log(f"Parent Class: {parent.get_name() if parent else 'None'}")
        log(f"Parent Path: {parent.get_path_name() if parent else 'None'}")
    except Exception as e:
        log(f"Parent Class: Error - {e}")

    # Check GC_Barrel asset
    log("\n--- GC_Barrel Asset ---")
    gc_barrel = unreal.load_asset(gc_barrel_path)
    if gc_barrel:
        log(f"GC_Barrel loaded: {gc_barrel.get_path_name()}")
        log(f"Class: {gc_barrel.get_class().get_name()}")
    else:
        log(f"ERROR: Could not load GC_Barrel at {gc_barrel_path}")

    # Spawn and check
    log("\n--- Spawned Instance Analysis ---")
    spawn_location = unreal.Vector(0.0, 0.0, 5000.0)
    spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_location)

    if spawned:
        log(f"Spawned: {spawned.get_name()}")

        # Check GeometryCollection on spawned actor
        try:
            gc_prop = spawned.get_editor_property("geometry_collection")
            log(f"Spawned GeometryCollection: {gc_prop.get_path_name() if gc_prop else 'None'}")
        except Exception as e:
            log(f"Spawned GeometryCollection: Error - {e}")

        # Find and check GC component
        gc_comps = spawned.get_components_by_class(unreal.GeometryCollectionComponent)
        log(f"GeometryCollectionComponents count: {len(gc_comps)}")

        for i, gc in enumerate(gc_comps):
            log(f"\nGC Component {i}: {gc.get_name()}")

            # Check RestCollection
            try:
                rest = gc.get_editor_property("rest_collection")
                log(f"  RestCollection: {rest.get_path_name() if rest else 'None!'}")
            except Exception as e:
                log(f"  RestCollection: Error - {e}")

            # Check physics state
            log(f"  IsActive: {gc.is_active()}")
            log(f"  IsSimulatingPhysics: {gc.is_simulating_physics()}")
            log(f"  IsVisible: {gc.is_visible()}")

            # Check collision profile
            try:
                collision_enabled = gc.get_collision_enabled()
                log(f"  CollisionEnabled: {collision_enabled}")
            except:
                pass

        # Cleanup
        unreal.EditorLevelLibrary.destroy_actor(spawned)
    else:
        log("ERROR: Failed to spawn")

    # Now try manually setting GeometryCollection and respawning
    log("\n--- Manual GeometryCollection Set Test ---")
    if gc_barrel and cdo:
        try:
            cdo.set_editor_property("geometry_collection", gc_barrel)
            log(f"Set CDO GeometryCollection to: {gc_barrel.get_path_name()}")

            # Verify it stuck
            gc_verify = cdo.get_editor_property("geometry_collection")
            log(f"Verify CDO GeometryCollection: {gc_verify.get_path_name() if gc_verify else 'None'}")

            # Compile and spawn again
            unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
            log("Compiled blueprint")

            spawned2 = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_location)
            if spawned2:
                log(f"Spawned after set: {spawned2.get_name()}")

                try:
                    gc_prop2 = spawned2.get_editor_property("geometry_collection")
                    log(f"Spawned GeometryCollection after set: {gc_prop2.get_path_name() if gc_prop2 else 'None'}")
                except Exception as e:
                    log(f"Spawned GeometryCollection after set: Error - {e}")

                gc_comps2 = spawned2.get_components_by_class(unreal.GeometryCollectionComponent)
                for gc in gc_comps2:
                    try:
                        rest = gc.get_editor_property("rest_collection")
                        log(f"RestCollection after set: {rest.get_path_name() if rest else 'None!'}")
                    except Exception as e:
                        log(f"RestCollection after set: Error - {e}")

                unreal.EditorLevelLibrary.destroy_actor(spawned2)

            # Save the Blueprint
            unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            log("Saved blueprint")

        except Exception as e:
            log(f"Manual set error: {e}")

    log("\n" + "="*80)
    log("Diagnosis Complete")
    log("="*80 + "\n")

    # Write output
    output_path = "C:/scripts/SLFConversion/migration_cache/destructible_cdo_diagnosis.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))
    log(f"Output written to: {output_path}")

if __name__ == "__main__":
    diagnose_destructible()
