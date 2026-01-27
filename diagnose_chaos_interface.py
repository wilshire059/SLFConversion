"""
Diagnose interface implementation for B_Chaos_ForceField.
Uses C++ SLFAutomationLibrary function.
"""
import unreal

def main():
    unreal.log_warning("=== DIAGNOSING B_CHAOS_FORCEFIELD INTERFACE ===")
    unreal.log_warning("")

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField"

    # Call C++ diagnostic function
    result = unreal.SLFAutomationLibrary.diagnose_interface_implementation(bp_path)

    unreal.log_warning("")
    unreal.log_warning("=== RESULT JSON ===")
    unreal.log_warning(result)
    unreal.log_warning("")
    unreal.log_warning("=== DONE ===")

if __name__ == "__main__":
    main()
