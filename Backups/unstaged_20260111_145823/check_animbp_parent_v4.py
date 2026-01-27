# check_animbp_parent_v4.py
# Check AnimBP parent class using SLFAutomationLibrary
import unreal

def log(msg):
    unreal.log_warning(f"[CHKP] {msg}")

animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

bp = unreal.load_asset(animbp_path)
if not bp:
    log("[ERROR] Could not load AnimBP")
else:
    log(f"Loaded: {bp.get_name()}")

    # Use SLFAutomationLibrary to get parent class
    if hasattr(unreal, 'SLFAutomationLibrary'):
        parent_str = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"Parent Class: {parent_str}")

        variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        log(f"Variables count: {len(variables)}")

        # Show variables with ?
        question_vars = [v for v in variables if "?" in v]
        log(f"Variables with '?': {len(question_vars)}")
        for var in question_vars:
            log(f"VAR? {var}")

        # Show variables without ?
        normal_vars = [v for v in variables if "?" not in v]
        log(f"Normal variables: {len(normal_vars)}")
        for var in normal_vars[:10]:  # First 10
            log(f"VAR: {var}")
    else:
        log("[ERROR] SLFAutomationLibrary not available")
