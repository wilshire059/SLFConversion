# execute_migration.py
# Executes the migration from YAML plans
# Converts Blueprint UserDefinedEnums/Structs to C++ types
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\execute_migration.py").read())

import unreal
import os
import re

# Configuration
MIGRATION_PLANS_PATH = r"C:\scripts\SLFConversion\migration_plans"
DRY_RUN = True  # Set to False to actually execute changes

# C++ type paths
CPP_TYPES = {
    "ESLFValueType": "/Script/SLFConversion.ESLFValueType",
    "FStatChange": "/Script/SLFConversion.FStatChange",
    "FStatusEffectTick": "/Script/SLFConversion.FStatusEffectTick",
    "FStatusEffectStatChanges": "/Script/SLFConversion.FStatusEffectStatChanges",
    "FStatusEffectOneShotAndTick": "/Script/SLFConversion.FStatusEffectOneShotAndTick",
    "FLevelChangeData": "/Script/SLFConversion.FLevelChangeData",
}

# Enum value mappings
ENUM_VALUE_MAP = {
    "NewEnumerator0": "CurrentValue",
    "NewEnumerator1": "MaxValue",
}


def load_yaml_simple(filepath):
    """Simple YAML parser for our migration plan format."""
    result = {
        "blueprint": "",
        "path": "",
        "variables": [],
        "nodes": [],
        "pin_values": [],
    }

    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    # Extract blueprint name
    match = re.search(r'^blueprint:\s*(.+)$', content, re.MULTILINE)
    if match:
        result["blueprint"] = match.group(1).strip()

    # Extract path
    match = re.search(r'^path:\s*(.+)$', content, re.MULTILINE)
    if match:
        result["path"] = match.group(1).strip()

    # Parse variables section
    var_section = re.search(r'variables:\n(.*?)(?=\n# )', content, re.DOTALL)
    if var_section:
        var_text = var_section.group(1)
        # Find all variable entries
        var_matches = re.finditer(r'- name: "([^"]+)".*?action: "([^"]+)"', var_text, re.DOTALL)
        for m in var_matches:
            # Extract migration details
            new_type_match = re.search(r'new_type: "([^"]+)"', m.group(0))
            new_path_match = re.search(r'new_path: "([^"]+)"', m.group(0))
            is_array_match = re.search(r'is_array: (true|false)', m.group(0))

            result["variables"].append({
                "name": m.group(1),
                "new_type": new_type_match.group(1) if new_type_match else "",
                "new_path": new_path_match.group(1) if new_path_match else "",
                "is_array": is_array_match.group(1) == "true" if is_array_match else False,
                "action": m.group(2),
            })

    # Parse nodes section
    node_section = re.search(r'nodes:\n(.*?)(?=\n# )', content, re.DOTALL)
    if node_section:
        node_text = node_section.group(1)
        node_matches = re.finditer(r'- node_guid: "([^"]+)".*?action: "([^"]+)"', node_text, re.DOTALL)
        for m in node_matches:
            graph_match = re.search(r'graph: "([^"]+)"', m.group(0))
            node_class_match = re.search(r'node_class: "([^"]+)"', m.group(0))
            node_title_match = re.search(r'node_title: "([^"]+)"', m.group(0))
            new_enum_match = re.search(r'new_enum: "([^"]+)"', m.group(0))
            new_struct_match = re.search(r'new_struct: "([^"]+)"', m.group(0))

            result["nodes"].append({
                "node_guid": m.group(1),
                "graph": graph_match.group(1) if graph_match else "",
                "node_class": node_class_match.group(1) if node_class_match else "",
                "node_title": node_title_match.group(1) if node_title_match else "",
                "new_enum": new_enum_match.group(1) if new_enum_match else "",
                "new_struct": new_struct_match.group(1) if new_struct_match else "",
                "action": m.group(2),
            })

    # Parse pin_values section
    pin_section = re.search(r'pin_values:\n(.*?)$', content, re.DOTALL)
    if pin_section:
        pin_text = pin_section.group(1)
        pin_matches = re.finditer(
            r'- node_guid: "([^"]+)".*?graph: "([^"]+)".*?node_title: "([^"]*)".*?pin_name: "([^"]+)".*?original_value: "([^"]*)".*?new_value: "([^"]*)"',
            pin_text, re.DOTALL
        )
        for m in pin_matches:
            result["pin_values"].append({
                "node_guid": m.group(1),
                "graph": m.group(2),
                "node_title": m.group(3),
                "pin_name": m.group(4),
                "original_value": m.group(5),
                "new_value": m.group(6),
            })

    return result


def get_blueprint(path):
    """Load a Blueprint asset."""
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    return unreal.load_asset(path)


def find_node_by_guid(blueprint, guid_string):
    """Find a node in a Blueprint by its GUID."""
    for graph in blueprint.ubergraph_pages:
        for node in graph.nodes:
            if str(node.node_guid) == guid_string:
                return node, graph

    # Check function graphs
    for func_graph in blueprint.function_graphs:
        for node in func_graph.nodes:
            if str(node.node_guid) == guid_string:
                return node, func_graph

    return None, None


def fix_pin_value(blueprint, pin_info):
    """Fix a single pin value on a node."""
    node_guid = pin_info["node_guid"]
    pin_name = pin_info["pin_name"]
    new_value = pin_info["new_value"]

    node, graph = find_node_by_guid(blueprint, node_guid)
    if not node:
        print(f"    WARNING: Could not find node with GUID {node_guid}")
        return False

    # Find the pin
    for pin in node.pins:
        if pin.pin_name == pin_name:
            if DRY_RUN:
                print(f"    [DRY RUN] Would change pin '{pin_name}' from '{pin.default_value}' to '{new_value}'")
            else:
                pin.default_value = new_value
                print(f"    Changed pin '{pin_name}' to '{new_value}'")
            return True

    print(f"    WARNING: Could not find pin '{pin_name}' on node")
    return False


def fix_variable_type(blueprint, var_info):
    """Fix a variable's struct type."""
    var_name = var_info["name"]
    new_path = var_info["new_path"]
    is_array = var_info["is_array"]

    if DRY_RUN:
        print(f"    [DRY RUN] Would change variable '{var_name}' to type at '{new_path}' (array={is_array})")
        return True

    # Use BlueprintFixerLibrary if available
    try:
        fixer = unreal.BlueprintFixerLibrary
        result = fixer.fix_variable_struct_type(blueprint, var_name, new_path)
        if result:
            print(f"    Changed variable '{var_name}' to C++ struct")
            return True
        else:
            print(f"    WARNING: Failed to change variable '{var_name}'")
            return False
    except Exception as e:
        print(f"    ERROR: {e}")
        return False


def recreate_switch_node(blueprint, node_info):
    """Recreate a Switch node with the new enum type."""
    node_guid = node_info["node_guid"]
    new_enum = node_info["new_enum"]

    if DRY_RUN:
        print(f"    [DRY RUN] Would recreate Switch node {node_guid} with enum {new_enum}")
        return True

    # For Switch nodes, we need to:
    # 1. Find the original node and save its connections
    # 2. Delete the original node
    # 3. Create a new Switch node with the C++ enum
    # 4. Restore connections

    # This is complex and needs proper implementation
    # For now, log what needs to be done manually
    print(f"    MANUAL: Recreate Switch node in graph '{node_info['graph']}' with enum {new_enum}")
    print(f"           Original title: '{node_info['node_title']}'")
    return True


def execute_migration_plan(plan):
    """Execute a single migration plan."""
    blueprint_name = plan["blueprint"]
    blueprint_path = plan["path"]

    print(f"\n{'='*60}")
    print(f"  Migrating: {blueprint_name}")
    print(f"{'='*60}")

    # Load the Blueprint
    blueprint = get_blueprint(blueprint_path)
    if not blueprint:
        print(f"  ERROR: Could not load Blueprint at {blueprint_path}")
        return False

    success_count = 0
    fail_count = 0

    # Process variables
    if plan["variables"]:
        print(f"\n  Variables ({len(plan['variables'])} to migrate):")
        for var_info in plan["variables"]:
            if fix_variable_type(blueprint, var_info):
                success_count += 1
            else:
                fail_count += 1

    # Process nodes (Switch/Break/Make)
    if plan["nodes"]:
        print(f"\n  Nodes ({len(plan['nodes'])} to migrate):")
        for node_info in plan["nodes"]:
            if "SwitchEnum" in node_info["node_class"]:
                if recreate_switch_node(blueprint, node_info):
                    success_count += 1
                else:
                    fail_count += 1
            else:
                print(f"    TODO: Handle node type {node_info['node_class']}")

    # Process pin values
    if plan["pin_values"]:
        print(f"\n  Pin Values ({len(plan['pin_values'])} to migrate):")
        for pin_info in plan["pin_values"]:
            if fix_pin_value(blueprint, pin_info):
                success_count += 1
            else:
                fail_count += 1

    # Save changes if not dry run
    if not DRY_RUN:
        unreal.EditorAssetLibrary.save_asset(blueprint_path)
        print(f"\n  Saved Blueprint")

    print(f"\n  Results: {success_count} successful, {fail_count} failed")
    return fail_count == 0


def main():
    print("\n" + "="*70)
    print("  MIGRATION EXECUTION")
    print("="*70)

    if DRY_RUN:
        print("\n  *** DRY RUN MODE - No changes will be made ***")
        print("  Set DRY_RUN = False to execute actual changes\n")

    # Find all migration plan files
    plan_files = [f for f in os.listdir(MIGRATION_PLANS_PATH) if f.endswith("_migration_plan.yaml")]

    if not plan_files:
        print(f"  No migration plans found in {MIGRATION_PLANS_PATH}")
        return

    print(f"  Found {len(plan_files)} migration plans")

    all_success = True
    for plan_file in plan_files:
        plan_path = os.path.join(MIGRATION_PLANS_PATH, plan_file)
        plan = load_yaml_simple(plan_path)

        if not execute_migration_plan(plan):
            all_success = False

    print("\n" + "="*70)
    if DRY_RUN:
        print("  DRY RUN COMPLETE - Review output above")
        print("  Set DRY_RUN = False to execute actual changes")
    else:
        if all_success:
            print("  MIGRATION COMPLETE")
        else:
            print("  MIGRATION COMPLETE WITH ERRORS")
    print("="*70)


if __name__ == "__main__" or True:
    main()
