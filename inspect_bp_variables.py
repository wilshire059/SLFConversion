# inspect_bp_variables.py
# Inspect what variables and VariableGet nodes exist in the AnimBP

import unreal

def log(msg):
    print(f"[Inspect] {msg}")
    unreal.log_warning(f"[Inspect] {msg}")

def main():
    log("=" * 70)
    log("INSPECTING ANIMBP VARIABLES")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Get all Blueprint variables
    log("\n=== BLUEPRINT VARIABLES ===")
    variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    for var in variables:
        if "Accelerating" in var or "Blocking" in var or "Falling" in var or "Resting" in var or "Guarding" in var or "Overlay" in var:
            log(f"  Variable: '{var}'")

    # Get all VariableGet node names across all graphs
    log("\n=== VARIABLEGET NODE NAMES (sample from transitions) ===")
    result = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "")
    for line in result.split('\n'):
        if "VariableGet" in line or "bIsAccelerating" in line or "bIsBlocking" in line or "bIsFalling" in line or "IsResting" in line:
            log(f"  {line.strip()}")

if __name__ == "__main__":
    main()
