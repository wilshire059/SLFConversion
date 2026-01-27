# verify_guard_sequences.py
# Check if weapon animsets have guard sequences set
# Run on SLFConversion project

import unreal

def log(msg):
    unreal.log_warning(str(msg))

ANIMSET_PATHS = [
    "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Shield",
    "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_LightSword",
    "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Greatsword",
    "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Katana",
]

def verify_guards():
    log("=" * 70)
    log("VERIFYING GUARD SEQUENCES IN WEAPON ANIMSETS")
    log("=" * 70)

    for animset_path in ANIMSET_PATHS:
        animset_name = animset_path.split("/")[-1]
        log(f"\n{'='*60}")
        log(f"[{animset_name}]")
        log(f"{'='*60}")

        animset = unreal.EditorAssetLibrary.load_asset(animset_path)
        if not animset:
            log(f"  ERROR: Could not load animset")
            continue

        try:
            # Check Guard_L
            guard_l = animset.get_editor_property("guard_l")
            if guard_l:
                log(f"  Guard_L: {guard_l.get_name()}")
            else:
                log(f"  Guard_L: NOT SET")

            # Check Guard_R
            guard_r = animset.get_editor_property("guard_r")
            if guard_r:
                log(f"  Guard_R: {guard_r.get_name()}")
            else:
                log(f"  Guard_R: NOT SET")

            # Check hit reactions
            guard_l_hit = animset.get_editor_property("guard_l_hit")
            guard_r_hit = animset.get_editor_property("guard_r_hit")
            log(f"  Guard_L_Hit: {guard_l_hit.get_name() if guard_l_hit else 'NOT SET'}")
            log(f"  Guard_R_Hit: {guard_r_hit.get_name() if guard_r_hit else 'NOT SET'}")

        except Exception as e:
            log(f"  ERROR: {e}")

if __name__ == "__main__":
    verify_guards()
    log("\n\nVERIFICATION COMPLETE")
