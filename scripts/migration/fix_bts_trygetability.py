# fix_bts_trygetability.py
# Verify and fix BTS_TryGetAbility reparenting

import unreal

def log(msg):
    print(f"[FIX_BTS] {msg}")

def main():
    log("=" * 60)
    log("Checking BTS_TryGetAbility")
    log("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_TryGetAbility"
    cpp_parent = "/Script/SLFConversion.BTS_TryGetAbility"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load Blueprint at {bp_path}")
        return

    log(f"Blueprint loaded: {bp.get_name()}")

    # Get generated class to check parent
    gen_class = bp.generated_class()
    if gen_class:
        log(f"GeneratedClass: {gen_class.get_name()}")
        log(f"GeneratedClassPath: {gen_class.get_path_name()}")

    # Load C++ parent class
    cpp_class = unreal.load_class(None, cpp_parent)
    if not cpp_class:
        log(f"ERROR: Could not load C++ class {cpp_parent}")
        return

    log(f"C++ class found: {cpp_class.get_name()}")

    # Try to reparent
    log("")
    log("Attempting reparent...")

    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if result:
            log("Reparent successful!")
            # Save
            saved = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            if saved:
                log("Blueprint saved")
            else:
                log("Warning: Blueprint may not have saved")
        else:
            log("Reparent returned false (may already be correct parent)")
    except Exception as e:
        log(f"Reparent error: {e}")

    # Verify again
    log("")
    log("Verification after reparent:")
    bp2 = unreal.load_asset(bp_path)
    if bp2:
        gen2 = bp2.generated_class()
        if gen2:
            log(f"  GeneratedClass: {gen2.get_name()}")
            log(f"  Path: {gen2.get_path_name()}")

    log("")
    log("=" * 60)
    log("Done")

if __name__ == "__main__":
    main()
