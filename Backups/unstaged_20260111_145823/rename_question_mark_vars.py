# rename_question_mark_vars.py
# Renames all variables with "?" suffix to remove the "?"
# Uses Unreal's proper rename API to update all references
# Run on backup project BEFORE migration

import unreal
import json
import os

# Blueprints to process for variable renaming
BLUEPRINTS_TO_PROCESS = [
    # AnimBlueprints
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
    # Components with "?" variables
    "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
]

# Additional Blueprints that may have "?" variables (discovered from exports)
ADDITIONAL_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/AI/BTS_ChaseBounds",
    "/Game/SoulslikeFramework/Demo/_Animations/_Notify/ANS_AI_FistTrace",
]

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache"

def log(msg):
    print(f"[RenameVars] {msg}")
    unreal.log(f"[RenameVars] {msg}")

def get_blueprint_variables_with_question_mark(bp):
    """Get all variables that have '?' in their name."""
    variables = []

    try:
        # Get new variable descriptions from the Blueprint
        # This gives us the editable variable list
        if hasattr(bp, "new_variables"):
            new_vars = bp.get_editor_property("new_variables")
            for var_desc in new_vars:
                var_name = str(var_desc.var_name)
                if "?" in var_name:
                    variables.append({
                        "name": var_name,
                        "new_name": var_name.replace("?", ""),
                        "guid": str(var_desc.var_guid) if hasattr(var_desc, "var_guid") else "",
                    })
    except Exception as e:
        log(f"  Error getting variables: {e}")

    return variables

def rename_variable(bp, old_name, new_name):
    """Rename a variable using Unreal's proper API."""
    try:
        # Use BlueprintEditorLibrary to rename the variable
        # This should update all references automatically
        result = unreal.BlueprintEditorLibrary.rename_member_variable(bp, old_name, new_name)
        return result
    except Exception as e:
        log(f"    ERROR renaming {old_name}: {e}")
        return False

def process_blueprint(asset_path):
    """Process a Blueprint to rename all '?' variables."""
    log(f"Processing: {asset_path}")

    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        log(f"  ERROR: Could not load asset")
        return {"path": asset_path, "error": "Could not load", "renames": []}

    # Check if it's a Blueprint
    bp = None
    if isinstance(asset, unreal.Blueprint):
        bp = asset
    elif isinstance(asset, unreal.AnimBlueprint):
        bp = asset
    else:
        log(f"  ERROR: Not a Blueprint (type: {type(asset).__name__})")
        return {"path": asset_path, "error": f"Not a Blueprint: {type(asset).__name__}", "renames": []}

    result = {
        "path": asset_path,
        "renames": [],
        "error": None,
    }

    # Find variables with "?"
    vars_to_rename = get_blueprint_variables_with_question_mark(bp)
    log(f"  Found {len(vars_to_rename)} variables with '?'")

    for var_info in vars_to_rename:
        old_name = var_info["name"]
        new_name = var_info["new_name"]
        log(f"    Renaming: '{old_name}' -> '{new_name}'")

        success = rename_variable(bp, old_name, new_name)
        result["renames"].append({
            "old": old_name,
            "new": new_name,
            "success": success,
        })

        if success:
            log(f"      OK")
        else:
            log(f"      FAILED")

    # Save the Blueprint if any renames were made
    if result["renames"]:
        try:
            # Compile the Blueprint
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            log(f"  Compiled Blueprint")

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(asset_path)
            log(f"  Saved asset")
        except Exception as e:
            log(f"  ERROR saving: {e}")
            result["error"] = str(e)

    return result

def main():
    log("=" * 60)
    log("RENAME QUESTION MARK VARIABLES")
    log("=" * 60)
    log("")
    log("This script renames all variables with '?' suffix")
    log("to prepare for C++ migration.")
    log("")

    # Create output directory
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    all_results = []

    # Process main blueprints
    for asset_path in BLUEPRINTS_TO_PROCESS:
        result = process_blueprint(asset_path)
        all_results.append(result)

    # Process additional blueprints
    log("")
    log("Processing additional blueprints...")
    for asset_path in ADDITIONAL_BLUEPRINTS:
        result = process_blueprint(asset_path)
        all_results.append(result)

    # Summary
    log("")
    log("=" * 60)
    log("SUMMARY")
    log("=" * 60)

    total_renames = 0
    total_success = 0
    total_failed = 0

    for result in all_results:
        for rename in result["renames"]:
            total_renames += 1
            if rename["success"]:
                total_success += 1
            else:
                total_failed += 1

    log(f"Total variables renamed: {total_renames}")
    log(f"  Success: {total_success}")
    log(f"  Failed: {total_failed}")

    # Save results
    output_file = os.path.join(OUTPUT_DIR, "rename_results.json")
    with open(output_file, "w") as f:
        json.dump(all_results, f, indent=2, default=str)

    log(f"")
    log(f"Results saved to: {output_file}")
    log("=" * 60)

if __name__ == "__main__":
    main()
