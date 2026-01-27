# verify_animbp_status.py
# Verify AnimBP compile status and check for errors

import unreal

def log(msg):
    unreal.log_warning(f"[VERIFY] {msg}")

def main():
    log("=" * 70)
    log("VERIFYING ANIMBP COMPILE STATUS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Check parent class
    log("")
    log("=== PARENT CLASS ===")
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent: {parent}")

    # Check Blueprint variables
    log("")
    log("=== BLUEPRINT VARIABLES ===")
    bp_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Count: {len(bp_vars)}")
    for v in bp_vars:
        log(f"  {v}")

    # Check compile status
    log("")
    log("=== COMPILE STATUS ===")
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        for line in errors.split('\n')[:50]:
            log(line)
    else:
        log("[OK] No compile errors or warnings!")

    log("")
    log("=== SUMMARY ===")
    if "/Script/SLFConversion" in str(parent):
        log("[OK] Reparented to C++ class")
    else:
        log("[WARN] NOT reparented to C++")

    if len(bp_vars) <= 5:
        log(f"[OK] Minimal Blueprint variables ({len(bp_vars)})")
    else:
        log(f"[WARN] Many Blueprint variables ({len(bp_vars)})")

    if not errors or "Errors: 0" in errors:
        log("[OK] No compile errors")
    else:
        log("[WARN] Has compile issues")

if __name__ == "__main__":
    main()
