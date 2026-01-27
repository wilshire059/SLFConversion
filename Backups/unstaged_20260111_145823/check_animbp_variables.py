# check_animbp_variables.py - List all AnimBP variables and their bindings
import unreal

def log(msg):
    print(msg)
    unreal.log_warning(f"[VARS] {msg}")

def main():
    log("=" * 70)
    log("CHECKING ANIMBP VARIABLES")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"AnimBP: {bp.get_name()}")

    # Get parent class
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent Class: {parent}")

    # Get all variables
    log("")
    log("BLUEPRINT VARIABLES:")
    vars_list = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    if vars_list:
        for v in vars_list:
            log(f"  - {v}")
    else:
        log("  (no variables found)")

    # Check the generated class for UPROPERTY names
    log("")
    log("GENERATED CLASS PROPERTIES (from C++ parent):")

    # Get the generated class
    gen_class = bp.generated_class()
    if gen_class:
        log(f"Generated class: {gen_class.get_name()}")

        # List some key properties
        key_props = [
            "Speed", "Direction", "Velocity", "Velocity2D",
            "bIsAccelerating", "bIsFalling", "bIsBlocking", "IsResting", "IsCrouched",
            "bIsAccelerating?", "bIsFalling?", "bIsBlocking?", "IsResting?",
            "Acceleration", "Acceleration2D", "WorldLocation", "WorldRotation"
        ]

        for prop_name in key_props:
            # Check if property exists
            try:
                cdo = unreal.get_default_object(gen_class)
                val = cdo.get_editor_property(prop_name.lower().replace("?", "").replace("(", "").replace(")", ""))
                log(f"  [OK] {prop_name} exists")
            except:
                log(f"  [--] {prop_name} NOT found")

if __name__ == "__main__":
    main()
