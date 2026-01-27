"""
Resave attack montages to pick up the reparented ANS_ToggleChaosField class.
After reparenting a Blueprint, montages that use the notify may need to be resaved.
"""
import unreal

def resave_montages():
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

    unreal.log_warning("=== Resaving Attack Montages ===")

    success_count = 0
    fail_count = 0

    for montage_path in montages:
        montage = unreal.load_asset(montage_path)
        if not montage:
            unreal.log_error(f"Could not load {montage_path}")
            fail_count += 1
            continue

        # Mark as dirty and save
        try:
            # Save the asset
            saved = unreal.EditorAssetLibrary.save_asset(montage_path)
            if saved:
                unreal.log_warning(f"OK: {montage_path}")
                success_count += 1
            else:
                unreal.log_error(f"FAILED to save: {montage_path}")
                fail_count += 1
        except Exception as e:
            unreal.log_error(f"ERROR saving {montage_path}: {e}")
            fail_count += 1

    unreal.log_warning(f"=== DONE: {success_count} saved, {fail_count} failed ===")

if __name__ == "__main__":
    resave_montages()
