# test_interface_impl.py
# Test if the interfaces are actually implemented

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("INTERFACE IMPLEMENTATION TEST")
    unreal.log_warning("=" * 70)

    # Load the C++ class directly
    unreal.log_warning("\n--- Testing C++ AB_RestingPoint class directly ---")
    try:
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.B_RestingPoint")
        if cpp_class:
            unreal.log_warning(f"C++ AB_RestingPoint class: {cpp_class.get_name()}")
            unreal.log_warning(f"  Path: {cpp_class.get_path_name()}")

            # Check if this C++ class implements the interface
            interface_class = unreal.load_class(None, "/Script/SLFConversion.SLFInteractableInterface")
            if interface_class:
                unreal.log_warning(f"SLFInteractableInterface class: {interface_class.get_name()}")
            else:
                unreal.log_warning("  Could not load SLFInteractableInterface class!")
        else:
            unreal.log_warning("Could not load AB_RestingPoint C++ class!")
    except Exception as e:
        unreal.log_error(f"Error loading C++ class: {e}")

    # Load the Blueprint and check its generated class
    unreal.log_warning("\n--- Testing Blueprint B_RestingPoint ---")
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
    bp = unreal.load_asset(bp_path)
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            unreal.log_warning(f"Blueprint generated class: {gen_class.get_name()}")
            unreal.log_warning(f"  Path: {gen_class.get_path_name()}")

            # Try to get the CDO
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                cdo_class = cdo.get_class()
                unreal.log_warning(f"  CDO class: {cdo_class.get_name()}")
                unreal.log_warning(f"  CDO class path: {cdo_class.get_path_name()}")

    # Check AB_Interactable (the parent class)
    unreal.log_warning("\n--- Testing C++ AB_Interactable class ---")
    try:
        ab_interactable = unreal.load_class(None, "/Script/SLFConversion.B_Interactable")
        if ab_interactable:
            unreal.log_warning(f"AB_Interactable class: {ab_interactable.get_name()}")
            unreal.log_warning(f"  Path: {ab_interactable.get_path_name()}")
        else:
            unreal.log_warning("Could not load AB_Interactable C++ class!")
    except Exception as e:
        unreal.log_error(f"Error loading AB_Interactable: {e}")

    # Check if we can spawn and test interface
    unreal.log_warning("\n--- Spawn test ---")
    try:
        # Get world
        world = unreal.EditorLevelLibrary.get_editor_world()
        if world:
            # Spawn AB_RestingPoint directly
            spawn_loc = unreal.Vector(0, 0, 0)
            spawn_rot = unreal.Rotator(0, 0, 0)

            # Spawn using C++ class
            cpp_class = unreal.load_class(None, "/Script/SLFConversion.B_RestingPoint")
            if cpp_class:
                actor = unreal.EditorLevelLibrary.spawn_actor_from_class(cpp_class, spawn_loc, spawn_rot)
                if actor:
                    unreal.log_warning(f"Spawned C++ actor: {actor.get_name()}")
                    unreal.log_warning(f"  Class: {actor.get_class().get_name()}")
                    unreal.log_warning(f"  Path: {actor.get_class().get_path_name()}")

                    # Check components
                    components = actor.get_components_by_class(unreal.ActorComponent)
                    unreal.log_warning(f"  Components ({len(components)}):")
                    for c in components:
                        unreal.log_warning(f"    - {c.get_name()} ({c.get_class().get_name()})")

                    # Delete the test actor
                    actor.destroy_actor()
                    unreal.log_warning("  (Deleted test actor)")
                else:
                    unreal.log_warning("Failed to spawn C++ actor!")
    except Exception as e:
        unreal.log_error(f"Spawn test error: {e}")

if __name__ == "__main__":
    main()
