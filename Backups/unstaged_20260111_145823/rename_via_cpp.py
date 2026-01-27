# rename_via_cpp.py
# Uses the C++ USLFAutomationLibrary::RenameVariable function
# to rename Blueprint variables with "?" suffix
#
# Run this on the BACKUP project (C:\scripts\bp_only\bp_only.uproject)
# AFTER building the main project with the RenameVariable functions

import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/rename_cpp_log.txt"
os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
with open(OUTPUT_FILE, "w") as f:
    f.write("")

def log(msg):
    print(f"[RenameCPP] {msg}")
    unreal.log(f"[RenameCPP] {msg}")
    with open(OUTPUT_FILE, "a") as f:
        f.write(f"{msg}\n")

def log_error(msg):
    print(f"[RenameCPP] ERROR: {msg}")
    unreal.log_error(f"[RenameCPP] {msg}")
    with open(OUTPUT_FILE, "a") as f:
        f.write(f"ERROR: {msg}\n")

# Blueprints and their variables to rename
BLUEPRINTS_TO_FIX = {
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive": [
        ("bIsAccelerating?", "bIsAccelerating"),
        ("bIsBlocking?", "bIsBlocking"),
        ("bIsFalling?", "bIsFalling"),
        ("IsResting?", "IsResting"),
    ],
    "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager": [
        ("IsGuarding?", "IsGuarding"),
    ],
}

def rename_variables_in_blueprint(asset_path, renames):
    """Use USLFAutomationLibrary to rename variables."""
    log(f"")
    log(f"Processing: {asset_path}")

    # Load the asset
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        log_error(f"  Could not load asset")
        return 0

    log(f"  Loaded: {asset.get_name()}")

    # Try to access USLFAutomationLibrary
    try:
        automation_lib = unreal.SLFAutomationLibrary
    except AttributeError:
        log_error(f"  USLFAutomationLibrary not available - build the main project first!")
        return 0

    success_count = 0
    for old_name, new_name in renames:
        log(f"  Renaming: '{old_name}' -> '{new_name}'")

        try:
            result = automation_lib.rename_variable(asset, old_name, new_name)
            if result:
                log(f"    OK")
                success_count += 1
            else:
                log_error(f"    Failed")
        except Exception as e:
            log_error(f"    Exception: {e}")

    # Save the asset if any renames succeeded
    if success_count > 0:
        try:
            # Compile and save
            unreal.SLFAutomationLibrary.compile_and_save(asset)
            log(f"  Saved asset")
        except Exception as e:
            log_error(f"  Save error: {e}")

    return success_count

def main():
    log("=" * 60)
    log("RENAME VARIABLES VIA C++ AUTOMATION")
    log("=" * 60)
    log("")
    log("This script uses USLFAutomationLibrary::RenameVariable")
    log("to properly rename Blueprint variables with '?' suffix.")
    log("")

    total_attempted = 0
    total_success = 0

    for asset_path, renames in BLUEPRINTS_TO_FIX.items():
        total_attempted += len(renames)
        total_success += rename_variables_in_blueprint(asset_path, renames)

    log("")
    log("=" * 60)
    log(f"SUMMARY: {total_success}/{total_attempted} renames succeeded")
    log("=" * 60)

    if total_success < total_attempted:
        log("")
        log("Some renames failed. Check the log for details.")
        log("You may need to rename these manually in the editor.")

    log(f"Full log saved to: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
