"""
Fix BlendListByEnum ActiveEnumValue binding - connect to OverlayState variable.

The BlendListByEnum nodes need their ActiveEnumValue pin connected to the
OverlayState variable (ESLFOverlayState) so they select the correct pose.
"""

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"


def log(msg):
    print(f"[BlendFix] {msg}")
    unreal.log_warning(f"[BlendFix] {msg}")


def main():
    log("=" * 70)
    log("FIX BLEND LIST BY ENUM BINDING")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return False

    log(f"Loaded: {bp.get_name()}")

    # Check if function exists
    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return False

    # Get diagnosis to see current BlendListByEnum state
    log("")
    log("=== CURRENT BLEND LIST BY ENUM STATE ===")

    try:
        diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

        # Find BlendListByEnum sections
        lines = diagnosis.split('\n')
        in_blend_section = False
        for line in lines:
            if "BLEND POSES BY ENUM" in line:
                in_blend_section = True
            if in_blend_section:
                log(line)
                if line.strip() == "" and in_blend_section:
                    in_blend_section = False
    except Exception as e:
        log(f"[WARN] Could not diagnose: {e}")

    # Try to fix the binding using available functions
    log("")
    log("=== ATTEMPTING FIX ===")

    # Check if there's a BindBlendListByEnumToVariable function
    if hasattr(unreal.SLFAutomationLibrary, 'bind_blend_list_by_enum_to_variable'):
        try:
            result = unreal.SLFAutomationLibrary.bind_blend_list_by_enum_to_variable(
                bp, "OverlayState"
            )
            log(f"Bind result: {result}")
        except Exception as e:
            log(f"[WARN] bind_blend_list_by_enum_to_variable: {e}")
    else:
        log("[INFO] bind_blend_list_by_enum_to_variable not available")

    # Alternative: try connect_blend_enum_to_property
    if hasattr(unreal.SLFAutomationLibrary, 'connect_blend_enum_to_property'):
        try:
            result = unreal.SLFAutomationLibrary.connect_blend_enum_to_property(
                bp, "OverlayState"
            )
            log(f"Connect result: {result}")
        except Exception as e:
            log(f"[WARN] connect_blend_enum_to_property: {e}")
    else:
        log("[INFO] connect_blend_enum_to_property not available")

    # Try FixBlendListByEnumActivePin
    if hasattr(unreal.SLFAutomationLibrary, 'fix_blend_list_by_enum_active_pin'):
        try:
            result = unreal.SLFAutomationLibrary.fix_blend_list_by_enum_active_pin(
                bp, "OverlayState"
            )
            log(f"Fix active pin result: {result}")
        except Exception as e:
            log(f"[WARN] fix_blend_list_by_enum_active_pin: {e}")
    else:
        log("[INFO] fix_blend_list_by_enum_active_pin not available")

    # List available functions for reference
    log("")
    log("=== AVAILABLE SLFAUTOMATIONLIBRARY FUNCTIONS ===")
    funcs = [f for f in dir(unreal.SLFAutomationLibrary) if not f.startswith('_')]
    blend_funcs = [f for f in funcs if 'blend' in f.lower() or 'enum' in f.lower()]
    for f in blend_funcs[:20]:
        log(f"  - {f}")

    # Compile and check
    log("")
    log("=== COMPILE ===")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log("[OK] Compiled")
    except Exception as e:
        log(f"[WARN] Compile: {e}")

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
