# generate_migration_checklist.py
# Generates a detailed migration checklist for E_ValueType -> ESLFValueType
# Outputs exact locations that need changing in each Blueprint

import os
import json
from collections import defaultdict

# Configuration
JSON_EXPORTS_PATH = r"C:\scripts\SLFConversion_Migration\Backups\SoulsClean\Exports\BlueprintDNA"
ENUM_NAME = "E_ValueType"
ENUM_PATH = "/Game/SoulslikeFramework/Enums/E_ValueType"
OUTPUT_FILE = r"C:\scripts\SLFConversion\ENUM_MIGRATION_CHECKLIST.md"

# Results storage
migrations = defaultdict(lambda: {
    "asset_path": "",
    "asset_type": "",
    "function_params": [],      # Function input/output parameters
    "event_dispatcher_params": [],  # Event dispatcher parameters
    "variables": [],            # Blueprint variables
    "local_variables": [],      # Function local variables
    "pins": [],                 # Node pins with enum type
    "pin_default_values": [],   # Pins with NewEnumerator0/1 default values
})

def get_asset_game_path(json_path, data):
    """Extract the game path from JSON data."""
    if "AssetPath" in data:
        return data["AssetPath"]
    # Construct from filename
    basename = os.path.basename(json_path).replace(".json", "")
    return f"/Game/.../{basename}"

def get_asset_type(json_path):
    """Determine asset type from folder structure."""
    if "Component" in json_path:
        return "Component"
    elif "WidgetBlueprint" in json_path:
        return "Widget"
    elif "Animation" in json_path or "Notify" in json_path:
        return "AnimNotify"
    elif "Struct" in json_path:
        return "Struct"
    elif "Enum" in json_path:
        return "Enum"
    else:
        return "Blueprint"

def extract_graph_name(path):
    """Extract readable graph name from JSON path."""
    # Example: Logic.AllGraphs[0].Nodes[1].CollapsedGraph.Nodes[2]
    parts = []
    if "AllGraphs[" in path:
        # Try to get graph index
        import re
        match = re.search(r'AllGraphs\[(\d+)\]', path)
        if match:
            parts.append(f"Graph[{match.group(1)}]")
    if "CollapsedGraph" in path:
        parts.append("CollapsedGraph")
    return " > ".join(parts) if parts else "EventGraph"

def process_function_signatures(data, asset_key):
    """Process function signatures for enum parameters."""
    if "FunctionSignatures" not in data:
        return

    funcs = data["FunctionSignatures"].get("Functions", [])
    for func in funcs:
        func_name = func.get("Name", "Unknown")

        # Check inputs
        for i, inp in enumerate(func.get("Inputs", [])):
            type_info = inp.get("Type", {})
            if isinstance(type_info, dict):
                sub_obj = type_info.get("SubCategoryObject", "")
                sub_path = type_info.get("SubCategoryObjectPath", "")
            else:
                sub_obj = ""
                sub_path = str(type_info)

            if ENUM_NAME in str(sub_obj) or ENUM_NAME in str(sub_path):
                param_name = inp.get("Name", f"Input[{i}]")
                migrations[asset_key]["function_params"].append({
                    "function": func_name,
                    "param_name": param_name,
                    "direction": "Input",
                    "index": i
                })

        # Check outputs
        for i, out in enumerate(func.get("Outputs", [])):
            type_info = out.get("Type", {})
            if isinstance(type_info, dict):
                sub_obj = type_info.get("SubCategoryObject", "")
                sub_path = type_info.get("SubCategoryObjectPath", "")
            else:
                sub_obj = ""
                sub_path = str(type_info)

            if ENUM_NAME in str(sub_obj) or ENUM_NAME in str(sub_path):
                param_name = out.get("Name", f"Output[{i}]")
                migrations[asset_key]["function_params"].append({
                    "function": func_name,
                    "param_name": param_name,
                    "direction": "Output",
                    "index": i
                })

def process_event_dispatchers(data, asset_key):
    """Process event dispatchers for enum parameters."""
    if "EventDispatchers" not in data:
        return

    dispatchers = data["EventDispatchers"].get("List", [])
    for disp in dispatchers:
        disp_name = disp.get("Name", "Unknown")

        for i, param in enumerate(disp.get("Parameters", [])):
            type_info = param.get("Type", {})
            if isinstance(type_info, dict):
                sub_obj = type_info.get("SubCategoryObject", "")
                sub_path = type_info.get("SubCategoryObjectPath", "")
            else:
                sub_obj = ""
                sub_path = str(type_info)

            if ENUM_NAME in str(sub_obj) or ENUM_NAME in str(sub_path):
                param_name = param.get("Name", f"Param[{i}]")
                migrations[asset_key]["event_dispatcher_params"].append({
                    "dispatcher": disp_name,
                    "param_name": param_name,
                    "index": i
                })

def process_variables(data, asset_key):
    """Process Blueprint variables for enum type."""
    if "Variables" not in data:
        return

    var_list = data["Variables"].get("List", [])
    for var in var_list:
        var_name = var.get("Name", "Unknown")
        type_info = var.get("Type", {})

        if isinstance(type_info, dict):
            sub_obj = type_info.get("SubCategoryObject", "")
            sub_path = type_info.get("SubCategoryObjectPath", "")
        else:
            sub_obj = ""
            sub_path = str(type_info)

        if ENUM_NAME in str(sub_obj) or ENUM_NAME in str(sub_path):
            migrations[asset_key]["variables"].append({
                "name": var_name,
                "category": var.get("Category", "Default")
            })

def process_nodes_recursive(nodes, asset_key, graph_path=""):
    """Recursively process nodes for enum pins."""
    if not nodes:
        return

    for node_idx, node in enumerate(nodes):
        node_title = node.get("NodeTitle", "Unknown Node")
        node_class = node.get("NodeClass", "")
        node_guid = node.get("NodeGuid", "")

        # Process pins
        for pin in node.get("Pins", []):
            pin_name = pin.get("Name", "")
            pin_type = pin.get("Type", {})
            default_value = pin.get("DefaultValue", "")

            if isinstance(pin_type, dict):
                sub_obj = pin_type.get("SubCategoryObject", "")
                sub_path = pin_type.get("SubCategoryObjectPath", "")
            else:
                sub_obj = ""
                sub_path = ""

            # Check if pin type references the enum
            is_e_valuetype_pin = ENUM_NAME in str(sub_obj) or ENUM_NAME in str(sub_path)

            if is_e_valuetype_pin:
                migrations[asset_key]["pins"].append({
                    "graph": graph_path,
                    "node_title": node_title,
                    "node_guid": node_guid,
                    "pin_name": pin_name,
                    "direction": pin.get("Direction", ""),
                    "default_value": default_value
                })

            # Check for enum default values (NewEnumerator0/1) - ONLY if this is an E_ValueType pin
            # Other enums like E_TraceType, E_AI_States also use NewEnumerator0/1 values
            if is_e_valuetype_pin and default_value in ["NewEnumerator0", "NewEnumerator1"]:
                migrations[asset_key]["pin_default_values"].append({
                    "graph": graph_path,
                    "node_title": node_title,
                    "node_guid": node_guid,
                    "pin_name": pin_name,
                    "old_value": default_value,
                    "new_value": "CurrentValue" if default_value == "NewEnumerator0" else "MaxValue"
                })

        # Process collapsed graphs
        if "CollapsedGraph" in node:
            collapsed = node["CollapsedGraph"]
            collapsed_name = collapsed.get("GraphName", "Collapsed")
            sub_path = f"{graph_path} > {collapsed_name}" if graph_path else collapsed_name
            process_nodes_recursive(collapsed.get("Nodes", []), asset_key, sub_path)

def process_logic(data, asset_key):
    """Process all graphs in the Logic section."""
    if "Logic" not in data:
        return

    all_graphs = data["Logic"].get("AllGraphs", [])
    for graph_idx, graph in enumerate(all_graphs):
        graph_name = graph.get("GraphName", f"Graph[{graph_idx}]")
        nodes = graph.get("Nodes", [])
        process_nodes_recursive(nodes, asset_key, graph_name)

def process_struct_members(data, asset_key):
    """Process struct members for enum type."""
    if "Members" not in data:
        return

    for member in data["Members"]:
        member_name = member.get("Name", "Unknown")
        member_type = member.get("Type", "")

        if ENUM_NAME in str(member_type):
            migrations[asset_key]["variables"].append({
                "name": member_name,
                "category": "Struct Member",
                "note": "UserDefinedStruct member - may need struct migration"
            })

def process_json_file(filepath):
    """Process a single JSON file."""
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            data = json.load(f)

        asset_path = get_asset_game_path(filepath, data)
        asset_type = get_asset_type(filepath)

        # Skip the enum itself and duplicates
        if "E_ValueType.json" in filepath:
            return
        if "\\Other\\" in filepath:  # Skip duplicates in Other folder
            return

        asset_key = asset_path
        migrations[asset_key]["asset_path"] = asset_path
        migrations[asset_key]["asset_type"] = asset_type

        # Process different sections
        process_function_signatures(data, asset_key)
        process_event_dispatchers(data, asset_key)
        process_variables(data, asset_key)
        process_logic(data, asset_key)
        process_struct_members(data, asset_key)

    except json.JSONDecodeError:
        pass
    except Exception as e:
        print(f"Error processing {filepath}: {e}")

def calculate_priority(asset_key, data):
    """Calculate migration priority (lower = migrate first)."""
    # Structs first (they're used by everything)
    if data["asset_type"] == "Struct":
        return 0

    # Then B_Stat (core stat object)
    if "B_Stat" in asset_key and "Status" not in asset_key:
        return 1

    # Then AC_StatManager (manager component)
    if "AC_StatManager" in asset_key:
        return 2

    # Then other components
    if data["asset_type"] == "Component":
        return 3

    # Then actions and status effects
    if "B_Action" in asset_key or "B_StatusEffect" in asset_key or "B_Buff" in asset_key:
        return 4

    # Then anim notifies
    if data["asset_type"] == "AnimNotify":
        return 5

    # Then widgets
    if data["asset_type"] == "Widget":
        return 6

    # Everything else
    return 7

def generate_markdown():
    """Generate the migration checklist markdown file."""
    lines = []
    lines.append("# E_ValueType → ESLFValueType Migration Checklist")
    lines.append("")
    lines.append("Generated automatically from JSON exports.")
    lines.append("Migrate in the order shown (top to bottom).")
    lines.append("")
    lines.append("## Migration Order")
    lines.append("")
    lines.append("| Priority | Asset | Functions | Variables | Pins |")
    lines.append("|----------|-------|-----------|-----------|------|")

    # Sort by priority
    sorted_assets = sorted(
        [(k, v) for k, v in migrations.items() if any([
            v["function_params"], v["event_dispatcher_params"],
            v["variables"], v["pins"], v["pin_default_values"]
        ])],
        key=lambda x: (calculate_priority(x[0], x[1]), x[0])
    )

    for asset_key, data in sorted_assets:
        priority = calculate_priority(asset_key, data)
        func_count = len(data["function_params"]) + len(data["event_dispatcher_params"])
        var_count = len(data["variables"])
        pin_count = len(data["pins"])

        # Extract short name
        short_name = asset_key.split("/")[-1].split(".")[0]
        lines.append(f"| {priority} | {short_name} | {func_count} | {var_count} | {pin_count} |")

    lines.append("")
    lines.append("---")
    lines.append("")

    # Detailed instructions for each asset
    for asset_key, data in sorted_assets:
        short_name = asset_key.split("/")[-1].split(".")[0]

        lines.append(f"## {short_name}")
        lines.append("")
        lines.append(f"**Path:** `{data['asset_path']}`")
        lines.append(f"**Type:** {data['asset_type']}")
        lines.append("")

        # Function parameters
        if data["function_params"]:
            lines.append("### Function Parameters to Change")
            lines.append("")
            lines.append("| Function | Parameter | Direction |")
            lines.append("|----------|-----------|-----------|")
            for param in data["function_params"]:
                lines.append(f"| {param['function']} | {param['param_name']} | {param['direction']} |")
            lines.append("")
            lines.append("**Instructions:**")
            lines.append("1. Open the function in the Blueprint editor")
            lines.append("2. Select the function parameter")
            lines.append("3. In Details panel, change type from `E_ValueType_DEPRECATED` → `ESLFValueType`")
            lines.append("")

        # Event dispatcher parameters
        if data["event_dispatcher_params"]:
            lines.append("### Event Dispatcher Parameters to Change")
            lines.append("")
            lines.append("| Dispatcher | Parameter |")
            lines.append("|------------|-----------|")
            for param in data["event_dispatcher_params"]:
                lines.append(f"| {param['dispatcher']} | {param['param_name']} |")
            lines.append("")
            lines.append("**Instructions:**")
            lines.append("1. Find the Event Dispatcher in the My Blueprint panel")
            lines.append("2. Click to edit its signature")
            lines.append("3. Change the parameter type from `E_ValueType_DEPRECATED` → `ESLFValueType`")
            lines.append("")

        # Variables
        if data["variables"]:
            lines.append("### Variables to Change")
            lines.append("")
            lines.append("| Variable Name | Category |")
            lines.append("|---------------|----------|")
            for var in data["variables"]:
                note = var.get("note", "")
                cat = var["category"]
                if note:
                    cat = f"{cat} ({note})"
                lines.append(f"| {var['name']} | {cat} |")
            lines.append("")
            lines.append("**Instructions:**")
            lines.append("1. Select the variable in My Blueprint panel")
            lines.append("2. In Details panel, change Variable Type from `E_ValueType_DEPRECATED` → `ESLFValueType`")
            lines.append("")

        # Pins (grouped by graph)
        if data["pins"]:
            lines.append("### Pins to Reconnect")
            lines.append("")

            # Group by graph
            pins_by_graph = defaultdict(list)
            for pin in data["pins"]:
                pins_by_graph[pin["graph"]].append(pin)

            for graph, pins in pins_by_graph.items():
                lines.append(f"**Graph: {graph or 'EventGraph'}**")
                lines.append("")
                lines.append("| Node | Pin | Direction |")
                lines.append("|------|-----|-----------|")
                for pin in pins[:20]:  # Limit to 20 per graph
                    direction = "→" if "Output" in pin["direction"] else "←"
                    lines.append(f"| {pin['node_title'][:40]} | {pin['pin_name']} | {direction} |")
                if len(pins) > 20:
                    lines.append(f"| ... | ({len(pins) - 20} more) | |")
                lines.append("")

            lines.append("**Instructions:**")
            lines.append("1. After changing function signatures, pins will turn red")
            lines.append("2. Delete the broken connection")
            lines.append("3. Reconnect - the types should now match")
            lines.append("")

        # Pin default values
        if data["pin_default_values"]:
            lines.append("### Pin Default Values to Update")
            lines.append("")
            lines.append("| Node | Pin | Old Value | New Value |")
            lines.append("|------|-----|-----------|-----------|")
            for pv in data["pin_default_values"][:20]:
                lines.append(f"| {pv['node_title'][:30]} | {pv['pin_name']} | {pv['old_value']} | {pv['new_value']} |")
            if len(data["pin_default_values"]) > 20:
                lines.append(f"| ... | | ({len(data['pin_default_values']) - 20} more) | |")
            lines.append("")

        lines.append("---")
        lines.append("")

    return "\n".join(lines)

def main():
    print("=" * 70)
    print("  Generating Migration Checklist")
    print("=" * 70)

    # Find all JSON files
    json_files = []
    for root, dirs, files in os.walk(JSON_EXPORTS_PATH):
        for file in files:
            if file.endswith('.json'):
                json_files.append(os.path.join(root, file))

    print(f"\n  Scanning {len(json_files)} JSON files...")

    for i, filepath in enumerate(json_files):
        if (i + 1) % 100 == 0:
            print(f"  Processing {i + 1}/{len(json_files)}...")
        process_json_file(filepath)

    print(f"\n  Found {len(migrations)} assets with enum references")

    # Generate markdown
    print("\n  Generating checklist...")
    markdown = generate_markdown()

    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        f.write(markdown)

    print(f"\n  Checklist saved to: {OUTPUT_FILE}")
    print("\n" + "=" * 70)

    # Print summary
    total_funcs = sum(len(m["function_params"]) + len(m["event_dispatcher_params"]) for m in migrations.values())
    total_vars = sum(len(m["variables"]) for m in migrations.values())
    total_pins = sum(len(m["pins"]) for m in migrations.values())

    print(f"  Total function/dispatcher params: {total_funcs}")
    print(f"  Total variables: {total_vars}")
    print(f"  Total pins: {total_pins}")
    print("=" * 70)

if __name__ == "__main__":
    main()
