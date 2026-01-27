"""
Check and fix character Blueprint parent classes.
Run in Unreal Editor: py fix_character_parent.py
"""
import unreal

def log(msg):
    unreal.log_warning(msg)

def reparent_blueprint(bp_path, expected_cpp_class_path, expected_cpp_name):
    """Reparent a single blueprint to C++ class."""
    bp_name = bp_path.split('/')[-1]
    log(f"")
    log(f"{'='*60}")
    log(f"Processing: {bp_name}")
    log(f"{'='*60}")

    bp = unreal.load_asset(bp_path)
    if not bp:
        log(f"[ERROR] Could not load {bp_path}")
        return False

    gen_class = bp.generated_class()
    if not gen_class:
        log("[ERROR] Could not get generated class")
        return False

    log(f"Generated class: {gen_class.get_name()}")
    log(f"Target parent: {expected_cpp_name}")

    # Load C++ class
    cpp_class = unreal.load_class(None, expected_cpp_class_path)
    if not cpp_class:
        log(f"[ERROR] Could not load {expected_cpp_class_path}")
        return False

    log(f"C++ class loaded: {cpp_class.get_name()}")

    # Check if already using the right class
    class_path = gen_class.get_path_name()
    if expected_cpp_name in class_path or f"Script/SLFConversion.{expected_cpp_name}" in class_path:
        log(f"[OK] May already be parented to C++ {expected_cpp_name}")
        # Still try to ensure it's correct

    # Reparent using SLFAutomationLibrary (takes string path, more reliable)
    log("Attempting reparent via SLFAutomationLibrary...")
    try:
        success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, expected_cpp_class_path)
        if success:
            log("[OK] Reparent successful")
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            log("[OK] Blueprint compiled")
            unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            log("[OK] Asset saved")
            return True
        else:
            log("[WARN] SLFAutomationLibrary.reparent_blueprint returned False")
            log("Trying BlueprintEditorLibrary as fallback...")
            success2 = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            if success2:
                log("[OK] Reparent successful via fallback")
                unreal.BlueprintEditorLibrary.compile_blueprint(bp)
                unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
                return True
            else:
                log("[ERROR] Both reparent methods failed")
                return False
    except Exception as e:
        log(f"[ERROR] Exception during reparent: {e}")
        return False

def main():
    log("=" * 70)
    log("FIXING CHARACTER BLUEPRINT PARENT CLASSES")
    log("=" * 70)

    # Check both character blueprints
    results = []

    # B_BaseCharacter -> SLFBaseCharacter (must be first since B_Soulslike_Character inherits from it)
    results.append(reparent_blueprint(
        "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter",
        "/Script/SLFConversion.SLFBaseCharacter",
        "SLFBaseCharacter"
    ))

    # B_Soulslike_Character -> SLFSoulslikeCharacter
    results.append(reparent_blueprint(
        "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
        "/Script/SLFConversion.SLFSoulslikeCharacter",
        "SLFSoulslikeCharacter"
    ))

    log("")
    log("=" * 70)
    if all(results):
        log("SUCCESS! All character Blueprints correctly parented.")
        log("")
        log("Restart PIE to test two-hand stance:")
        log("  - Gamepad: Hold Y + Press RT (right hand)")
        log("  - Gamepad: Hold Y + Press LT (left hand)")
        log("  - Keyboard: Hold E + Click LMB (right hand)")
        log("  - Keyboard: Hold E + Click RMB (left hand)")
    else:
        log("Some Blueprints could not be fixed. Check errors above.")
    log("=" * 70)

if __name__ == "__main__":
    main()
