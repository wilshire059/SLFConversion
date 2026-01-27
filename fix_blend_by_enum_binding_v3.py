"""
Fix BlendListByEnum ActiveEnumValue binding - call with correct signature.
"""

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"


def log(msg):
    print(f"[BlendFix] {msg}")
    unreal.log_warning(f"[BlendFix] {msg}")


def main():
    log("=" * 70)
    log("FIX BLEND LIST BY ENUM BINDING v3")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return False

    log(f"Loaded: {bp.get_name()}")

    # Try fix_all_blend_list_by_enum_bindings with no extra args
    log("")
    log("=== CALLING fix_all_blend_list_by_enum_bindings ===")
    try:
        result = unreal.SLFAutomationLibrary.fix_all_blend_list_by_enum_bindings(bp)
        log(f"Result: {result} nodes fixed")
    except Exception as e:
        log(f"[ERROR] {e}")

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
    except:
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
