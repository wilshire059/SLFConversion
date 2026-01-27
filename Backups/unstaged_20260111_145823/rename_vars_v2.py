# rename_vars_v2.py
# Rename variables with "?" suffix using available UE5 Python APIs
# Uses replace_variable_references approach

import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/rename_v2_log.txt"

# Clear output file
os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
with open(OUTPUT_FILE, "w") as f:
    f.write("")

def log(msg):
    print(f"[RenameV2] {msg}")
    unreal.log(f"[RenameV2] {msg}")
    with open(OUTPUT_FILE, "a") as f:
        f.write(f"{msg}\n")

def log_error(msg):
    print(f"[RenameV2] ERROR: {msg}")
    unreal.log_error(f"[RenameV2] {msg}")
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

def get_variable_guid(bp, var_name):
    """Get the GUID of a variable by name."""
    try:
        if hasattr(bp, 'new_variables'):
            new_vars = bp.get_editor_property('new_variables')
            for var in new_vars:
                if str(var.var_name) == var_name:
                    return var.var_guid
    except Exception as e:
        log_error(f"Error getting var guid: {e}")
    return None

def examine_blueprint(bp, bp_path):
    """Examine a Blueprint's variables in detail."""
    log(f"  Examining Blueprint: {bp.get_name()}")

    # Check if new_variables exists
    if hasattr(bp, 'new_variables'):
        try:
            new_vars = bp.get_editor_property('new_variables')
            log(f"  Found {len(new_vars)} new_variables")
            for i, var in enumerate(new_vars):
                var_name = str(var.var_name)
                log(f"    [{i}] Name: {var_name}")
                if "?" in var_name:
                    log(f"         ^ Contains '?' - needs renaming")
        except Exception as e:
            log_error(f"  Error examining new_variables: {e}")
    else:
        log(f"  No new_variables attribute")

    # Try generated_class properties
    try:
        gen_class = bp.generated_class
        if gen_class:
            log(f"  Generated class: {gen_class.get_name()}")
            props = list(gen_class.properties())
            log(f"  Properties count: {len(props)}")
            for prop in props[:20]:  # First 20
                prop_name = str(prop.get_name())
                if "?" in prop_name:
                    log(f"    FOUND '?': {prop_name}")
    except Exception as e:
        log_error(f"  Error examining generated_class: {e}")

def try_replace_references(bp, old_name, new_name):
    """Try to replace variable references."""
    try:
        # Use BlueprintEditorLibrary.replace_variable_references
        result = unreal.BlueprintEditorLibrary.replace_variable_references(bp, old_name, new_name)
        log(f"    replace_variable_references result: {result}")
        return result
    except Exception as e:
        log_error(f"    replace_variable_references failed: {e}")
        return False

def try_rename_via_property(bp, old_name, new_name):
    """Try to rename by modifying the variable descriptor directly."""
    try:
        if hasattr(bp, 'new_variables'):
            new_vars = bp.get_editor_property('new_variables')
            for var in new_vars:
                if str(var.var_name) == old_name:
                    # Try to set var_name directly
                    log(f"    Found variable, attempting direct rename...")
                    try:
                        var.set_editor_property('var_name', new_name)
                        log(f"    Direct rename succeeded")
                        return True
                    except Exception as e:
                        log(f"    Direct rename failed: {e}")

                    # Try setting via FName
                    try:
                        var.var_name = unreal.Name(new_name)
                        log(f"    FName assignment succeeded")
                        return True
                    except Exception as e:
                        log(f"    FName assignment failed: {e}")

                    return False
    except Exception as e:
        log_error(f"    Error in try_rename_via_property: {e}")
    return False

def process_blueprint(bp_path, renames):
    """Process a single Blueprint."""
    log("")
    log(f"Processing: {bp_path}")

    asset = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not asset:
        log_error(f"  Could not load asset")
        return False

    bp = asset
    log(f"  Loaded: {bp.get_name()} ({type(bp).__name__})")

    # Examine the blueprint
    examine_blueprint(bp, bp_path)

    # Try each rename
    success_count = 0
    for old_name, new_name in renames:
        log(f"")
        log(f"  Renaming: '{old_name}' -> '{new_name}'")

        # Method 1: Try replace_variable_references
        log(f"  Method 1: replace_variable_references")
        if try_replace_references(bp, old_name, new_name):
            success_count += 1
            continue

        # Method 2: Try direct property modification
        log(f"  Method 2: Direct property modification")
        if try_rename_via_property(bp, old_name, new_name):
            success_count += 1
            continue

        log_error(f"    All methods failed for '{old_name}'")

    # Compile after all renames
    log(f"")
    log(f"  Compiling Blueprint...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log(f"  Compiled successfully")
    except Exception as e:
        log_error(f"  Compilation error: {e}")

    # Save
    if success_count > 0:
        log(f"  Saving...")
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path)
            log(f"  Saved")
        except Exception as e:
            log_error(f"  Save error: {e}")

    return success_count == len(renames)

def main():
    log("=" * 60)
    log("RENAME VARIABLES V2 - Using replace_variable_references")
    log("=" * 60)

    total_success = 0
    total_attempted = 0

    for bp_path, renames in BLUEPRINTS_TO_FIX.items():
        total_attempted += len(renames)
        if process_blueprint(bp_path, renames):
            total_success += len(renames)

    log("")
    log("=" * 60)
    log(f"SUMMARY: {total_success}/{total_attempted} renames succeeded")
    log("=" * 60)
    log(f"Full log saved to: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
