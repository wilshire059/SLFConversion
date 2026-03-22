"""
Verify AnimBP pins - comprehensive check for:
1. Remaining "?" variables
2. Disconnected pins on NOT nodes
3. Disconnected pins on TransitionResult nodes
4. Property access issues
"""

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"


def log(msg):
    print(f"[Verify] {msg}")
    unreal.log_warning(f"[Verify] {msg}")


def main():
    log("=" * 70)
    log("ANIMBP PIN VERIFICATION")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"Loaded: {bp.get_name()}")

    # Check parent class
    try:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"Parent class: {parent}")
        if "ABP_SoulslikeCharacter_Additive" in parent:
            log("  [OK] Parented to C++")
        else:
            log("  [!!] Not parented to C++")
    except Exception as e:
        log(f"[WARN] Could not check parent: {e}")

    # Check for "?" variables
    log("")
    log("=== CHECKING FOR '?' VARIABLES ===")
    try:
        variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        question_vars = [v for v in variables if "?" in v]
        if question_vars:
            log(f"[!!] Found {len(question_vars)} variables with '?':")
            for v in question_vars:
                log(f"    - {v}")
        else:
            log("[OK] No '?' variables found")
    except Exception as e:
        log(f"[WARN] Could not check variables: {e}")

    # Full diagnosis
    log("")
    log("=== FULL ANIMBP DIAGNOSIS ===")
    try:
        diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

        # Count issues
        lines = diagnosis.split('\n')
        not_nodes_broken = 0
        transition_broken = 0
        property_access_issues = 0

        for line in lines:
            if "NOT node" in line and "disconnected" in line.lower():
                not_nodes_broken += 1
            if "TransitionResult" in line and "disconnected" in line.lower():
                transition_broken += 1
            if "Invalid field" in line or "?" in line:
                property_access_issues += 1

        log(f"NOT nodes with disconnected pins: {not_nodes_broken}")
        log(f"TransitionResult with disconnected pins: {transition_broken}")
        log(f"Property access issues: {property_access_issues}")

        # Show first 30 lines of diagnosis
        log("")
        log("--- Diagnosis output (first 30 lines) ---")
        for i, line in enumerate(lines[:30]):
            if line.strip():
                log(line)

    except Exception as e:
        log(f"[WARN] Could not diagnose: {e}")

    # Compile check
    log("")
    log("=== COMPILE CHECK ===")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)

        if errors:
            # Count specific issues
            will_never_taken = errors.count("will never be taken")
            invalid_field = errors.count("Invalid field")

            log(f"'will never be taken' warnings: {will_never_taken}")
            log(f"'Invalid field' warnings: {invalid_field}")

            if will_never_taken > 0 or invalid_field > 0:
                log("")
                log("--- Error details ---")
                for line in errors.split('\n')[:20]:
                    if "will never be taken" in line or "Invalid field" in line:
                        log(f"  {line.strip()}")
        else:
            log("[OK] No compile errors")

    except Exception as e:
        log(f"[WARN] Compile error: {e}")

    # Check transition graphs specifically
    log("")
    log("=== TRANSITION GRAPH CHECK ===")
    try:
        transition_info = unreal.SLFAutomationLibrary.get_transition_graph_info(bp)
        if transition_info:
            lines = transition_info.split('\n')
            for line in lines[:30]:
                if line.strip():
                    log(line)
        else:
            log("Could not get transition info")
    except Exception as e:
        log(f"[INFO] get_transition_graph_info not available: {e}")
        # Alternative: check the AnimGraph diagnosis for transitions
        pass

    log("")
    log("=" * 70)
    log("VERIFICATION COMPLETE")
    log("=" * 70)


if __name__ == "__main__":
    main()
