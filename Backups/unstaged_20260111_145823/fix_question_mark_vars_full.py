# fix_question_mark_vars_full.py
# Complete workflow to fix "?" suffix variables in backup project
#
# Steps:
# 1. Export current state (nodes, variables)
# 2. Rename all "?" variables
# 3. Refresh all nodes to update references
# 4. Compile and validate
# 5. Report any issues
#
# Run on BACKUP project (C:\scripts\bp_only)

import unreal
import json
import os

# All Blueprints that need "?" variable fixing
BLUEPRINTS_TO_FIX = {
    # AnimBlueprints
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive": [
        ("bIsAccelerating?", "bIsAccelerating"),
        ("bIsBlocking?", "bIsBlocking"),
        ("bIsFalling?", "bIsFalling"),
        ("IsResting?", "IsResting"),
    ],
    # Components
    "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager": [
        ("IsGuarding?", "IsGuarding"),
    ],
    # Add more as discovered
}

# Additional Blueprints to scan for "?" variables
BLUEPRINTS_TO_SCAN = [
    "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
    "/Game/SoulslikeFramework/Blueprints/AI/BTS_ChaseBounds",
    "/Game/SoulslikeFramework/Demo/_Animations/_Notify/ANS_AI_FistTrace",
    "/Game/SoulslikeFramework/Demo/_Animations/_Notify/ANS_HyperArmor",
    "/Game/SoulslikeFramework/Demo/_Animations/_Notify/ANS_InvincibilityFrame",
    "/Game/SoulslikeFramework/Demo/_Animations/_Notify/ANS_InputBuffer",
    "/Game/SoulslikeFramework/Demo/_Animations/_Notify/ANS_RegisterAttackSequence",
]

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache"

def log(msg):
    print(f"[FixVars] {msg}")
    unreal.log(f"[FixVars] {msg}")

def log_error(msg):
    print(f"[FixVars] ERROR: {msg}")
    unreal.log_error(f"[FixVars] {msg}")

def log_success(msg):
    print(f"[FixVars] OK: {msg}")
    unreal.log(f"[FixVars] OK: {msg}")

def find_question_mark_variables(bp):
    """Find all variables with '?' in their name."""
    variables = []

    try:
        if hasattr(bp, "new_variables"):
            new_vars = bp.get_editor_property("new_variables")
            for var_desc in new_vars:
                var_name = str(var_desc.var_name)
                if "?" in var_name:
                    variables.append((var_name, var_name.replace("?", "")))
    except Exception as e:
        log_error(f"  Error finding variables: {e}")

    return variables

def rename_variable_safe(bp, old_name, new_name):
    """
    Safely rename a variable, checking if the API is available.
    Returns (success, error_message)
    """
    try:
        # Method 1: Use BlueprintEditorLibrary.rename_member_variable
        if hasattr(unreal.BlueprintEditorLibrary, 'rename_member_variable'):
            result = unreal.BlueprintEditorLibrary.rename_member_variable(bp, old_name, new_name)
            return (result, None if result else "rename_member_variable returned False")

        # Method 2: Try using the Kismet library
        # This is a fallback that may not update all references
        log(f"    Warning: rename_member_variable not available, using fallback")

        # Get the variable GUID and try to rename via property modification
        if hasattr(bp, "new_variables"):
            new_vars = bp.get_editor_property("new_variables")
            for i, var_desc in enumerate(new_vars):
                if str(var_desc.var_name) == old_name:
                    # Found the variable - this approach won't work as new_variables is read-only
                    return (False, "Fallback method not implemented - need rename_member_variable")

        return (False, "Could not find variable to rename")

    except Exception as e:
        return (False, str(e))

def refresh_all_nodes(bp):
    """Refresh all nodes in the Blueprint to update references."""
    try:
        # Compile triggers a refresh of all nodes
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        return True
    except Exception as e:
        log_error(f"  Error refreshing nodes: {e}")
        return False

def check_blueprint_status(bp):
    """Check the compilation status of a Blueprint."""
    status_info = {
        "is_valid": True,
        "status": "Unknown",
        "errors": [],
        "warnings": [],
    }

    try:
        # Check if Blueprint has any errors by examining its status
        if hasattr(bp, 'status'):
            status = bp.get_editor_property('status')
            status_info["status"] = str(status)
            if status != unreal.BlueprintStatus.BS_UP_TO_DATE:
                status_info["is_valid"] = False

    except Exception as e:
        status_info["errors"].append(f"Error checking status: {e}")
        status_info["is_valid"] = False

    return status_info

def process_blueprint(asset_path, known_renames=None):
    """Process a single Blueprint."""
    result = {
        "path": asset_path,
        "loaded": False,
        "renames_attempted": [],
        "renames_succeeded": [],
        "renames_failed": [],
        "compiled": False,
        "status": None,
        "errors": [],
    }

    log(f"")
    log(f"Processing: {asset_path}")

    # Load the Blueprint
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        result["errors"].append("Could not load asset")
        return result
    result["loaded"] = True

    bp = None
    if isinstance(asset, unreal.Blueprint) or isinstance(asset, unreal.AnimBlueprint):
        bp = asset
    else:
        result["errors"].append(f"Not a Blueprint: {type(asset).__name__}")
        return result

    # Get renames to perform
    renames = known_renames if known_renames else []

    # Also scan for any "?" variables we might have missed
    discovered = find_question_mark_variables(bp)
    if discovered:
        log(f"  Discovered {len(discovered)} additional variables with '?'")
        for old, new in discovered:
            if (old, new) not in renames:
                renames.append((old, new))

    if not renames:
        log(f"  No variables to rename")
        result["compiled"] = refresh_all_nodes(bp)
        result["status"] = check_blueprint_status(bp)
        return result

    log(f"  Renaming {len(renames)} variables...")

    # Perform renames
    for old_name, new_name in renames:
        result["renames_attempted"].append((old_name, new_name))
        log(f"    '{old_name}' -> '{new_name}'")

        success, error = rename_variable_safe(bp, old_name, new_name)

        if success:
            result["renames_succeeded"].append((old_name, new_name))
            log_success(f"    Renamed")
        else:
            result["renames_failed"].append((old_name, new_name, error))
            log_error(f"    Failed: {error}")

    # Refresh/Compile
    log(f"  Compiling...")
    result["compiled"] = refresh_all_nodes(bp)

    # Check status
    result["status"] = check_blueprint_status(bp)

    # Save if any renames succeeded
    if result["renames_succeeded"]:
        try:
            unreal.EditorAssetLibrary.save_asset(asset_path)
            log(f"  Saved")
        except Exception as e:
            result["errors"].append(f"Failed to save: {e}")
            log_error(f"  Failed to save: {e}")

    return result

def main():
    log("=" * 60)
    log("FIX QUESTION MARK VARIABLES - FULL WORKFLOW")
    log("=" * 60)
    log("")
    log("This script renames all variables with '?' suffix")
    log("to prepare Blueprints for C++ migration.")
    log("")

    os.makedirs(OUTPUT_DIR, exist_ok=True)

    all_results = []

    # Process known Blueprints with specific renames
    log("PHASE 1: Processing known Blueprints...")
    for asset_path, renames in BLUEPRINTS_TO_FIX.items():
        result = process_blueprint(asset_path, renames)
        all_results.append(result)

    # Scan additional Blueprints for "?" variables
    log("")
    log("PHASE 2: Scanning additional Blueprints...")
    for asset_path in BLUEPRINTS_TO_SCAN:
        if asset_path not in BLUEPRINTS_TO_FIX:
            result = process_blueprint(asset_path)
            all_results.append(result)

    # Summary
    log("")
    log("=" * 60)
    log("SUMMARY")
    log("=" * 60)

    total_attempted = 0
    total_succeeded = 0
    total_failed = 0
    blueprints_with_issues = []

    for result in all_results:
        total_attempted += len(result["renames_attempted"])
        total_succeeded += len(result["renames_succeeded"])
        total_failed += len(result["renames_failed"])

        if result["errors"] or result["renames_failed"] or (result["status"] and not result["status"]["is_valid"]):
            blueprints_with_issues.append(result)

    log(f"Variables renamed: {total_succeeded}/{total_attempted}")
    log(f"Blueprints with issues: {len(blueprints_with_issues)}")

    if blueprints_with_issues:
        log("")
        log("ISSUES FOUND:")
        for result in blueprints_with_issues:
            log(f"  {result['path']}")
            for error in result.get("errors", []):
                log_error(f"    {error}")
            for old, new, error in result.get("renames_failed", []):
                log_error(f"    Failed to rename '{old}': {error}")
            if result["status"] and not result["status"]["is_valid"]:
                log_error(f"    Blueprint status: {result['status']['status']}")

    # Save results
    output_file = os.path.join(OUTPUT_DIR, "fix_question_mark_results.json")
    with open(output_file, "w") as f:
        json.dump(all_results, f, indent=2, default=str)

    log("")
    log(f"Full results saved to: {output_file}")

    if total_failed > 0:
        log("")
        log("=" * 60)
        log("MANUAL FIXES REQUIRED")
        log("=" * 60)
        log("Some variables could not be renamed automatically.")
        log("Please open these Blueprints in the editor and rename manually:")
        for result in all_results:
            for old, new, error in result.get("renames_failed", []):
                log(f"  - {result['path']}: '{old}' -> '{new}'")

    log("")
    log("=" * 60)

if __name__ == "__main__":
    main()
