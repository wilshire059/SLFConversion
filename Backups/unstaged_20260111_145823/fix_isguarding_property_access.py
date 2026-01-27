# fix_isguarding_property_access.py
# Fix the IsGuarding? Property Access path to IsGuarding

import unreal

def log(msg):
    unreal.log_warning(f"[FixIsGuarding] {msg}")

def main():
    log("=" * 60)
    log("Fixing IsGuarding? Property Access Path")
    log("=" * 60)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log(f"[ERROR] Failed to load AnimBP")
        return

    # Check current status
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log(f"Before fix - Compile Status:")
    for line in errors.split('\n')[:10]:
        if line.strip():
            log(f"  {line}")

    # Fix the Property Access path
    log("")
    log("Attempting to fix 'IsGuarding?' -> 'IsGuarding'...")
    fixed = unreal.SLFAutomationLibrary.fix_property_access_paths(bp, "IsGuarding?", "IsGuarding")
    log(f"Fixed {fixed} Property Access nodes")

    # Save the asset
    if fixed > 0:
        unreal.EditorAssetLibrary.save_asset(animbp_path)
        log(f"[OK] Saved AnimBP")
    else:
        log(f"[INFO] No changes needed or function not working")

    # Reload and verify
    bp = unreal.load_asset(animbp_path)
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log("")
    log(f"After fix - Compile Status:")
    for line in errors.split('\n')[:10]:
        if line.strip():
            log(f"  {line}")

    log("=" * 60)
    log("Done!")
    log("=" * 60)

if __name__ == "__main__":
    main()
