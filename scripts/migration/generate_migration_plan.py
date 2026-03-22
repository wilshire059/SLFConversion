# generate_migration_plan.py
# Generates a YAML migration plan from JSON exports
# Shows original nodes/pins and what will be changed
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\generate_migration_plan.py").read())

import json
import os

# Paths
JSON_EXPORT_PATH = r"C:\scripts\SLFConversion_Migration\Backups\SoulsClean\Exports\BlueprintDNA"
OUTPUT_PATH = r"C:\scripts\SLFConversion\migration_plans"

# Migration mappings
TYPE_MIGRATIONS = {
    # Enums
    "E_ValueType": {
        "new_type": "ESLFValueType",
        "new_path": "/Script/SLFConversion.ESLFValueType",
        "value_map": {
            "NewEnumerator0": "CurrentValue",
            "NewEnumerator1": "MaxValue",
            "Current Value": "CurrentValue",
            "Max Value": "MaxValue",
        }
    },
    # Structs
    "FStatChange": {
        "new_type": "FStatChange",
        "new_path": "/Script/SLFConversion.FStatChange",
        "old_path": "/Game/SoulslikeFramework/Structures/Stats/FStatChange.FStatChange"
    },
    "FLevelChangeData": {
        "new_type": "FLevelChangeData",
        "new_path": "/Script/SLFConversion.FLevelChangeData",
        "old_path": "/Game/SoulslikeFramework/Structures/Stats/FLevelChangeData.FLevelChangeData"
    },
    "FStatusEffectTick": {
        "new_type": "FStatusEffectTick",
        "new_path": "/Script/SLFConversion.FStatusEffectTick",
        "old_path": "/Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectTick.FStatusEffectTick"
    },
    "FStatusEffectStatChanges": {
        "new_type": "FStatusEffectStatChanges",
        "new_path": "/Script/SLFConversion.FStatusEffectStatChanges",
        "old_path": "/Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectStatChanges.FStatusEffectStatChanges"
    },
    "FStatusEffectOneShotAndTick": {
        "new_type": "FStatusEffectOneShotAndTick",
        "new_path": "/Script/SLFConversion.FStatusEffectOneShotAndTick",
        "old_path": "/Game/SoulslikeFramework/Structures/StatusEffects/FStatusEffectOneShotAndTick.FStatusEffectOneShotAndTick"
    },
}

# Blueprints to analyze
BLUEPRINTS_TO_ANALYZE = [
    ("Blueprint", "B_Stat"),
    ("Blueprint", "B_StatusEffect"),
    ("WidgetBlueprint", "W_HUD"),
    ("WidgetBlueprint", "W_LevelUp"),
]


def load_json(folder, name):
    """Load a JSON export file."""
    path = os.path.join(JSON_EXPORT_PATH, folder, f"{name}.json")
    if os.path.exists(path):
        with open(path, 'r', encoding='utf-8') as f:
            return json.load(f)
    return None


def check_type_needs_migration(type_info):
    """Check if a type info dict contains a type that needs migration."""
    if not type_info:
        return None

    sub_cat_path = type_info.get("SubCategoryObjectPath", "")
    sub_cat_obj = type_info.get("SubCategoryObject", "")

    for type_name, migration in TYPE_MIGRATIONS.items():
        if type_name in sub_cat_obj or type_name in sub_cat_path:
            return type_name

    return None


def analyze_variables(bp_data):
    """Analyze variables for migration needs."""
    migrations = []

    variables = bp_data.get("Variables", {}).get("List", [])
    for var in variables:
        var_name = var.get("Name", "")
        var_type = var.get("Type", {})

        needs_migration = check_type_needs_migration(var_type)
        if needs_migration:
            migrations.append({
                "type": "variable",
                "name": var_name,
                "category": var_type.get("Category", ""),
                "original_type": var_type.get("SubCategoryObject", ""),
                "original_path": var_type.get("SubCategoryObjectPath", ""),
                "is_array": var_type.get("IsArray", False),
                "is_map": var_type.get("IsMap", False),
                "migration": TYPE_MIGRATIONS[needs_migration],
                "action": f"Change type to {TYPE_MIGRATIONS[needs_migration]['new_type']}"
            })

    return migrations


def analyze_node(node, graph_name):
    """Analyze a single node for migration needs."""
    migrations = []

    node_class = node.get("NodeClass", "")
    node_title = node.get("NodeTitle", "")
    node_guid = node.get("NodeGuid", "")

    # Check pins
    pins = node.get("Pins", [])
    for pin in pins:
        pin_name = pin.get("Name", "")
        pin_type = pin.get("Type", {})
        default_value = pin.get("DefaultValue", "")

        needs_migration = check_type_needs_migration(pin_type)
        if needs_migration:
            migration_info = TYPE_MIGRATIONS[needs_migration]

            # Check if value needs mapping
            new_value = default_value
            if "value_map" in migration_info and default_value in migration_info["value_map"]:
                new_value = migration_info["value_map"][default_value]

            migrations.append({
                "type": "pin",
                "graph": graph_name,
                "node_guid": node_guid,
                "node_class": node_class,
                "node_title": node_title,
                "pin_name": pin_name,
                "pin_direction": pin.get("Direction", ""),
                "original_type": pin_type.get("SubCategoryObject", ""),
                "original_value": default_value,
                "new_type": migration_info["new_type"],
                "new_value": new_value if new_value != default_value else None,
                "linked_to": pin.get("LinkedTo", []),
                "action": f"Change pin type to {migration_info['new_type']}" +
                         (f", value: {default_value} -> {new_value}" if new_value != default_value else "")
            })

        # Check for enum value that needs migration even if type is already correct
        if default_value:
            for type_name, migration in TYPE_MIGRATIONS.items():
                if "value_map" in migration and default_value in migration["value_map"]:
                    # Only add if not already added
                    if not any(m.get("pin_name") == pin_name and m.get("node_guid") == node_guid for m in migrations):
                        migrations.append({
                            "type": "pin_value",
                            "graph": graph_name,
                            "node_guid": node_guid,
                            "node_class": node_class,
                            "node_title": node_title,
                            "pin_name": pin_name,
                            "original_value": default_value,
                            "new_value": migration["value_map"][default_value],
                            "action": f"Change value: {default_value} -> {migration['value_map'][default_value]}"
                        })

    # Check for Break/Make struct nodes
    if "BreakStruct" in node_class or "MakeStruct" in node_class:
        for type_name, migration in TYPE_MIGRATIONS.items():
            if type_name in node_title:
                migrations.append({
                    "type": "struct_node",
                    "graph": graph_name,
                    "node_guid": node_guid,
                    "node_class": node_class,
                    "node_title": node_title,
                    "original_struct": type_name,
                    "new_struct": migration["new_type"],
                    "new_path": migration["new_path"],
                    "pins": pins,
                    "action": f"Recreate node with C++ struct {migration['new_type']}"
                })

    # Check for Switch nodes on enum
    if "SwitchEnum" in node_class:
        for type_name, migration in TYPE_MIGRATIONS.items():
            if type_name in node_title:
                migrations.append({
                    "type": "switch_node",
                    "graph": graph_name,
                    "node_guid": node_guid,
                    "node_class": node_class,
                    "node_title": node_title,
                    "original_enum": type_name,
                    "new_enum": migration["new_type"],
                    "pins": pins,
                    "action": f"Recreate Switch node with C++ enum {migration['new_type']}"
                })

    return migrations


def analyze_graph(graph, parent_name=""):
    """Recursively analyze a graph for migration needs."""
    migrations = []

    graph_name = graph.get("GraphName", "")
    full_graph_name = f"{parent_name}/{graph_name}" if parent_name else graph_name

    nodes = graph.get("Nodes", [])
    for node in nodes:
        migrations.extend(analyze_node(node, full_graph_name))

        # Check collapsed graphs
        collapsed = node.get("CollapsedGraph")
        if collapsed:
            migrations.extend(analyze_graph(collapsed, full_graph_name))

    return migrations


def generate_yaml(bp_name, bp_data, migrations):
    """Generate YAML migration plan."""
    lines = []
    lines.append(f"# Migration Plan for {bp_name}")
    lines.append(f"# Generated from JSON export")
    lines.append(f"# Compare with original Blueprint to verify")
    lines.append("")
    lines.append(f"blueprint: {bp_name}")
    lines.append(f"path: {bp_data.get('Path', '')}")
    lines.append(f"parent_class: {bp_data.get('ParentClass', '')}")
    lines.append("")

    # Group migrations by type
    var_migrations = [m for m in migrations if m["type"] == "variable"]
    pin_migrations = [m for m in migrations if m["type"] in ["pin", "pin_value"]]
    node_migrations = [m for m in migrations if m["type"] in ["struct_node", "switch_node"]]

    # Variables section
    lines.append("# ============================================================")
    lines.append("# VARIABLES REQUIRING MIGRATION")
    lines.append("# ============================================================")
    lines.append("variables:")
    if var_migrations:
        for m in var_migrations:
            lines.append(f"  - name: \"{m['name']}\"")
            lines.append(f"    original:")
            lines.append(f"      type: \"{m['original_type']}\"")
            lines.append(f"      path: \"{m['original_path']}\"")
            lines.append(f"      is_array: {str(m['is_array']).lower()}")
            lines.append(f"      is_map: {str(m['is_map']).lower()}")
            lines.append(f"    migration:")
            lines.append(f"      new_type: \"{m['migration']['new_type']}\"")
            lines.append(f"      new_path: \"{m['migration']['new_path']}\"")
            lines.append(f"    action: \"{m['action']}\"")
            lines.append("")
    else:
        lines.append("  # No variable migrations needed")
        lines.append("")

    # Nodes section
    lines.append("# ============================================================")
    lines.append("# NODES REQUIRING MIGRATION (Break/Make/Switch)")
    lines.append("# ============================================================")
    lines.append("nodes:")
    if node_migrations:
        for m in node_migrations:
            lines.append(f"  - node_guid: \"{m['node_guid']}\"")
            lines.append(f"    graph: \"{m['graph']}\"")
            lines.append(f"    node_class: \"{m['node_class']}\"")
            lines.append(f"    node_title: \"{m['node_title']}\"")
            lines.append(f"    original:")
            if m["type"] == "struct_node":
                lines.append(f"      struct: \"{m['original_struct']}\"")
            else:
                lines.append(f"      enum: \"{m['original_enum']}\"")
            lines.append(f"    migration:")
            if m["type"] == "struct_node":
                lines.append(f"      new_struct: \"{m['new_struct']}\"")
                lines.append(f"      new_path: \"{m['new_path']}\"")
            else:
                lines.append(f"      new_enum: \"{m['new_enum']}\"")
            lines.append(f"    pins:")
            for pin in m.get("pins", [])[:10]:  # Limit to 10 pins for readability
                lines.append(f"      - name: \"{pin.get('Name', '')}\"")
                lines.append(f"        direction: \"{pin.get('Direction', '')}\"")
                if pin.get("LinkedTo"):
                    lines.append(f"        linked_to:")
                    for link in pin["LinkedTo"][:3]:
                        lines.append(f"          - node: \"{link.get('NodeTitle', '')}\"")
                        lines.append(f"            pin: \"{link.get('PinName', '')}\"")
            lines.append(f"    action: \"{m['action']}\"")
            lines.append("")
    else:
        lines.append("  # No node migrations needed")
        lines.append("")

    # Pin values section
    lines.append("# ============================================================")
    lines.append("# PIN VALUES REQUIRING MIGRATION")
    lines.append("# ============================================================")
    lines.append("pin_values:")
    if pin_migrations:
        for m in pin_migrations:
            if m.get("new_value") or m["type"] == "pin_value":
                lines.append(f"  - node_guid: \"{m['node_guid']}\"")
                lines.append(f"    graph: \"{m['graph']}\"")
                lines.append(f"    node_title: \"{m['node_title']}\"")
                lines.append(f"    pin_name: \"{m['pin_name']}\"")
                lines.append(f"    original_value: \"{m.get('original_value', '')}\"")
                lines.append(f"    new_value: \"{m.get('new_value', '')}\"")
                lines.append(f"    action: \"{m['action']}\"")
                lines.append("")
    else:
        lines.append("  # No pin value migrations needed")
        lines.append("")

    return "\n".join(lines)


def main():
    print("\n" + "="*70)
    print("  MIGRATION PLAN GENERATOR")
    print("="*70)

    # Create output directory
    os.makedirs(OUTPUT_PATH, exist_ok=True)

    for folder, bp_name in BLUEPRINTS_TO_ANALYZE:
        print(f"\nAnalyzing {bp_name}...")

        bp_data = load_json(folder, bp_name)
        if not bp_data:
            print(f"  Could not load JSON for {bp_name}")
            continue

        # Analyze variables
        migrations = analyze_variables(bp_data)

        # Analyze all graphs
        logic = bp_data.get("Logic", {})
        all_graphs = logic.get("AllGraphs", [])
        for graph in all_graphs:
            migrations.extend(analyze_graph(graph))

        print(f"  Found {len(migrations)} migration items")

        # Group by type for summary
        var_count = len([m for m in migrations if m["type"] == "variable"])
        node_count = len([m for m in migrations if m["type"] in ["struct_node", "switch_node"]])
        pin_count = len([m for m in migrations if m["type"] in ["pin", "pin_value"]])

        print(f"    - Variables: {var_count}")
        print(f"    - Nodes: {node_count}")
        print(f"    - Pin values: {pin_count}")

        # Generate YAML
        yaml_content = generate_yaml(bp_name, bp_data, migrations)

        # Write YAML file
        yaml_path = os.path.join(OUTPUT_PATH, f"{bp_name}_migration_plan.yaml")
        with open(yaml_path, 'w', encoding='utf-8') as f:
            f.write(yaml_content)

        print(f"  Written to: {yaml_path}")

    print("\n" + "="*70)
    print("  MIGRATION PLANS GENERATED")
    print("="*70)
    print(f"\nOutput directory: {OUTPUT_PATH}")
    print("\nReview the YAML files to verify the migration plan matches")
    print("what you see in the original Blueprints.")


if __name__ == "__main__" or True:
    main()
