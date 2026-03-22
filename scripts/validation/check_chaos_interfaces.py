"""
Check what interfaces B_Chaos_ForceField implements.
"""
import unreal

def main():
    unreal.log_warning("=== CHECKING B_CHAOS_FORCEFIELD INTERFACES ===")
    unreal.log_warning("")

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField"
    bp = unreal.load_asset(bp_path)

    if not bp:
        unreal.log_error(f"Failed to load: {bp_path}")
        return

    # Get the generated class
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"Generated Class: {gen_class.get_path_name()}")

        # Check what interfaces it implements
        unreal.log_warning("")
        unreal.log_warning("=== Implemented Interfaces ===")

        # Get all interfaces from the class hierarchy
        interfaces = gen_class.get_class_flags()
        unreal.log_warning(f"Class Flags: {interfaces}")

        # Try to check if it implements specific interfaces
        test_actor = None
        try:
            # Spawn a test actor to check interface implementation
            world = unreal.EditorLevelLibrary.get_editor_world()
            if world:
                test_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
                    gen_class,
                    unreal.Vector(0, 0, -10000),  # Far away
                    unreal.Rotator(0, 0, 0)
                )

                if test_actor:
                    # Check interfaces
                    unreal.log_warning("")
                    unreal.log_warning("=== Interface Check on Spawned Actor ===")

                    # Check if it implements our C++ interface
                    cpp_interface_path = "/Script/SLFConversion.SLFDestructibleHelperInterface"
                    bp_interface_path = "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_DestructibleHelper.BPI_DestructibleHelper_C"

                    # Use ImplementsInterface
                    cpp_interface_class = unreal.load_class(None, cpp_interface_path)
                    if cpp_interface_class:
                        implements_cpp = test_actor.get_class().implements_interface(cpp_interface_class)
                        unreal.log_warning(f"Implements C++ ISLFDestructibleHelperInterface: {implements_cpp}")
                    else:
                        unreal.log_warning(f"Could not load C++ interface class: {cpp_interface_path}")

                    bp_interface_class = unreal.load_class(None, bp_interface_path)
                    if bp_interface_class:
                        implements_bp = test_actor.get_class().implements_interface(bp_interface_class)
                        unreal.log_warning(f"Implements BP BPI_DestructibleHelper: {implements_bp}")
                    else:
                        unreal.log_warning(f"Could not load BP interface class: {bp_interface_path}")

                    # Clean up
                    test_actor.destroy_actor()
        except Exception as e:
            unreal.log_error(f"Error: {e}")
            if test_actor:
                test_actor.destroy_actor()

    # Check parent class
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    unreal.log_warning("")
    unreal.log_warning(f"Parent Class: {parent}")

    unreal.log_warning("")
    unreal.log_warning("=== DONE ===")

if __name__ == "__main__":
    main()
