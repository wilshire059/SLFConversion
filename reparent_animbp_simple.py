"""
Simple reparenting of ABP_SoulslikeCharacter_Additive to C++ class.
Handles the variable name conflicts by removing Blueprint variables that shadow C++.
"""
import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
CPP_PARENT = "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive"

# Blueprint variables that should be deleted because C++ provides them
VARIABLES_TO_REMOVE = [
    # Overlay states (C++ provides LeftHandOverlayState, RightHandOverlayState)
    "RightHandOverlayState",
    "LeftHandOverlayState",
    "RightHandOverlayState_0",
    "LeftHandOverlayState_0",
    # Variables with "?" suffix (C++ provides without "?")
    "bIsAccelerating?",
    "bIsBlocking?",
    "bIsFalling?",
    "IsResting?",
    "IsGuarding?",
]

def log(msg):
    print(f"[Reparent] {msg}")
    unreal.log_warning(f"[Reparent] {msg}")

def simple_reparent():
    # Load the AnimBP
    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log(f"ERROR: Could not load {ANIMBP_PATH}")
        return False

    log(f"Loaded AnimBP: {bp.get_name()}")

    # Load target C++ parent class
    cpp_class = unreal.load_class(None, CPP_PARENT)
    if not cpp_class:
        log(f"ERROR: Could not load C++ class {CPP_PARENT}")
        return False

    log(f"Target C++ parent: {cpp_class.get_name()}")

    # Remove conflicting Blueprint variables BEFORE reparenting
    log("Removing conflicting Blueprint variables...")
    if hasattr(unreal, 'SLFAutomationLibrary'):
        for var_name in VARIABLES_TO_REMOVE:
            result = unreal.SLFAutomationLibrary.remove_variable(bp, var_name)
            if result:
                log(f"  Removed: {var_name}")
            else:
                log(f"  Not found: {var_name}")
    else:
        log("  ERROR: SLFAutomationLibrary not available")

    # Reparent using SLFAutomationLibrary (works better for AnimBPs)
    log("Reparenting to C++ class...")
    if hasattr(unreal, 'SLFAutomationLibrary'):
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, CPP_PARENT)
        log(f"SLFAutomationLibrary Reparent result: {result}")
    else:
        # Fallback to BlueprintEditorLibrary
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        log(f"BlueprintEditorLibrary Reparent result: {result}")

    # Compile the blueprint
    log("Compiling blueprint...")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    log("Blueprint compiled")

    # Save
    unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH)
    log(f"Saved {ANIMBP_PATH}")

    # Check file size to ensure we didn't lose data
    import os
    size = os.path.getsize("C:/scripts/SLFConversion/Content/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive.uasset")
    log(f"Final file size: {size} bytes")

    return True

if __name__ == "__main__":
    success = simple_reparent()
    print(f"\nResult: {'SUCCESS' if success else 'FAILED'}")
