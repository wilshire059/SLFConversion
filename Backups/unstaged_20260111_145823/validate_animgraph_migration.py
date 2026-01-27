# validate_animgraph_migration.py
# Validates AnimBlueprint after migration
# Compiles, checks for errors, and compares with pre-migration export
# Run on migrated project AFTER running rename_question_mark_vars.py

import unreal
import json
import os

# Target AnimBlueprints to validate
ANIM_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
]

# Components that were modified
COMPONENTS_TO_CHECK = [
    "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
]

CACHE_DIR = "C:/scripts/SLFConversion/migration_cache"

def log(msg):
    print(f"[ValidateAnimGraph] {msg}")
    unreal.log(f"[ValidateAnimGraph] {msg}")

def log_error(msg):
    print(f"[ValidateAnimGraph] ERROR: {msg}")
    unreal.log_error(f"[ValidateAnimGraph] {msg}")

def log_warning(msg):
    print(f"[ValidateAnimGraph] WARNING: {msg}")
    unreal.log_warning(f"[ValidateAnimGraph] {msg}")

def compile_and_get_errors(bp):
    """Compile a Blueprint and return any errors."""
    errors = []
    warnings = []

    try:
        # Compile the Blueprint
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)

        # Check compilation status
        # Unfortunately, Python API doesn't directly expose compilation errors
        # We'll check if the Blueprint is up to date and valid
        if hasattr(bp, "status"):
            status = bp.get_editor_property("status")
            if status != unreal.BlueprintStatus.BS_UP_TO_DATE:
                warnings.append(f"Blueprint status: {status}")

    except Exception as e:
        errors.append(f"Compilation exception: {str(e)}")

    return errors, warnings

def get_current_nodes(bp):
    """Get current nodes from the Blueprint."""
    nodes = []

    try:
        all_graphs = unreal.BlueprintEditorLibrary.get_graphs(bp)
        for graph in all_graphs:
            graph_name = graph.get_name()

            for node in graph.nodes:
                node_class = node.get_class().get_name()
                node_data = {
                    "graph": graph_name,
                    "class": node_class,
                    "name": node.get_name(),
                    "properties": {},
                }

                # Check for PropertyAccess nodes specifically
                if "PropertyAccess" in node_class:
                    try:
                        # Check if the path property contains "?"
                        for prop in node.get_class().properties():
                            prop_name = str(prop.get_name())
                            if "path" in prop_name.lower():
                                try:
                                    value = node.get_editor_property(prop_name)
                                    if value:
                                        value_str = str(value)
                                        node_data["properties"][prop_name] = value_str
                                        if "?" in value_str:
                                            node_data["has_question_mark_path"] = True
                                except:
                                    pass
                    except:
                        pass

                nodes.append(node_data)

    except Exception as e:
        log_error(f"Error getting nodes: {e}")

    return nodes

def validate_blueprint(asset_path, before_data=None):
    """Validate a Blueprint after migration."""
    log(f"Validating: {asset_path}")

    result = {
        "path": asset_path,
        "loaded": False,
        "compiled": False,
        "errors": [],
        "warnings": [],
        "nodes_with_issues": [],
        "question_mark_paths": [],
    }

    # Load the asset
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        result["errors"].append("Could not load asset")
        return result
    result["loaded"] = True

    # Get the Blueprint
    bp = None
    if isinstance(asset, unreal.Blueprint) or isinstance(asset, unreal.AnimBlueprint):
        bp = asset
    else:
        result["errors"].append(f"Not a Blueprint: {type(asset).__name__}")
        return result

    # Compile and check for errors
    log(f"  Compiling...")
    errors, warnings = compile_and_get_errors(bp)
    result["errors"].extend(errors)
    result["warnings"].extend(warnings)
    result["compiled"] = len(errors) == 0

    # Get current nodes
    log(f"  Checking nodes...")
    current_nodes = get_current_nodes(bp)

    # Check for nodes with "?" in paths
    for node in current_nodes:
        if node.get("has_question_mark_path"):
            result["question_mark_paths"].append({
                "graph": node["graph"],
                "class": node["class"],
                "name": node["name"],
                "properties": node["properties"],
            })

    # Compare with before data if available
    if before_data:
        log(f"  Comparing with pre-migration state...")

        before_vars = {v["name"]: v for v in before_data.get("variables", [])}
        before_var_count = len(before_vars)

        # Check that "?" variables were renamed
        for var_name, var_info in before_vars.items():
            if var_info.get("has_question_mark"):
                expected_new_name = var_name.replace("?", "")
                # Check if the new variable exists in current Blueprint
                try:
                    gen_class = bp.generated_class
                    found = False
                    for prop in gen_class.properties():
                        if str(prop.get_name()) == expected_new_name:
                            found = True
                            break
                    if not found:
                        result["warnings"].append(f"Variable '{var_name}' should be renamed to '{expected_new_name}' but not found")
                except:
                    pass

    log(f"  Errors: {len(result['errors'])}, Warnings: {len(result['warnings'])}")
    return result

def validate_component(asset_path):
    """Validate a component Blueprint."""
    log(f"Validating component: {asset_path}")

    result = {
        "path": asset_path,
        "loaded": False,
        "compiled": False,
        "errors": [],
        "warnings": [],
        "question_mark_vars": [],
    }

    # Load the asset
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        result["errors"].append("Could not load asset")
        return result
    result["loaded"] = True

    # Get the Blueprint
    bp = None
    if isinstance(asset, unreal.Blueprint):
        bp = asset
    else:
        result["errors"].append(f"Not a Blueprint: {type(asset).__name__}")
        return result

    # Check for remaining "?" variables
    try:
        if hasattr(bp, "new_variables"):
            new_vars = bp.get_editor_property("new_variables")
            for var_desc in new_vars:
                var_name = str(var_desc.var_name)
                if "?" in var_name:
                    result["question_mark_vars"].append(var_name)
                    result["warnings"].append(f"Variable still has '?': {var_name}")
    except Exception as e:
        result["errors"].append(f"Error checking variables: {e}")

    # Compile
    log(f"  Compiling...")
    errors, warnings = compile_and_get_errors(bp)
    result["errors"].extend(errors)
    result["warnings"].extend(warnings)
    result["compiled"] = len(errors) == 0

    log(f"  Errors: {len(result['errors'])}, Warnings: {len(result['warnings'])}")
    return result

def main():
    log("=" * 60)
    log("ANIMGRAPH MIGRATION VALIDATION")
    log("=" * 60)

    # Load pre-migration data if available
    before_file = os.path.join(CACHE_DIR, "animgraph_nodes_before.json")
    before_data = {}
    if os.path.exists(before_file):
        with open(before_file, "r") as f:
            before_data = json.load(f)
        log(f"Loaded pre-migration data: {len(before_data)} blueprints")
    else:
        log("No pre-migration data found (this is OK if running fresh)")

    all_results = {
        "anim_blueprints": [],
        "components": [],
        "summary": {
            "total_errors": 0,
            "total_warnings": 0,
            "question_mark_paths_remaining": 0,
        }
    }

    # Validate AnimBlueprints
    log("")
    log("Validating AnimBlueprints...")
    for asset_path in ANIM_BLUEPRINTS:
        bp_before = before_data.get(asset_path)
        result = validate_blueprint(asset_path, bp_before)
        all_results["anim_blueprints"].append(result)
        all_results["summary"]["total_errors"] += len(result["errors"])
        all_results["summary"]["total_warnings"] += len(result["warnings"])
        all_results["summary"]["question_mark_paths_remaining"] += len(result["question_mark_paths"])

    # Validate Components
    log("")
    log("Validating Components...")
    for asset_path in COMPONENTS_TO_CHECK:
        result = validate_component(asset_path)
        all_results["components"].append(result)
        all_results["summary"]["total_errors"] += len(result["errors"])
        all_results["summary"]["total_warnings"] += len(result["warnings"])

    # Print summary
    log("")
    log("=" * 60)
    log("VALIDATION SUMMARY")
    log("=" * 60)
    log(f"Total Errors: {all_results['summary']['total_errors']}")
    log(f"Total Warnings: {all_results['summary']['total_warnings']}")
    log(f"Property Access paths with '?' remaining: {all_results['summary']['question_mark_paths_remaining']}")

    if all_results["summary"]["total_errors"] == 0:
        log("")
        log("SUCCESS: No critical errors found!")
    else:
        log("")
        log("ISSUES FOUND - See details below:")
        for bp_result in all_results["anim_blueprints"]:
            if bp_result["errors"]:
                log(f"  {bp_result['path']}:")
                for err in bp_result["errors"]:
                    log_error(f"    {err}")
        for comp_result in all_results["components"]:
            if comp_result["errors"]:
                log(f"  {comp_result['path']}:")
                for err in comp_result["errors"]:
                    log_error(f"    {err}")

    if all_results["summary"]["question_mark_paths_remaining"] > 0:
        log("")
        log("Property Access nodes that still have '?' in paths:")
        for bp_result in all_results["anim_blueprints"]:
            for node in bp_result.get("question_mark_paths", []):
                log_warning(f"  Graph: {node['graph']}, Node: {node['name']}")
                for prop_name, prop_value in node["properties"].items():
                    log_warning(f"    {prop_name}: {prop_value}")

    # Save results
    output_file = os.path.join(CACHE_DIR, "validation_results.json")
    with open(output_file, "w") as f:
        json.dump(all_results, f, indent=2, default=str)

    log("")
    log(f"Full results saved to: {output_file}")
    log("=" * 60)

if __name__ == "__main__":
    main()
