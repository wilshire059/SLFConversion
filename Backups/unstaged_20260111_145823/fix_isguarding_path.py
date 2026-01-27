# fix_isguarding_path.py
# Fix the IsGuarding? -> IsGuarding property access path in AnimBP

import unreal

def log(msg):
    unreal.log_warning(f"[FIX] {msg}")

def main():
    log("=" * 70)
    log("FIX ISGUARDING PROPERTY ACCESS PATH")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"Loaded AnimBP: {animbp_path}")

    # Fix the property access path
    log("")
    log("Fixing 'IsGuarding?' -> 'IsGuarding'...")
    try:
        fixed_count = unreal.SLFAutomationLibrary.fix_property_access_paths(bp, "IsGuarding?", "IsGuarding")
        log(f"Fixed {fixed_count} property access path(s)")
    except Exception as e:
        log(f"ERROR: {e}")

    # Check compile status after fix
    log("")
    log("Checking compile status after fix...")
    try:
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        log(f"Compile status:")
        for line in errors.split('\n')[:10]:
            if line.strip():
                log(f"  {line}")
    except Exception as e:
        log(f"ERROR: {e}")

    # Save the blueprint
    log("")
    log("Saving AnimBP...")
    try:
        unreal.EditorAssetLibrary.save_asset(animbp_path)
        log("AnimBP saved successfully")
    except Exception as e:
        log(f"ERROR saving: {e}")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
