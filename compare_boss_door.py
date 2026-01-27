"""
Compare Boss Door configuration between bp_only and SLFConversion.
This script examines scale, components, and transforms to identify the door scale issue.
"""

import unreal

def compare_boss_door():
    """Compare boss door configuration."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("COMPARING BOSS DOOR CONFIGURATION")
    unreal.log_warning("=" * 70)

    # Load the B_BossDoor Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
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
    # Parent class check removed - method not available in Python API

    # Get CDO and check default values
    cdo = unreal.get_default_object(gen_class)
    if cdo:
        unreal.log_warning("\n=== CDO Properties ===")

        # Check scale-related properties
        try:
            actor_scale = getattr(cdo, 'actor_scale_3d', None)
            if actor_scale:
                unreal.log_warning(f"Actor Scale 3D: {actor_scale}")
        except:
            pass

        # Check specific B_Door properties
        try:
            yaw_rotation = getattr(cdo, 'yaw_rotation_amount', None)
            if yaw_rotation:
                unreal.log_warning(f"Yaw Rotation Amount: {yaw_rotation}")
        except:
            pass

    # Spawn a test instance to check components
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        unreal.log_warning("[ERROR] No editor world")
        return

    # Spawn actor
    spawn_loc = unreal.Vector(0, 0, 1000)  # Spawn high to avoid collision
    spawn_rot = unreal.Rotator(0, 0, 0)

    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc, spawn_rot)

    if not actor:
        unreal.log_warning("[ERROR] Could not spawn test actor")
        return

    unreal.log_warning(f"\n=== Spawned Actor: {actor.get_name()} ===")

    # Get actor scale
    actor_scale = actor.get_actor_scale3d()
    unreal.log_warning(f"Actor Scale: X={actor_scale.x}, Y={actor_scale.y}, Z={actor_scale.z}")

    # Get all components and their transforms
    components = actor.get_components_by_class(unreal.SceneComponent)

    unreal.log_warning(f"\n=== Components ({len(components)}) ===")

    for comp in components:
        if not comp:
            continue

        comp_name = comp.get_name()
        comp_class = comp.__class__.__name__

        # Get relative transform
        rel_loc = comp.get_relative_location()
        rel_rot = comp.get_relative_rotation()
        rel_scale = comp.get_relative_scale3d()

        unreal.log_warning(f"\n  {comp_name} ({comp_class})")
        unreal.log_warning(f"    Relative Location: X={rel_loc.x:.2f}, Y={rel_loc.y:.2f}, Z={rel_loc.z:.2f}")
        unreal.log_warning(f"    Relative Rotation: P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f}")
        unreal.log_warning(f"    Relative Scale: X={rel_scale.x:.2f}, Y={rel_scale.y:.2f}, Z={rel_scale.z:.2f}")

        # Check if it's a static mesh component
        if isinstance(comp, unreal.StaticMeshComponent):
            mesh = comp.get_static_mesh()
            if mesh:
                unreal.log_warning(f"    Static Mesh: {mesh.get_name()}")

                # Get mesh bounds
                bounds = comp.get_local_bounds()
                box_extent = bounds.box_extent
                unreal.log_warning(f"    Mesh Bounds Extent: X={box_extent.x:.2f}, Y={box_extent.y:.2f}, Z={box_extent.z:.2f}")

        # Check if it's a niagara component
        if isinstance(comp, unreal.NiagaraComponent):
            asset = comp.get_asset()
            if asset:
                unreal.log_warning(f"    Niagara Asset: {asset.get_name()}")

    # Check for MoveTo component specifically
    unreal.log_warning("\n=== Looking for MoveTo Component ===")
    billboard_comps = actor.get_components_by_class(unreal.BillboardComponent)
    for bb in billboard_comps:
        unreal.log_warning(f"BillboardComponent: {bb.get_name()}")
        rel_loc = bb.get_relative_location()
        unreal.log_warning(f"  Location: X={rel_loc.x:.2f}, Y={rel_loc.y:.2f}, Z={rel_loc.z:.2f}")

    # Check for Interactable SM specifically
    unreal.log_warning("\n=== Looking for Interactable SM ===")
    static_mesh_comps = actor.get_components_by_class(unreal.StaticMeshComponent)
    for sm in static_mesh_comps:
        name = sm.get_name()
        unreal.log_warning(f"StaticMeshComponent: {name}")
        mesh = sm.get_static_mesh()
        if mesh:
            unreal.log_warning(f"  Mesh: {mesh.get_name()}")
        rel_scale = sm.get_relative_scale3d()
        unreal.log_warning(f"  Scale: X={rel_scale.x:.2f}, Y={rel_scale.y:.2f}, Z={rel_scale.z:.2f}")

        # World bounds
        bounds = sm.get_bounds()
        unreal.log_warning(f"  World Origin: X={bounds.origin.x:.2f}, Y={bounds.origin.y:.2f}, Z={bounds.origin.z:.2f}")
        unreal.log_warning(f"  World Extent: X={bounds.box_extent.x:.2f}, Y={bounds.box_extent.y:.2f}, Z={bounds.box_extent.z:.2f}")

    # Clean up test actor
    actor.destroy_actor()

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("COMPARISON COMPLETE - Actor destroyed")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    compare_boss_door()
