"""
Fix ANS_ToggleChaosField reparenting and resave montages.
"""
import unreal

def reparent_if_needed():
    """Reparent ANS_ToggleChaosField to C++ class if not already done."""
    bp_path = "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_ToggleChaosField"
    cpp_class_path = "/Script/SLFConversion.ANS_ToggleChaosField"

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load {bp_path}")
        return False

    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        unreal.log_error(f"C++ class not found: {cpp_class_path}")
        return False

    # Try to reparent
    unreal.log_warning(f"Reparenting {bp.get_name()} to {cpp_class_path}...")
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if result:
            unreal.log_warning("Reparent succeeded")
            # Clear event graph since C++ handles it now
            clear_result = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
            unreal.log_warning(f"Cleared {clear_result} event graphs")
            # Save Blueprint
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning("Saved Blueprint")
            return True
        else:
            unreal.log_warning("Reparent returned false (may already be reparented)")
            return True  # Continue anyway
    except Exception as e:
        unreal.log_error(f"Reparent error: {e}")
        return False


def resave_montages():
    """Resave all montages that use ANS_ToggleChaosField."""
    montages = [
        "/Game/SoulslikeFramework/Demo/_Animations/Combat/OneHanded/AM_SLF_1h_Light_L_Greystone",
        "/Game/SoulslikeFramework/Demo/_Animations/Combat/OneHanded/AM_SLF_1h_Light_R_Greystone",
        "/Game/SoulslikeFramework/Demo/_Animations/Combat/TwoHanded/AM_SLF_2h_Light_Kwang",
        "/Game/SoulslikeFramework/Demo/_Animations/Combat/DualWield/AM_SLF_DualWield_Light_Countess",
        # Dodges
        "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Backstep",
        "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_B",
        "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_BL",
        "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_BR",
        "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_F",
        "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_FL",
        "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_FR",
        "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_L",
        "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_R",
    ]

    unreal.log_warning("=== Resaving Montages ===")

    success = 0
    fail = 0

    for path in montages:
        montage = unreal.load_asset(path)
        if not montage:
            unreal.log_error(f"Could not load: {path}")
            fail += 1
            continue

        try:
            saved = unreal.EditorAssetLibrary.save_asset(path)
            if saved:
                unreal.log_warning(f"OK: {path}")
                success += 1
            else:
                unreal.log_error(f"FAIL: {path}")
                fail += 1
        except Exception as e:
            unreal.log_error(f"ERROR {path}: {e}")
            fail += 1

    unreal.log_warning(f"Montages: {success} saved, {fail} failed")


def main():
    unreal.log_warning("=== Fixing ANS_ToggleChaosField ===")

    # Step 1: Reparent Blueprint
    reparent_if_needed()

    # Step 2: Resave montages
    resave_montages()

    unreal.log_warning("=== DONE ===")


if __name__ == "__main__":
    main()
