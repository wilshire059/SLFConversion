"""
Detailed comparison of B_Destructible between SLFConversion and bp_only.
Focus on physics and collision settings that could affect breaking.
"""

import unreal

def compare_destructible():
    """Compare GeometryCollectionComponent settings in detail."""

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible"

    output_lines = []
    def log(msg):
        output_lines.append(msg)
        print(msg)

    log("\n" + "="*80)
    log("B_Destructible Detailed Component Analysis")
    log("="*80)

    # Load and spawn
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        log(f"ERROR: Could not load Blueprint: {bp_path}")
        return

    gen_class = bp_asset.generated_class()
    spawn_location = unreal.Vector(0.0, 0.0, 5000.0)
    spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_location)

    if not spawned:
        log("ERROR: Failed to spawn")
        return

    log(f"\nSpawned: {spawned.get_name()}")

    # Find GC component
    gc_comps = spawned.get_components_by_class(unreal.GeometryCollectionComponent)
    if not gc_comps:
        log("ERROR: No GeometryCollectionComponent found!")
        unreal.EditorLevelLibrary.destroy_actor(spawned)
        return

    gc = gc_comps[0]
    log(f"Component: {gc.get_name()}")

    # Detailed property inspection
    log(f"\n--- Simulation Settings ---")

    properties_to_check = [
        # Physics
        ("simulate_physics", "bool"),
        ("enable_gravity", "bool"),
        ("mass_in_kg", "float"),
        ("linear_damping", "float"),
        ("angular_damping", "float"),

        # Collision
        ("collision_enabled", "enum"),
        ("generate_overlap_events", "bool"),
        ("can_ever_affect_navigation", "bool"),

        # Chaos-specific
        ("notify_breaks", "bool"),
        ("notify_collisions", "bool"),
        ("notify_trailing", "bool"),
        ("notify_removals", "bool"),
        ("use_size_specific_damage_threshold", "bool"),
        ("damage_propagation_data", "struct"),
        ("enable_clustering", "bool"),
        ("cluster_group_index", "int"),
        ("max_cluster_level", "int"),

        # Component state
        ("is_visible", "bool"),
        ("mobility", "enum"),
    ]

    for prop_name, prop_type in properties_to_check:
        try:
            value = gc.get_editor_property(prop_name)
            log(f"  {prop_name}: {value}")
        except Exception as e:
            pass  # Property doesn't exist or not accessible

    # Check BodyInstance
    log(f"\n--- BodyInstance ---")
    try:
        body = gc.get_editor_property("body_instance")
        if body:
            body_props = [
                "simulate_physics",
                "collision_enabled",
                "object_type",
                "mass_in_kg",
                "linear_damping",
                "angular_damping",
                "enable_gravity",
                "start_awake",
                "physics_blend_weight",
                "position_solver_iteration_count",
                "velocity_solver_iteration_count",
                "collision_responses",
            ]
            for prop in body_props:
                try:
                    val = body.get_editor_property(prop)
                    log(f"  {prop}: {val}")
                except:
                    pass
    except Exception as e:
        log(f"  BodyInstance error: {e}")

    # Check collision responses
    log(f"\n--- Collision ---")
    try:
        obj_type = gc.get_collision_object_type()
        log(f"  ObjectType: {obj_type}")
    except Exception as e:
        log(f"  ObjectType error: {e}")

    try:
        collision_enabled = gc.get_collision_enabled()
        log(f"  CollisionEnabled: {collision_enabled}")
    except Exception as e:
        log(f"  CollisionEnabled error: {e}")

    # Check collision profile
    try:
        profile_name = gc.get_collision_profile_name()
        log(f"  CollisionProfileName: {profile_name}")
    except Exception as e:
        log(f"  CollisionProfileName: Not accessible via this API")

    # Check collision responses for specific channels
    log(f"\n--- Collision Responses ---")
    channels = [
        unreal.CollisionChannel.ECC_WORLD_STATIC,
        unreal.CollisionChannel.ECC_WORLD_DYNAMIC,
        unreal.CollisionChannel.ECC_PAWN,
        unreal.CollisionChannel.ECC_VISIBILITY,
        unreal.CollisionChannel.ECC_CAMERA,
        unreal.CollisionChannel.ECC_PHYSICS_BODY,
        unreal.CollisionChannel.ECC_VEHICLE,
        unreal.CollisionChannel.ECC_DESTRUCTIBLE,
    ]
    for channel in channels:
        try:
            response = gc.get_collision_response_to_channel(channel)
            log(f"  {channel.name}: {response}")
        except Exception as e:
            pass

    # RestCollection info
    log(f"\n--- RestCollection ---")
    try:
        rest_coll = gc.get_editor_property("rest_collection")
        if rest_coll:
            log(f"  Asset: {rest_coll.get_path_name()}")

            # Try to get damage threshold from asset
            try:
                damage_threshold = rest_coll.get_editor_property("damage_threshold")
                log(f"  DamageThreshold: {damage_threshold}")
            except:
                pass

            try:
                damage_model = rest_coll.get_editor_property("damage_model")
                log(f"  DamageModel: {damage_model}")
            except:
                pass
        else:
            log("  RestCollection: None!")
    except Exception as e:
        log(f"  RestCollection error: {e}")

    # Check if there are any physics fields affecting this
    log(f"\n--- Physics State (Runtime) ---")
    try:
        log(f"  is_simulating_physics(): {gc.is_simulating_physics()}")
        log(f"  is_active(): {gc.is_active()}")
        log(f"  is_visible(): {gc.is_visible()}")
        log(f"  is_registered(): {gc.is_registered()}")
    except Exception as e:
        log(f"  State check error: {e}")

    # Cleanup
    unreal.EditorLevelLibrary.destroy_actor(spawned)

    log("\n" + "="*80)
    log("Analysis Complete")
    log("="*80 + "\n")

    # Write output
    output_path = "C:/scripts/SLFConversion/migration_cache/destructible_detailed.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))
    log(f"Output written to: {output_path}")

if __name__ == "__main__":
    compare_destructible()
