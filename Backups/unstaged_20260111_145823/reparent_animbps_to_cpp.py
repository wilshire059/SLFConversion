# reparent_animbps_to_cpp.py
# Reparent AnimBPs to C++ AnimInstance classes so NativeUpdateAnimation() is called

import unreal

def log(msg):
    unreal.log_warning(f"[ReparentAnimBP] {msg}")

ANIMBP_REPARENT_MAP = {
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive":
        "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive",
}

def main():
    log("=" * 60)
    log("Reparenting AnimBPs to C++ Classes")
    log("=" * 60)

    for bp_path, cpp_class_path in ANIMBP_REPARENT_MAP.items():
        bp = unreal.load_asset(bp_path)
        if not bp:
            log(f"[ERROR] Failed to load: {bp_path}")
            continue

        name = bp_path.split("/")[-1]
        current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"{name}: Current parent = {current_parent}")

        if "SLFConversion" in current_parent:
            log(f"  [SKIP] Already reparented to C++")
            continue

        # Reparent to C++ class
        log(f"  Attempting reparent to {cpp_class_path}...")
        success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_class_path)
        if success:
            log(f"  [OK] Reparented successfully")
            # Save the asset
            unreal.EditorAssetLibrary.save_asset(bp_path)
            log(f"  [OK] Saved")

            # Verify new parent
            new_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
            log(f"  New parent = {new_parent}")
        else:
            log(f"  [FAIL] Reparent failed")

    log("=" * 60)
    log("Done!")
    log("=" * 60)

if __name__ == "__main__":
    main()
