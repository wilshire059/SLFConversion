# fix_blend_bindings.py
# Fix BlendListByEnum overlay state bindings in AnimBP

import unreal

def log(msg):
    unreal.log_warning(f"[FIXBLEND] {msg}")

def main():
    log("=" * 70)
    log("FIXING BLENDLISTBYENUM OVERLAY STATE BINDINGS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Fix all BlendListByEnum bindings
    log("")
    log("=== FIXING BINDINGS ===")
    fixed = unreal.SLFAutomationLibrary.fix_all_blend_list_by_enum_bindings(bp)
    log(f"Fixed {fixed} BlendListByEnum bindings")

    # Save
    log("")
    log("=== SAVING ===")
    save_result = unreal.EditorAssetLibrary.save_asset(animbp_path, only_if_is_dirty=False)
    log(f"Save result: {save_result}")

    # Verify
    log("")
    log("=== VERIFYING ===")
    diagnostic = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

    # Find BlendListByEnum sections
    lines = diagnostic.split('\n')
    in_blend_section = False
    for line in lines:
        if 'BLEND POSES BY ENUM' in line or 'BlendListByEnum' in line:
            in_blend_section = True
        if in_blend_section:
            log(line)
            if line.strip() == '' and in_blend_section:
                in_blend_section = False

if __name__ == "__main__":
    main()
