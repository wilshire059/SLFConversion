# fix_overlay_bindings_simple.py
# Fix AnimBP BlendListByEnum overlay state bindings using C++ automation

import unreal

def log(msg):
    print(f"[OverlayFix] {msg}")
    unreal.log_warning(f"[OverlayFix] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 70)
    log("FIXING ANIMBP OVERLAY STATE BINDINGS")
    log("=" * 70)

    # Load the AnimBP
    animbp = unreal.load_asset(ANIMBP_PATH)
    if not animbp:
        log(f"[ERROR] Could not load AnimBP: {ANIMBP_PATH}")
        return

    log(f"Loaded: {animbp.get_name()}")

    # Call the C++ automation function
    try:
        fixed_count = unreal.SLFAutomationLibrary.fix_all_blend_list_by_enum_bindings(animbp)
        log(f"Fixed {fixed_count} BlendListByEnum node bindings")
    except Exception as e:
        log(f"[ERROR] fix_all_blend_list_by_enum_bindings: {e}")
        log("")
        log("If this fails, you need to manually fix in UE Editor:")
        log("1. Open ABP_SoulslikeCharacter_Additive AnimGraph")
        log("2. Find the two 'Blend Poses (E_OverlayState)' nodes")
        log("3. For FIRST node: Bind ActiveEnumValue to LeftHandOverlayState")
        log("4. For SECOND node: Bind ActiveEnumValue to RightHandOverlayState")
        log("5. Compile and Save")
        return

    # Save if successful
    if fixed_count > 0:
        try:
            unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH)
            log(f"SAVED: {ANIMBP_PATH}")
        except Exception as e:
            log(f"[ERROR] Save failed: {e}")
            return

        log("=" * 70)
        log("SUCCESS! Test in PIE - fingers should now grip the weapon.")
        log("=" * 70)
    else:
        log("No nodes needed fixing (may already be connected)")

if __name__ == "__main__":
    main()
