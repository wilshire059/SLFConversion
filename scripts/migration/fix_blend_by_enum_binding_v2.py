"""
Fix BlendListByEnum ActiveEnumValue binding using the correct function.
"""

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"


def log(msg):
    print(f"[BlendFix] {msg}")
    unreal.log_warning(f"[BlendFix] {msg}")


def main():
    log("=" * 70)
    log("FIX BLEND LIST BY ENUM BINDING v2")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return False

    log(f"Loaded: {bp.get_name()}")

    # Try fix_all_blend_list_by_enum_bindings first
    log("")
    log("=== TRYING fix_all_blend_list_by_enum_bindings ===")
    try:
        # This function should connect all BlendListByEnum ActiveEnumValue pins to their appropriate variables
        result = unreal.SLFAutomationLibrary.fix_all_blend_list_by_enum_bindings(bp, "OverlayState")
        log(f"Result: {result} nodes fixed")
    except Exception as e:
        log(f"[ERROR] fix_all_blend_list_by_enum_bindings: {e}")

    # Try fix_blend_list_by_enum_binding as fallback
    log("")
    log("=== TRYING fix_blend_list_by_enum_binding ===")
    try:
        # Try different variable names
        for var_name in ["OverlayState", "OverlayStateEnum", "CurrentOverlayState"]:
            result = unreal.SLFAutomationLibrary.fix_blend_list_by_enum_binding(bp, var_name)
            if result > 0:
                log(f"Success with '{var_name}': {result} nodes fixed")
                break
            else:
                log(f"'{var_name}': {result} nodes")
    except Exception as e:
        log(f"[ERROR] fix_blend_list_by_enum_binding: {e}")

    # Compile
    log("")
    log("=== COMPILE ===")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log("[OK] Compiled")
    except Exception as e:
        log(f"[WARN] Compile: {e}")

    # Check result
    log("")
    log("=== VERIFY RESULT ===")
    try:
        diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)
        lines = diagnosis.split('\n')
        for line in lines:
            if "BLEND POSES BY ENUM" in line or "ActiveEnumValue" in line:
                log(line)
    except Exception as e:
        log(f"[WARN] Diagnose: {e}")

    # Check compile errors
    try:
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if errors:
            will_never_taken = errors.count("will never be taken")
            log(f"'will never be taken' warnings: {will_never_taken}")
    except Exception as e:
        pass

    # Save
    log("")
    log("=== SAVE ===")
    try:
        saved = unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH, only_if_is_dirty=False)
        log(f"Save result: {saved}")
    except Exception as e:
        log(f"[ERROR] Save: {e}")

    log("")
    log("=" * 70)
    log("COMPLETE")
    log("=" * 70)

    return True


if __name__ == "__main__":
    main()
