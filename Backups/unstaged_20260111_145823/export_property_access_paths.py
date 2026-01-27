# export_property_access_paths.py
# Exports all Property Access node paths from AnimGraphs
# These paths are stored as strings and need to match C++ property names
# Run on backup project BEFORE renaming to document all paths

import unreal
import json
import os
import re

ANIM_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
]

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache"

def log(msg):
    print(f"[ExportPaths] {msg}")
    unreal.log(f"[ExportPaths] {msg}")

def export_text_and_parse_paths(asset):
    """Use export_text to get the raw text and parse Property Access paths."""
    paths = []

    try:
        # Export the asset to text format
        export_text = ""
        exporter = unreal.AnimBlueprintExporterFactory()

        # Alternative: use the asset registry to get properties
        # For now, let's try to read from the generated class

        bp = asset
        if hasattr(bp, 'generated_class') and bp.generated_class:
            gen_class = bp.generated_class

            # Try to find Property Access bindings in the class default object
            cdo = gen_class.get_default_object()

            # Unfortunately, Property Access node data isn't easily accessible via Python
            # The paths are stored in FPropertyAccessPath which is internal

            # Let's try a different approach - iterate through all editor properties
            log("  Attempting to find Property Access data...")

    except Exception as e:
        log(f"  Error: {e}")

    return paths

def extract_paths_from_asset_export(asset_path):
    """Extract Property Access paths by examining the asset."""
    log(f"Extracting paths from: {asset_path}")

    paths = []
    variables_with_question = []

    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        log(f"  ERROR: Could not load asset")
        return paths, variables_with_question

    bp = asset

    # Get variables with "?" in name
    try:
        if hasattr(bp, "new_variables"):
            new_vars = bp.get_editor_property("new_variables")
            for var_desc in new_vars:
                var_name = str(var_desc.var_name)
                if "?" in var_name:
                    variables_with_question.append({
                        "name": var_name,
                        "clean_name": var_name.replace("?", ""),
                        "type": str(var_desc.var_type.pin_category) if hasattr(var_desc.var_type, "pin_category") else "unknown",
                    })
    except Exception as e:
        log(f"  Error getting variables: {e}")

    # Iterate through all graphs to find Property Access nodes
    try:
        all_graphs = unreal.BlueprintEditorLibrary.get_graphs(bp)
        for graph in all_graphs:
            graph_name = graph.get_name()

            for node in graph.nodes:
                node_class = node.get_class().get_name()

                if "PropertyAccess" in node_class:
                    path_info = {
                        "graph": graph_name,
                        "node_class": node_class,
                        "node_name": node.get_name(),
                        "properties": {},
                    }

                    # Try to get all properties that might contain paths
                    for prop in node.get_class().properties():
                        prop_name = str(prop.get_name())
                        try:
                            value = node.get_editor_property(prop_name)
                            if value is not None:
                                # Check if it looks like a path (contains property names)
                                value_str = str(value)
                                if value_str and len(value_str) < 500:  # Skip very long values
                                    path_info["properties"][prop_name] = value_str
                        except:
                            pass

                    paths.append(path_info)

    except Exception as e:
        log(f"  Error iterating graphs: {e}")

    log(f"  Found {len(paths)} Property Access nodes")
    log(f"  Found {len(variables_with_question)} variables with '?'")

    return paths, variables_with_question

def main():
    log("=" * 60)
    log("PROPERTY ACCESS PATH EXPORT")
    log("=" * 60)

    os.makedirs(OUTPUT_DIR, exist_ok=True)

    all_data = {}

    for asset_path in ANIM_BLUEPRINTS:
        paths, vars_with_q = extract_paths_from_asset_export(asset_path)
        all_data[asset_path] = {
            "property_access_nodes": paths,
            "variables_with_question_mark": vars_with_q,
        }

    # Also check the JSON exports we already have
    log("")
    log("Checking existing JSON exports for reference...")

    json_export_path = "C:/scripts/SLFConversion/Exports/BlueprintDNA_v2/AnimBlueprint/ABP_SoulslikeCharacter_Additive.json"
    if os.path.exists(json_export_path):
        with open(json_export_path, "r") as f:
            json_data = json.load(f)

        # Extract variables with "?"
        json_vars = []
        for var in json_data.get("Variables", {}).get("List", []):
            var_name = var.get("Name", "")
            if "?" in var_name:
                json_vars.append({
                    "name": var_name,
                    "clean_name": var_name.replace("?", ""),
                    "category": var.get("Category", ""),
                    "type": var.get("Type", {}).get("Category", "unknown"),
                })

        all_data["json_export_reference"] = {
            "variables_with_question_mark": json_vars,
        }

        log(f"  Found {len(json_vars)} variables with '?' in JSON export")

    # Save
    output_file = os.path.join(OUTPUT_DIR, "property_access_paths.json")
    with open(output_file, "w") as f:
        json.dump(all_data, f, indent=2, default=str)

    # Print summary
    log("")
    log("=" * 60)
    log("SUMMARY OF VARIABLES TO RENAME")
    log("=" * 60)

    if "json_export_reference" in all_data:
        for var in all_data["json_export_reference"]["variables_with_question_mark"]:
            log(f"  '{var['name']}' -> '{var['clean_name']}' (type: {var['type']})")

    log("")
    log(f"Results saved to: {output_file}")
    log("=" * 60)

if __name__ == "__main__":
    main()
