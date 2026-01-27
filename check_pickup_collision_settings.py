"""
Check pickup item collision settings.
This script examines B_PickupItem Blueprint SCS to see collision configuration.
"""

import unreal

def check_pickup_collision():
    """Check collision settings on B_PickupItem Blueprint."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING B_PickupItem COLLISION SETTINGS")
    unreal.log_warning("=" * 70)

    # Load the B_PickupItem Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem"
    bp = unreal.load_asset(bp_path)

    if not bp:
        unreal.log_warning(f"[ERROR] Could not load Blueprint: {bp_path}")
        return

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    # Get the generated class
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning("[ERROR] No generated class")
        return

    unreal.log_warning(f"Generated Class: {gen_class.get_name()}")

    # Spawn a test instance to check components
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        unreal.log_warning("[ERROR] No editor world")
        return

    # Spawn actor
    spawn_loc = unreal.Vector(0, 0, 0)
    spawn_rot = unreal.Rotator(0, 0, 0)

    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc, spawn_rot)

    if not actor:
        unreal.log_warning("[ERROR] Could not spawn test actor")
        return

    unreal.log_warning(f"Spawned test actor: {actor.get_name()}")

    # Get all components
    components = actor.get_components_by_class(unreal.SceneComponent)

    unreal.log_warning(f"\nFound {len(components)} scene components:")

    for comp in components:
        unreal.log_warning(f"  - {comp.get_name()} (Class: {comp.__class__.__name__})")

        # Check if it's a primitive component (has collision)
        if isinstance(comp, unreal.PrimitiveComponent):
            collision_enabled = comp.get_collision_enabled()
            collision_object_type = comp.get_collision_object_type()

            unreal.log_warning(f"    Collision Enabled: {collision_enabled}")
            unreal.log_warning(f"    Object Type: {collision_object_type}")

            # Check collision responses
            try:
                # Check response to each channel we trace
                channels_to_check = [
                    ("WorldStatic", unreal.CollisionChannel.ECC_WORLD_STATIC),
                    ("WorldDynamic", unreal.CollisionChannel.ECC_WORLD_DYNAMIC),
                    ("Pawn", unreal.CollisionChannel.ECC_PAWN),
                    ("PhysicsBody", unreal.CollisionChannel.ECC_PHYSICS_BODY),
                    ("Visibility", unreal.CollisionChannel.ECC_VISIBILITY),
                ]

                for channel_name, channel in channels_to_check:
                    response = comp.get_collision_response_to_channel(channel)
                    unreal.log_warning(f"    Response to {channel_name}: {response}")
            except Exception as e:
                unreal.log_warning(f"    Could not get collision responses: {e}")

    # Check sphere component specifically
    sphere_comps = actor.get_components_by_class(unreal.SphereComponent)
    unreal.log_warning(f"\nFound {len(sphere_comps)} SphereComponents:")

    for sphere in sphere_comps:
        unreal.log_warning(f"  - {sphere.get_name()}")
        unreal.log_warning(f"    Radius: {sphere.get_unscaled_sphere_radius()}")
        unreal.log_warning(f"    Collision Enabled: {sphere.get_collision_enabled()}")
        unreal.log_warning(f"    Object Type: {sphere.get_collision_object_type()}")
        unreal.log_warning(f"    Is Query Collision: {sphere.is_query_collision_enabled()}")
        unreal.log_warning(f"    Generate Overlap Events: {sphere.get_generates_overlap_events()}")

    # Clean up test actor
    actor.destroy_actor()

    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECK COMPLETE - Actor destroyed")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    check_pickup_collision()
