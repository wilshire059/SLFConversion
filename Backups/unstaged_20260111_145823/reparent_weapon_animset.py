"""
reparent_weapon_animset.py
Reparent PDA_WeaponAnimset Blueprint to C++ UPDA_WeaponAnimset class
"""
import unreal

def log(msg):
    unreal.log_warning(f"[REPARENT_ANIMSET] {msg}")

def main():
    log("=" * 70)
    log("REPARENTING PDA_WeaponAnimset TO C++ CLASS")
    log("=" * 70)

    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Data/WeaponAnimsets/PDA_WeaponAnimset"
    bp = unreal.load_asset(bp_path)
    if not bp:
        log(f"[ERROR] Could not load Blueprint: {bp_path}")
        return

    log(f"Loaded Blueprint: {bp.get_name()}")

    # Load the C++ parent class
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_WeaponAnimset")
    if not cpp_class:
        log(f"[ERROR] Could not load C++ class UPDA_WeaponAnimset")
        return

    log(f"C++ class: {cpp_class.get_name()}")

    # Reparent
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if result:
            log(f"[OK] Reparented successfully")
        else:
            log(f"[WARN] Reparent returned False")
    except Exception as e:
        log(f"[ERROR] Reparent failed: {e}")
        return

    # Compile and save
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log(f"[OK] Compiled")
    except Exception as e:
        log(f"[WARN] Compile issue: {e}")

    try:
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        log(f"[OK] Saved")
    except Exception as e:
        log(f"[ERROR] Save failed: {e}")

    log(f"\n=== REPARENT COMPLETE ===")
    log(f"DA_Animset_LightSword and other instances should now Cast to UPDA_WeaponAnimset")

if __name__ == "__main__":
    main()
