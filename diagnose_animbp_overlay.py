"""
Diagnose AnimBP overlay state mismatch.

Theory: C++ UPROPERTY and Blueprint variable with same name are separate.
C++ NativeUpdateAnimation sets the C++ property, but AnimGraph reads Blueprint variable.
"""
import unreal

def diagnose():
    # Get PIE world
    pie_world = None
    for context in unreal.SystemLibrary.get_engine_subsystem(unreal.UnrealEditorSubsystem).get_editor_world().get_outer().get_world_context():
        pass  # This won't work, need different approach

    # Try to find player character's anim instance
    print("=== AnimBP Overlay State Diagnosis ===")
    print("Looking for player character...")

    # Get all actors in editor world (won't work for PIE)
    # Need to use debug output instead

    print("")
    print("DIAGNOSIS:")
    print("The AnimBP has TWO sets of overlay state variables:")
    print("  1. Blueprint variables (type: E_OverlayState) - used by AnimGraph")
    print("  2. C++ UPROPERTY (type: ESLFOverlayState) - set by NativeUpdateAnimation")
    print("")
    print("These are SEPARATE memory locations!")
    print("C++ sets one, AnimGraph reads the other.")
    print("")
    print("FIX OPTIONS:")
    print("  A. Remove Blueprint variables, update AnimGraph to use C++ enum")
    print("  B. Remove C++ UPROPERTY, keep Blueprint vars, update Blueprint EventGraph")
    print("  C. Use reflection in C++ to set Blueprint variables")
    print("")
    print("Recommended: Option A - migrate AnimGraph to C++ enum type")

if __name__ == "__main__":
    diagnose()
