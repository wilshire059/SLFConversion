"""
Check B_Destructible physics settings to diagnose why barrels don't break.
"""

import unreal

def check_destructible_physics():
    """Check physics settings on spawned B_Destructible."""

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible"

    output_lines = []
    def log(msg):
        output_lines.append(msg)
        print(msg)

    log("\n" + "="*80)
    log("B_Destructible Physics Settings Check")
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

    # Spawn an instance
    spawn_location = unreal.Vector(0.0, 0.0, 500.0)
    spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_location)

    if not spawned:
        log("ERROR: Failed to spawn actor!")
        return

    log(f"\nSpawned actor: {spawned.get_name()}")

    # Find the GeometryCollectionComponent
    gc_components = spawned.get_components_by_class(unreal.GeometryCollectionComponent)
    log(f"\nGeometryCollectionComponents: {len(gc_components)}")

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

        # Check mobility
        try:
            mobility = comp.get_editor_property("mobility")
            mobility_str = str(mobility).split('.')[-1] if mobility else "Unknown"
            log(f"    Mobility: {mobility_str}")
        except Exception as e:
            log(f"    Mobility error: {e}")

        # Check collision
        log(f"\n  Collision Settings:")
        try:
            collision_enabled = comp.get_collision_enabled()
            log(f"    CollisionEnabled: {collision_enabled}")
        except Exception as e:
            log(f"    CollisionEnabled error: {e}")

        try:
            collision_profile = comp.get_editor_property("collision_profile_name")
            log(f"    CollisionProfileName: {collision_profile}")
        except Exception as e:
            log(f"    collision_profile_name error: {e}")

        try:
            object_type = comp.get_collision_object_type()
            log(f"    CollisionObjectType: {object_type}")
        except Exception as e:
            log(f"    CollisionObjectType error: {e}")

        # Check RestCollection
        log(f"\n  Geometry Collection:")
        try:
            rest_coll = comp.get_editor_property("rest_collection")
            if rest_coll:
                log(f"    RestCollection: {rest_coll.get_path_name()}")

                # Check GC asset settings
                log(f"\n    GC Asset Physics Settings:")
                try:
                    # Try to get physics info from the asset
                    log(f"      Asset class: {type(rest_coll).__name__}")
                except:
                    pass
            else:
                log(f"    RestCollection: None <-- PROBLEM!")
        except Exception as e:
            log(f"    RestCollection error: {e}")

        # Check if chaos break event is bound
        log(f"\n  Event Binding:")
        try:
            # Check if OnChaosBreakEvent has bound delegates
            has_break_event = hasattr(comp, 'on_chaos_break_event')
            log(f"    has OnChaosBreakEvent attribute: {has_break_event}")
        except:
            pass

        # Check if component is visible
        try:
            visible = comp.is_visible()
            log(f"    IsVisible: {visible}")
        except:
            pass

        # Check component enable state
        try:
            active = comp.is_active()
            log(f"    IsActive: {active}")
        except:
            pass

    # Check all other components
    log(f"\n" + "-"*40)
    log(f"All Components:")
    try:
        all_comps = spawned.get_components_by_class(unreal.ActorComponent)
        for comp in all_comps:
            log(f"  - {comp.get_name()} ({type(comp).__name__})")
    except Exception as e:
        log(f"  Error: {e}")

    # Cleanup
    log(f"\nCleaning up test actor...")
    unreal.EditorLevelLibrary.destroy_actor(spawned)

    log("\n" + "="*80)
    log("Check Complete")
    log("="*80 + "\n")

    # Write to file
    output_path = "C:/scripts/SLFConversion/migration_cache/destructible_physics_check.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))
    log(f"Output written to: {output_path}")

if __name__ == "__main__":
    check_destructible_physics()
