"""
Verify AnimBP migration state - check for any remaining issues.
"""

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"


def log(msg):
    print(f"[Verify] {msg}")
    unreal.log_warning(f"[Verify] {msg}")


def main():
    log("=" * 70)
    log("VERIFY ANIMBP MIGRATION STATE")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    # Check parent class
    try:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"Parent class: {parent}")
        if "ABP_SoulslikeCharacter_Additive" in parent:
            log("  [OK] Reparented to C++ class")
        else:
            log("  [!!] Still using Blueprint parent")
    except Exception as e:
        log(f"[WARN] Could not check parent: {e}")

    # Check compile status
    try:
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if errors:
            if "No compile errors" in errors or "UP TO DATE" in errors:
                log("[OK] No compile errors")
            else:
                log(f"Compile status:\n{errors[:300]}")
        else:
            log("[OK] No compile errors")
    except Exception as e:
        log(f"[WARN] Could not check compile errors: {e}")

    # Check for BlendListByEnum bound enum
    log("")
    log("=== BLEND LIST BY ENUM NODES ===")
    try:
        diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

        # Extract relevant sections
        lines = diagnosis.split('\n')
        in_section = False
        for line in lines:
            if "BLEND POSES BY ENUM" in line or "BlendListByEnum" in line:
                in_section = True
            if in_section:
                log(line)
                if line.strip() == "" and in_section:
                    in_section = False
    except Exception as e:
        log(f"[WARN] Could not diagnose: {e}")

    # Check for ? variables in property access
    log("")
    log("=== PROPERTY ACCESS PATHS ===")
    try:
        # Export AnimGraph state to check property access
        state = unreal.SLFAutomationLibrary.export_anim_graph_state(bp, "")
        if state:
            if "IsGuarding?" in state:
                log("[!!] Found 'IsGuarding?' in AnimGraph state")
            else:
                log("[OK] No 'IsGuarding?' found in AnimGraph state")

            # Check for other ? variables
            problem_vars = ["bIsAccelerating?", "bIsBlocking?", "bIsFalling?", "IsResting?"]
            for var in problem_vars:
                if var in state:
                    log(f"[!!] Found '{var}' in AnimGraph state")
                else:
                    log(f"[OK] No '{var}' found")
        else:
            log("[WARN] Could not export AnimGraph state")
    except Exception as e:
        log(f"[WARN] Could not check property access: {e}")

    log("")
    log("=" * 70)
    log("VERIFICATION COMPLETE")
    log("=" * 70)


if __name__ == "__main__":
    main()
