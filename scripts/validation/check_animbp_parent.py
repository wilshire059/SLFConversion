# check_animbp_parent.py
# Check if AnimBP is using the correct C++ parent class

import unreal

ANIMBP_PATHS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",  # PLAYER
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
]

def log(msg):
    print(f"[CheckAnimBP] {msg}")
    unreal.log_warning(f"[CheckAnimBP] {msg}")

def main():
    log("=" * 60)
    log("ANIMBP PARENT CLASS CHECK")
    log("=" * 60)

    for path in ANIMBP_PATHS:
        log(f"\n{path.split('/')[-1]}:")

        bp = unreal.load_asset(path)
        if not bp:
            log("  FAILED TO LOAD")
            continue

        # Get the generated class
        gen_class = bp.generated_class()
        if not gen_class:
            log("  No generated class")
            continue

        # AnimBlueprint stores parent in get_editor_property
        try:
            parent = bp.get_editor_property("parent_class")
        except:
            parent = None
        log(f"  parent_class: {parent.get_name() if parent else 'None'}")
        if parent:
            log(f"  Parent Path: {parent.get_path_name()}")

            # Check if it's the expected C++ class
            parent_name = parent.get_name()
            if "ABP_Soulslike" in parent_name or "SLFEnemy" in parent_name:
                log(f"  STATUS: Using C++ parent class - GOOD")
            elif parent_name == "AnimInstance":
                log(f"  STATUS: Using base AnimInstance - MAY NEED REPARENTING!")
            else:
                log(f"  STATUS: Parent is {parent_name}")

            # Check if parent has bIsAccelerating property using C++ library
            if hasattr(unreal, 'SLFAutomationLibrary'):
                has_prop = unreal.SLFAutomationLibrary.class_has_property(parent, "bIsAccelerating")
                log(f"  Parent has C++ bIsAccelerating: {has_prop}")

            # Also try get_editor_property
            try:
                cdo = parent.get_default_object()
                if cdo:
                    prop_val = cdo.get_editor_property("bIsAccelerating")
                    log(f"  Parent CDO bIsAccelerating = {prop_val}")
            except Exception as e:
                log(f"  Parent CDO has no bIsAccelerating: {str(e)[:50]}")
        else:
            log("  No parent class found")

        # Check Blueprint variables
        if hasattr(unreal, 'SLFAutomationLibrary'):
            vars_list = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
            relevant_vars = [v for v in vars_list if "Accelerating" in v or "Resting" in v or "Blocking" in v or "Falling" in v]
            if relevant_vars:
                log(f"  Blueprint vars with '?': {relevant_vars}")
            else:
                log(f"  No problematic Blueprint vars found")

    log("\n" + "=" * 60)

if __name__ == "__main__":
    main()
