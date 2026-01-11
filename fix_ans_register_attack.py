# fix_ans_register_attack.py
# Reparent ANS_RegisterAttackSequence Blueprint to C++ class
import unreal

def log(msg):
    unreal.log_warning(f"[FIX_ANS] {msg}")

def main():
    log("=" * 60)
    log("REPARENTING ANS_RegisterAttackSequence")
    log("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_RegisterAttackSequence"
    cpp_class_path = "/Script/SLFConversion.ANS_RegisterAttackSequence"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        log(f"[ERROR] Could not load: {bp_path}")
        return

    log(f"Loaded Blueprint: {bp.get_name()}")

    # Load the C++ class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        log(f"[ERROR] Could not load C++ class: {cpp_class_path}")
        return

    log(f"Target C++ class: {cpp_class.get_name()}")

    # Clear the Blueprint's graphs first (remove any Blueprint logic)
    log("")
    log("=== CLEARING BLUEPRINT GRAPHS ===")

    if hasattr(unreal, 'SLFAutomationLibrary'):
        # Clear EventGraph to remove all Blueprint logic
        result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
        if result:
            log("[OK] Cleared Blueprint logic")
        else:
            log("[WARN] Clear Blueprint logic returned false")
    else:
        log("[WARN] SLFAutomationLibrary not available, skipping graph clear")

    # Reparent to C++ class
    log("")
    log("=== REPARENTING ===")

    # Try using SLFAutomationLibrary reparent
    if hasattr(unreal, 'SLFAutomationLibrary'):
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_class_path)
        if result:
            log(f"[OK] Reparented to {cpp_class_path}")
        else:
            log(f"[ERROR] Reparent to {cpp_class_path} failed")
            # Try standard library as fallback
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            if result:
                log(f"[OK] Fallback reparent succeeded")
            else:
                log("[ERROR] Fallback reparent also failed")
                return
    else:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if result:
            log(f"[OK] Reparented to {cpp_class.get_name()}")
        else:
            log("[ERROR] Reparent failed")
            return

    # Compile
    log("")
    log("=== COMPILING ===")

    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    log("[OK] Compiled")

    # Save
    log("")
    log("=== SAVING ===")

    unreal.EditorAssetLibrary.save_asset(bp_path)
    log("[OK] Saved")

    log("")
    log("=== DONE ===")
    log(f"ANS_RegisterAttackSequence now inherits from C++ UANS_RegisterAttackSequence")
    log("The C++ class will handle NotifyBegin/End and call EventRegisterNextCombo")

if __name__ == "__main__":
    main()
