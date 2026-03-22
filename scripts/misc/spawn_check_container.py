# spawn_check_container.py
# Spawn a B_Container instance and check its components

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("SPAWNING AND CHECKING B_CONTAINER")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("No generated class")
        return

    unreal.log_warning(f"Generated class: {gen_class.get_name()}")

    # Check CDO type
    cdo = unreal.get_default_object(gen_class)
    if cdo:
        unreal.log_warning(f"CDO Type: {type(cdo).__name__}")

    # Spawn instance
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        unreal.log_error("No editor world")
        return

    spawn_location = unreal.Vector(0, 0, -5000)  # Spawn underground so it doesn't interfere
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_location)
    if not actor:
        unreal.log_error("Failed to spawn actor")
        return

    unreal.log_warning(f"\nSpawned: {actor.get_name()}")
    unreal.log_warning(f"Actor Type: {type(actor).__name__}")

    # Get all components on spawned instance
    components = actor.get_components_by_class(unreal.ActorComponent)
    unreal.log_warning(f"\nInstance Components: {len(components) if components else 0}")
    if components:
        for comp in components:
            comp_name = comp.get_name()
            comp_type = type(comp).__name__
            unreal.log_warning(f"  - {comp_name} ({comp_type})")

            # Check if it's the Lid component
            if "Lid" in comp_name and isinstance(comp, unreal.StaticMeshComponent):
                try:
                    mesh = comp.get_editor_property("static_mesh")
                    if mesh:
                        unreal.log_warning(f"      Mesh: {mesh.get_name()}")
                except:
                    pass

    # Check if interface is implemented
    interface_class = unreal.load_class(None, "/Script/SLFConversion.SLFInteractableInterface")
    if interface_class and actor:
        # Check via class
        actor_class = actor.get_class()
        unreal.log_warning(f"\nActor class: {actor_class.get_name()}")
        unreal.log_warning(f"Actor class path: {actor_class.get_path_name()}")

    # Clean up
    actor.destroy_actor()
    unreal.log_warning("\nActor destroyed")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
