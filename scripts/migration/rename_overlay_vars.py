# rename_overlay_vars.py
# Rename "_0" suffix Blueprint variables to match C++ property names
# This allows AnimGraph to read directly from C++ UPROPERTY without reflection

import unreal

def log(msg):
    unreal.log_warning(f"[RENAME] {msg}")

def main():
    log("=" * 70)
    log("RENAMING OVERLAY STATE VARIABLES TO MATCH C++ PROPERTIES")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Show current variables
    log("")
    log("=== CURRENT VARIABLES ===")
    current_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    for v in current_vars:
        log(f"  {v}")

    # Rename variables to match C++ UPROPERTY names
    # This removes the _0 suffix that was added during reparenting
    RENAMES = {
        "LeftHandOverlayState_0": "LeftHandOverlayState",
        "RightHandOverlayState_0": "RightHandOverlayState",
    }

    log("")
    log("=== RENAMING VARIABLES ===")

    for old_name, new_name in RENAMES.items():
        if old_name in current_vars:
            success = unreal.SLFAutomationLibrary.rename_variable(bp, old_name, new_name)
            if success:
                log(f"  [OK] Renamed '{old_name}' -> '{new_name}'")
            else:
                log(f"  [WARN] Failed to rename '{old_name}' -> '{new_name}'")
        else:
            log(f"  [--] Variable not found: '{old_name}'")

    # Compile
    log("")
    log("=== COMPILING ===")
    compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
    log(f"Compile result: {compile_result}")

    # Verify
    log("")
    log("=== VARIABLES AFTER RENAME ===")
    new_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    for v in new_vars:
        log(f"  {v}")

    # Check compile status
    log("")
    log("=== COMPILE STATUS ===")
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        for line in errors.split('\n')[:20]:
            log(line)
    else:
        log("[OK] No compile errors!")

    # Verify BlendListByEnum bindings updated
    log("")
    log("=== VERIFYING BLENDLISTBYENUM BINDINGS ===")
    diagnostic = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

    lines = diagnostic.split('\n')
    in_blend_section = False
    for line in lines:
        if 'BLEND POSES BY ENUM' in line:
            in_blend_section = True
        if in_blend_section:
            log(line)
            if line.strip() == '':
                in_blend_section = False

    # Save
    log("")
    log("=== SAVING ===")
    save_result = unreal.EditorAssetLibrary.save_asset(animbp_path, only_if_is_dirty=False)
    log(f"Save result: {save_result}")

    log("")
    log("=== SUMMARY ===")
    log("Blueprint variables renamed to match C++ UPROPERTY names.")
    log("AnimGraph K2Node_VariableGet nodes now read directly from C++ properties.")
    log("Reflection code in NativeUpdateAnimation() can now be removed.")

if __name__ == "__main__":
    main()
