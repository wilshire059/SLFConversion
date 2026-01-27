"""
apply_animset_montages.py
Configure weapon animset data assets with attack montage references
"""
import unreal

def log(msg):
    unreal.log_warning(f"[ANIMSET_CONFIG] {msg}")

# Montage paths
MONTAGES = {
    "1h_light_r": "/Game/SoulslikeFramework/Demo/_Animations/Combat/OneHanded/AM_SLF_1h_Light_R_Greystone",
    "1h_light_l": "/Game/SoulslikeFramework/Demo/_Animations/Combat/OneHanded/AM_SLF_1h_Light_L_Greystone",
    "2h_light": "/Game/SoulslikeFramework/Demo/_Animations/Combat/TwoHanded/AM_SLF_2h_Light_Kwang",
    "dual_light": "/Game/SoulslikeFramework/Demo/_Animations/Combat/DualWield/AM_SLF_DualWield_Light_Countess",
    # Guard poses (AnimSequence, not Montage)
    "guard_r": "/Game/SoulslikeFramework/Demo/_Animations/Combat/Poses/AS_SLF_Guard_Pose_R",
    "guard_l": "/Game/SoulslikeFramework/Demo/_Animations/Combat/Poses/AS_SLF_Guard_Pose_L",
    "shield_guard": "/Game/SoulslikeFramework/Demo/_Animations/Combat/Poses/AS_SLF_Shield_Block_AO_L",
}

# Animset configurations
# For each animset, define which montages it should use
ANIMSET_CONFIGS = {
    "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_LightSword": {
        "one_h_light_combo_montage_r": MONTAGES["1h_light_r"],
        "one_h_light_combo_montage_l": MONTAGES["1h_light_l"],
        "two_h_light_combo_montage": MONTAGES["2h_light"],
        "one_h_heavy_combo_montage_r": MONTAGES["1h_light_r"],  # Use light for now
        "one_h_heavy_combo_montage_l": MONTAGES["1h_light_l"],
        "two_h_heavy_combo_montage": MONTAGES["2h_light"],
        "light_dual_wield_montage": MONTAGES["dual_light"],
        "heavy_dual_wield_montage": MONTAGES["dual_light"],
        "guard_r": MONTAGES["guard_r"],
        "guard_l": MONTAGES["guard_l"],
    },
    "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Greatsword": {
        "one_h_light_combo_montage_r": MONTAGES["2h_light"],  # Greatsword uses 2h
        "one_h_light_combo_montage_l": MONTAGES["2h_light"],
        "two_h_light_combo_montage": MONTAGES["2h_light"],
        "one_h_heavy_combo_montage_r": MONTAGES["2h_light"],
        "one_h_heavy_combo_montage_l": MONTAGES["2h_light"],
        "two_h_heavy_combo_montage": MONTAGES["2h_light"],
        "guard_r": MONTAGES["guard_r"],
        "guard_l": MONTAGES["guard_l"],
    },
    "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Katana": {
        "one_h_light_combo_montage_r": MONTAGES["1h_light_r"],
        "one_h_light_combo_montage_l": MONTAGES["1h_light_l"],
        "two_h_light_combo_montage": MONTAGES["2h_light"],
        "one_h_heavy_combo_montage_r": MONTAGES["1h_light_r"],
        "one_h_heavy_combo_montage_l": MONTAGES["1h_light_l"],
        "two_h_heavy_combo_montage": MONTAGES["2h_light"],
        "guard_r": MONTAGES["guard_r"],
        "guard_l": MONTAGES["guard_l"],
    },
    "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Shield": {
        # Shield doesn't attack, mainly for guard
        "guard_r": MONTAGES["guard_r"],
        "guard_l": MONTAGES["shield_guard"],
    },
    "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Unarmed": {
        "one_h_light_combo_montage_r": MONTAGES["1h_light_r"],
        "one_h_light_combo_montage_l": MONTAGES["1h_light_l"],
        "guard_r": MONTAGES["guard_r"],
        "guard_l": MONTAGES["guard_l"],
    },
}

def main():
    log("=" * 70)
    log("CONFIGURING WEAPON ANIMSETS WITH MONTAGE REFERENCES")
    log("=" * 70)

    success_count = 0
    fail_count = 0

    for animset_path, montage_config in ANIMSET_CONFIGS.items():
        log(f"\n--- {animset_path.split('/')[-1]} ---")

        animset = unreal.load_asset(animset_path)
        if not animset:
            log(f"  [ERROR] Could not load animset")
            fail_count += 1
            continue

        changes_made = 0

        for prop_name, montage_path in montage_config.items():
            try:
                # Load the montage/sequence
                montage = unreal.load_asset(montage_path)
                if not montage:
                    log(f"  [WARN] Could not load montage: {montage_path}")
                    continue

                # Try to set the property
                # Need to use the correct method for soft object ptrs
                animset.set_editor_property(prop_name, montage)
                log(f"  [OK] Set {prop_name} = {montage_path.split('/')[-1]}")
                changes_made += 1

            except Exception as e:
                log(f"  [WARN] Could not set {prop_name}: {e}")

        if changes_made > 0:
            # Save the asset
            try:
                unreal.EditorAssetLibrary.save_asset(animset_path, only_if_is_dirty=False)
                log(f"  Saved {changes_made} changes")
                success_count += 1
            except Exception as e:
                log(f"  [ERROR] Could not save: {e}")
                fail_count += 1
        else:
            log(f"  [WARN] No changes made")

    log(f"\n=== COMPLETE: {success_count} animsets configured, {fail_count} failed ===")

if __name__ == "__main__":
    main()
