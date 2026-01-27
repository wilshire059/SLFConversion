"""
fix_weaponanimset_reparent.py
Properly reparent PDA_WeaponAnimset Blueprint to C++ UPDA_WeaponAnimset class
"""
import unreal

def log(msg):
    unreal.log_warning(f"[REPARENT_FIX] {msg}")

def main():
    log("=" * 70)
    log("FIXING PDA_WeaponAnimset REPARENTING")
    log("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Data/WeaponAnimsets/PDA_WeaponAnimset"

    # 1. Load the Blueprint
    log("\n=== STEP 1: Load Blueprint ===")
    bp = unreal.load_asset(bp_path)
    if not bp:
        log(f"[ERROR] Could not load Blueprint: {bp_path}")
        return

    log(f"Blueprint: {bp.get_name()}")
    log(f"Blueprint type: {type(bp).__name__}")

    # 2. Get the generated class
    log("\n=== STEP 2: Check Generated Class ===")
    gen_class = None
    if hasattr(bp, 'generated_class') and callable(bp.generated_class):
        gen_class = bp.generated_class()
    elif hasattr(bp, 'GeneratedClass'):
        gen_class = bp.GeneratedClass

    if gen_class:
        log(f"Generated class: {gen_class.get_name()}")
        log(f"Generated class path: {gen_class.get_path_name()}")

        # Check parent
        try:
            parent_class = gen_class.get_super_class() if hasattr(gen_class, 'get_super_class') else None
            if parent_class:
                log(f"Current parent class: {parent_class.get_name()}")
                log(f"Current parent path: {parent_class.get_path_name()}")
            else:
                log(f"Could not get parent class via get_super_class()")
        except Exception as e:
            log(f"Error getting parent: {e}")
    else:
        log(f"Could not get generated class")

    # 3. Load the C++ class
    log("\n=== STEP 3: Load C++ Class ===")
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_WeaponAnimset")
    if not cpp_class:
        log(f"[ERROR] Could not load C++ class UPDA_WeaponAnimset")
        log(f"  Check that the C++ class is properly exported with SLFCONVERSION_API")
        return

    log(f"C++ class: {cpp_class.get_name()}")
    log(f"C++ class path: {cpp_class.get_path_name()}")

    # 4. Check if already properly parented
    log("\n=== STEP 4: Check If Already Parented ===")
    if gen_class:
        try:
            is_child = gen_class.is_child_of(cpp_class)
            log(f"Is generated class child of C++ class? {is_child}")
            if is_child:
                log(f"[INFO] Blueprint is already parented to C++ class!")
        except Exception as e:
            log(f"is_child_of check failed: {e}")

    # 5. Reparent the Blueprint
    log("\n=== STEP 5: Reparent Blueprint ===")
    try:
        # Use BlueprintEditorLibrary to reparent
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        log(f"Reparent result: {result}")
        if result:
            log(f"[OK] Reparent succeeded")
        else:
            log(f"[WARN] Reparent returned False - may already be parented or have issues")
    except Exception as e:
        log(f"[ERROR] Reparent failed: {e}")
        return

    # 6. Compile the Blueprint
    log("\n=== STEP 6: Compile Blueprint ===")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log(f"[OK] Compiled")
    except Exception as e:
        log(f"[WARN] Compile issue: {e}")

    # 7. Save the Blueprint
    log("\n=== STEP 7: Save Blueprint ===")
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        log(f"[OK] Saved")
    except Exception as e:
        log(f"[ERROR] Save failed: {e}")

    # 8. Reload and verify
    log("\n=== STEP 8: Verify After Save ===")
    bp2 = unreal.load_asset(bp_path)
    if bp2:
        gen_class2 = None
        if hasattr(bp2, 'generated_class') and callable(bp2.generated_class):
            gen_class2 = bp2.generated_class()
        elif hasattr(bp2, 'GeneratedClass'):
            gen_class2 = bp2.GeneratedClass

        if gen_class2:
            log(f"Verified generated class: {gen_class2.get_name()}")
            try:
                is_child = gen_class2.is_child_of(cpp_class)
                log(f"Is now child of C++ class? {is_child}")
                if is_child:
                    log(f"[SUCCESS] Blueprint is now properly parented!")
                else:
                    log(f"[FAIL] Blueprint still not parented correctly")
            except Exception as e:
                log(f"Verification failed: {e}")

    # 9. Also check an animset data asset
    log("\n=== STEP 9: Check Animset Instance ===")
    animset_path = "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_LightSword"
    animset = unreal.load_asset(animset_path)
    if animset:
        log(f"Animset: {animset.get_name()}")
        animset_class = animset.get_class()
        log(f"Animset class: {animset_class.get_name()}")
        log(f"Animset class path: {animset_class.get_path_name()}")

        try:
            is_instance = animset_class.is_child_of(cpp_class)
            log(f"Is animset's class child of C++ class? {is_instance}")
        except Exception as e:
            log(f"Check failed: {e}")

    log("\n=== REPARENT FIX COMPLETE ===")

if __name__ == "__main__":
    main()
