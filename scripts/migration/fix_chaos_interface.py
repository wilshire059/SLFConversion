"""
Fix B_Chaos_ForceField by removing the Blueprint interface BPI_DestructibleHelper.
The C++ parent class already implements ISLFDestructibleHelperInterface.
"""
import unreal

def main():
    unreal.log_warning("=== FIXING B_CHAOS_FORCEFIELD INTERFACE ===")
    unreal.log_warning("")

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField"

    # Check current state
    unreal.log_warning("=== BEFORE FIX ===")
    before = unreal.SLFAutomationLibrary.diagnose_interface_implementation(bp_path)
    unreal.log_warning(before)

    # Remove Blueprint-implemented interfaces
    unreal.log_warning("")
    unreal.log_warning("=== REMOVING BLUEPRINT INTERFACES ===")
    bp = unreal.load_asset(bp_path)
    if bp:
        removed = unreal.SLFAutomationLibrary.remove_implemented_interfaces(bp)
        unreal.log_warning(f"Removed {removed} interfaces")

        # Compile and save
        unreal.log_warning("Compiling and saving...")
        unreal.SLFAutomationLibrary.compile_and_save(bp)
    else:
        unreal.log_error(f"Failed to load: {bp_path}")
        return

    # Check after
    unreal.log_warning("")
    unreal.log_warning("=== AFTER FIX ===")
    after = unreal.SLFAutomationLibrary.diagnose_interface_implementation(bp_path)
    unreal.log_warning(after)

    unreal.log_warning("")
    unreal.log_warning("=== DONE ===")

if __name__ == "__main__":
    main()
