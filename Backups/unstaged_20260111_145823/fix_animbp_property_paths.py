# fix_animbp_property_paths.py
# Fix Property Access paths that reference "?" suffix properties

import unreal

def log(msg):
    unreal.log_warning(f"[FIXPATH] {msg}")

# Property Access path fixes: old_path -> new_path
PATH_FIXES = [
    ("IsGuarding?", "IsGuarding"),
    # Add any other paths that need fixing
]

def main():
    log("=" * 70)
    log("FIXING ANIMBP PROPERTY ACCESS PATHS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Fix Property Access paths
    log("")
    log("=== FIXING PROPERTY ACCESS PATHS ===")
    total_fixed = 0
    for old_path, new_path in PATH_FIXES:
        fixed = unreal.SLFAutomationLibrary.fix_property_access_paths(bp, old_path, new_path)
        if fixed > 0:
            log(f"  [OK] Fixed '{old_path}' -> '{new_path}': {fixed} occurrences")
            total_fixed += fixed
        else:
            log(f"  [--] No occurrences of '{old_path}' found")

    log(f"  Total fixed: {total_fixed}")

    # Compile and save
    log("")
    log("=== COMPILING AND SAVING ===")
    success = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if success:
        log("[OK] Compiled and saved")
    else:
        log("[WARN] Compile may have issues")

    # Force save
    save_result = unreal.EditorAssetLibrary.save_asset(animbp_path, only_if_is_dirty=False)
    log(f"Save result: {save_result}")

    # Check for errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        log("")
        log("=== COMPILE STATUS ===")
        for line in errors.split('\n')[:30]:
            log(line)
    else:
        log("[OK] No compile errors!")

if __name__ == "__main__":
    main()
