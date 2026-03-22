"""
Reparent AnimBP to C++ class WITHOUT removing variables.
The variable fix has already been done - we just need to reparent.
"""
import unreal

def log(msg):
    print(f"[ReparentOnly] {msg}")
    unreal.log_warning(f"[ReparentOnly] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
CPP_PARENT = "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 60)
    log("REPARENTING ANIMBP (without removing variables)")
    log("=" * 60)

    # Load the AnimBP
    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log(f"ERROR: Could not load {ANIMBP_PATH}")
        return False

    log(f"Loaded AnimBP: {bp.get_name()}")

    # Reparent using SLFAutomationLibrary
    if hasattr(unreal, 'SLFAutomationLibrary'):
        log(f"Reparenting to: {CPP_PARENT}")
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, CPP_PARENT)
        log(f"Reparent result: {result}")
    else:
        log("ERROR: SLFAutomationLibrary not available")
        return False

    # Compile
    log("Compiling...")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    log("Blueprint compiled")

    # Save
    unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH)
    log(f"Saved")

    return True

if __name__ == "__main__":
    main()
