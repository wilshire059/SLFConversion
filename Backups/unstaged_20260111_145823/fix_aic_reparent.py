# fix_aic_reparent.py
# Reparent AIC_SoulslikeFramework Blueprint to C++ AI Controller class

import unreal

def log(msg):
    unreal.log_warning(f"[FIX_AIC] {msg}")

def main():
    log("=" * 70)
    log("REPARENTING AIC_SoulslikeFramework TO C++")
    log("=" * 70)

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_AI/Misc/AIC_SoulslikeFramework"
    bp = unreal.load_asset(bp_path)
    if not bp:
        log(f"[ERROR] Could not load Blueprint: {bp_path}")
        return

    log(f"Loaded Blueprint: {bp.get_name()}")

    # Clear Blueprint logic (event graphs, functions, etc.)
    log("=== CLEARING BLUEPRINT LOGIC ===")
    success = unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
    if success:
        log("[OK] Cleared Blueprint logic")
    else:
        log("[WARN] Could not clear logic (may already be empty)")

    # Reparent to C++ class
    log("=== REPARENTING ===")
    cpp_class_path = "/Script/SLFConversion.AIC_SoulslikeFramework"
    success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_class_path)
    if success:
        log(f"[OK] Reparented to {cpp_class_path}")
    else:
        log(f"[ERROR] Failed to reparent to {cpp_class_path}")
        return

    # Compile and save
    log("=== COMPILING ===")
    success = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if success:
        log("[OK] Compiled and saved")
    else:
        log("[WARN] Compile may have issues")

    # Check for errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("=== COMPILE STATUS ===")
        for line in errors.split('\n')[:20]:
            log(line)
    else:
        log("[OK] No compile errors!")

    log("")
    log("=" * 70)
    log("COMPLETE")
    log("=" * 70)

if __name__ == "__main__":
    main()
