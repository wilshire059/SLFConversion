"""
Verify B_Destructible physics is now working after the fix.
Check that:
1. Component is active
2. Physics simulation is enabled
3. Component responds to physics
"""

import unreal

def verify_destructible_physics():
    """Verify physics settings on spawned B_Destructible after the fix."""

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible"

    output_lines = []
    def log(msg):
        output_lines.append(msg)
        print(msg)

    log("\n" + "="*80)
    log("B_Destructible Physics Verification (After Fix)")
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

    # Spawn an instance in the editor world
    spawn_location = unreal.Vector(0.0, 0.0, 500.0)
    spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_location)

    if not spawned:
        log("ERROR: Failed to spawn actor!")
        return

    log(f"\nSpawned actor: {spawned.get_name()}")

    # Find the GeometryCollectionComponent
    gc_components = spawned.get_components_by_class(unreal.GeometryCollectionComponent)
    log(f"GeometryCollectionComponents: {len(gc_components)}")

    for comp in gc_components:
        log(f"\n  Component: {comp.get_name()}")

        # Check physics settings
        log(f"\n  Physics Settings:")
        try:
            simulate_physics = comp.is_simulating_physics()
            log(f"    is_simulating_physics(): {simulate_physics}")
        except Exception as e:
            log(f"    is_simulating_physics error: {e}")

        try:
            body_instance = comp.get_editor_property("body_instance")
            if body_instance:
                log(f"    BodyInstance found")
                try:
                    simulate = body_instance.get_editor_property("simulate_physics")
                    log(f"      bSimulatePhysics: {simulate}")
                except:
                    pass
        except Exception as e:
            log(f"    body_instance error: {e}")

        # Check if component is active
        try:
            active = comp.is_active()
            log(f"    IsActive: {active}")
            if active:
                log(f"    *** PASS: Component is now ACTIVE! ***")
            else:
                log(f"    FAIL: Component still not active")
        except Exception as e:
            log(f"    IsActive error: {e}")

        # Check RestCollection
        try:
            rest_coll = comp.get_editor_property("rest_collection")
            if rest_coll:
                log(f"\n  RestCollection: {rest_coll.get_path_name()}")
            else:
                log(f"\n  RestCollection: None (PROBLEM!)")
        except Exception as e:
            log(f"  RestCollection error: {e}")

        # Check collision
        log(f"\n  Collision Settings:")
        try:
            collision_enabled = comp.get_collision_enabled()
            log(f"    CollisionEnabled: {collision_enabled}")
        except Exception as e:
            log(f"    CollisionEnabled error: {e}")

        try:
            object_type = comp.get_collision_object_type()
            log(f"    CollisionObjectType: {object_type}")
        except Exception as e:
            log(f"    CollisionObjectType error: {e}")

    # Cleanup
    log(f"\nCleaning up test actor...")
    unreal.EditorLevelLibrary.destroy_actor(spawned)

    log("\n" + "="*80)
    log("Verification Complete")
    log("="*80 + "\n")

    # Write to file
    output_path = "C:/scripts/SLFConversion/migration_cache/destructible_physics_verify.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))
    log(f"Output written to: {output_path}")

if __name__ == "__main__":
    verify_destructible_physics()
