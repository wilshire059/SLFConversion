# fix_animbp_remove_actual_dupes.py
# Remove ALL Blueprint variables using their ACTUAL names

import unreal

def log(msg):
    unreal.log_warning(f"[FixDupes] {msg}")

def main():
    log("=" * 60)
    log("Removing Blueprint Variables with ACTUAL names")
    log("=" * 60)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log(f"[ERROR] Failed to load AnimBP")
        return

    # Get current Blueprint variables and remove them ALL
    bp_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Blueprint has {len(bp_vars)} variables to remove:")
    for v in bp_vars:
        log(f"  - '{v}'")

    # Remove each variable by its ACTUAL name
    log("")
    log("Removing variables...")
    removed_count = 0
    for var_name in bp_vars:
        success = unreal.SLFAutomationLibrary.remove_variable(bp, var_name)
        if success:
            log(f"  [REMOVED] '{var_name}'")
            removed_count += 1
        else:
            log(f"  [FAILED] '{var_name}'")

    log(f"")
    log(f"Removed {removed_count} of {len(bp_vars)} variables")

    # Save the Blueprint
    if removed_count > 0:
        unreal.EditorAssetLibrary.save_asset(animbp_path)
        log("[OK] Saved AnimBP")

    # Reload and check
    bp = unreal.load_asset(animbp_path)
    bp_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Blueprint now has {len(bp_vars)} variables")

    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log("")
    log("Compile Status:")
    for line in errors.split('\n')[:20]:
        if line.strip():
            log(f"  {line}")

    log("=" * 60)

if __name__ == "__main__":
    main()
