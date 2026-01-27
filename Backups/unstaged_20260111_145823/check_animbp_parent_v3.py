# check_animbp_parent_v3.py
# Check AnimBP parent class using SLFAutomationLibrary
import unreal

animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

bp = unreal.load_asset(animbp_path)
if not bp:
    print("[ERROR] Could not load AnimBP")
else:
    print(f"[INFO] Loaded: {bp.get_name()}")

    # Use SLFAutomationLibrary to get parent class
    if hasattr(unreal, 'SLFAutomationLibrary'):
        parent_str = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        print(f"[INFO] Parent Class: {parent_str}")

        variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        print(f"[INFO] Variables count: {len(variables)}")

        # Show variables with ?
        question_vars = [v for v in variables if "?" in v]
        print(f"[INFO] Variables with '?': {len(question_vars)}")
        for var in question_vars:
            print(f"[VAR?] {var}")

        # Show variables without ?
        normal_vars = [v for v in variables if "?" not in v]
        print(f"[INFO] Normal variables: {len(normal_vars)}")
        for var in normal_vars[:15]:  # First 15
            print(f"[VAR] {var}")
    else:
        print("[ERROR] SLFAutomationLibrary not available")
