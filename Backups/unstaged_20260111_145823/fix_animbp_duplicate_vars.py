# fix_animbp_duplicate_vars.py
# Remove duplicate Blueprint variables that conflict with C++ UPROPERTY

import unreal

def log(msg):
    unreal.log_warning(f"[FixDupeVars] {msg}")

def main():
    log("=" * 60)
    log("Removing Duplicate Blueprint Variables")
    log("=" * 60)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log(f"[ERROR] Failed to load AnimBP")
        return

    # Variables to remove (these exist in C++ already)
    vars_to_remove = [
        "LeftHandOverlayState",
        "RightHandOverlayState",
        "AnimDataAsset",
        # Also check for any other duplicates
    ]

    log("Current Status:")
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"  Parent: {parent}")

    # Get list of Blueprint variables
    log("")
    log("Checking for Blueprint variables to remove...")

    # Use remove_blueprint_variable for each conflicting variable
    removed_count = 0
    for var_name in vars_to_remove:
        success = unreal.SLFAutomationLibrary.remove_blueprint_variable(bp, var_name)
        if success:
            log(f"  [REMOVED] {var_name}")
            removed_count += 1
        else:
            log(f"  [SKIP] {var_name} - not found or couldn't remove")

    log(f"")
    log(f"Removed {removed_count} variables")

    # Save and recompile
    if removed_count > 0:
        unreal.EditorAssetLibrary.save_asset(animbp_path)
        log("[OK] Saved AnimBP")

    # Reload and check compile status
    bp = unreal.load_asset(animbp_path)
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log("")
    log("Compile Status:")
    for line in errors.split('\n')[:20]:
        if line.strip():
            log(f"  {line}")

    log("=" * 60)

if __name__ == "__main__":
    main()
