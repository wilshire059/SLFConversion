# fix_variable_references.py
# Fix VariableGet nodes that reference variables with ? suffix
# Redirect them to the C++ property names (without ?)

import unreal

def log(msg):
    print(f"[FixVarRef] {msg}")
    unreal.log_warning(f"[FixVarRef] {msg}")

def main():
    log("=" * 70)
    log("FIXING VARIABLE REFERENCES (? suffix -> C++ properties)")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Map of old -> new variable names
    # Replace Blueprint variables with ? suffix with C++ properties
    replacements = [
        ("bIsAccelerating?", "bIsAccelerating"),
        ("bIsBlocking?", "bIsBlocking"),
        ("bIsFalling?", "bIsFalling"),
        ("IsResting?", "IsResting"),
        ("IsGuarding?", "IsGuarding"),
        ("LeftHandOverlayState_0", "LeftHandOverlayState"),
        ("RightHandOverlayState_0", "RightHandOverlayState"),
    ]

    total_fixed = 0

    for old_name, new_name in replacements:
        log(f"  Replacing: {old_name} -> {new_name}")
        fixed = unreal.SLFAutomationLibrary.replace_variable_references(bp, old_name, new_name)
        log(f"    Fixed {fixed} references")
        total_fixed += fixed

    log(f"")
    log(f"=== TOTAL FIXED: {total_fixed} ===")

    if total_fixed > 0:
        log("")
        log("=== SAVING ===")
        save_result = unreal.EditorAssetLibrary.save_asset(animbp_path, only_if_is_dirty=False)
        log(f"Save result: {save_result}")

        # Verify by inspecting transitions again
        log("")
        log("=== VERIFYING TRANSITIONS ===")
        result = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "")

        # Check first few transitions
        lines = result.split('\n')
        count = 0
        for line in lines:
            if "bIsAccelerating" in line or "bCanEnter" in line:
                log(f"  {line}")
                count += 1
                if count > 10:
                    break

if __name__ == "__main__":
    main()
