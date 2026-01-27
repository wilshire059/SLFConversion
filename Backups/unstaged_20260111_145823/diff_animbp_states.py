# diff_animbp_states.py
# Compare backup and current AnimBP states

import json
import os

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/animbp_states"
REPORT_FILE = "C:/scripts/SLFConversion/migration_cache/animbp_diff_report.txt"

ANIMBPS = [
    "ABP_SoulslikeCharacter_Additive",
    "ABP_SoulslikeNPC",
    "ABP_SoulslikeEnemy",
    "ABP_SoulslikeBossNew",
]

def compare_variables(backup_vars, current_vars):
    """Compare variables between backup and current"""
    differences = []

    backup_names = {v['Name']: v for v in backup_vars}
    current_names = {v['Name']: v for v in current_vars}

    # Find missing variables
    for name in backup_names:
        if name not in current_names:
            differences.append(f"  MISSING VAR: {name}")

    # Find added variables
    for name in current_names:
        if name not in backup_names:
            differences.append(f"  ADDED VAR: {name}")

    # Find changed types
    for name in backup_names:
        if name in current_names:
            if backup_names[name]['Type'] != current_names[name]['Type']:
                differences.append(f"  TYPE CHANGE: {name}: {backup_names[name]['Type']} -> {current_names[name]['Type']}")

    return differences

def compare_graphs(backup_graphs, current_graphs):
    """Compare graphs between backup and current"""
    differences = []

    backup_by_name = {g['GraphName']: g for g in backup_graphs}
    current_by_name = {g['GraphName']: g for g in current_graphs}

    for name, backup_graph in backup_by_name.items():
        if name not in current_by_name:
            differences.append(f"  MISSING GRAPH: {name}")
            continue

        current_graph = current_by_name[name]

        backup_nodes = len(backup_graph.get('Nodes', []))
        current_nodes = len(current_graph.get('Nodes', []))

        if name == 'EventGraph':
            # EventGraph should be cleared - this is expected
            if backup_nodes > 0 and current_nodes == 0:
                differences.append(f"  [EXPECTED] EventGraph cleared: {backup_nodes} -> {current_nodes} nodes")
            elif current_nodes > 0:
                differences.append(f"  [WARNING] EventGraph not cleared: still has {current_nodes} nodes")
        elif name == 'AnimGraph':
            if backup_nodes != current_nodes:
                differences.append(f"  [CRITICAL] AnimGraph node count: {backup_nodes} -> {current_nodes}")
            else:
                differences.append(f"  [OK] AnimGraph node count: {backup_nodes} (unchanged)")
        else:
            # Other graphs (functions)
            if backup_nodes != current_nodes:
                differences.append(f"  GRAPH {name}: {backup_nodes} -> {current_nodes} nodes")

    for name in current_by_name:
        if name not in backup_by_name:
            differences.append(f"  ADDED GRAPH: {name}")

    return differences

def compare_nodes_detailed(backup_graph, current_graph, graph_name):
    """Compare nodes in detail for AnimGraph"""
    differences = []

    if graph_name != 'AnimGraph':
        return differences

    backup_nodes = backup_graph.get('Nodes', [])
    current_nodes = current_graph.get('Nodes', [])

    backup_by_name = {n['NodeName']: n for n in backup_nodes}
    current_by_name = {n['NodeName']: n for n in current_nodes}

    # Check for missing nodes
    for name, node in backup_by_name.items():
        if name not in current_by_name:
            differences.append(f"    MISSING NODE: {name} ({node.get('NodeClass', 'unknown')})")
        else:
            current_node = current_by_name[name]
            # Check pin count
            backup_pins = len(node.get('Pins', []))
            current_pins = len(current_node.get('Pins', []))
            if backup_pins != current_pins:
                differences.append(f"    PIN COUNT: {name}: {backup_pins} -> {current_pins}")

    return differences

def diff_animbp(name):
    """Diff a single AnimBP"""
    backup_file = os.path.join(OUTPUT_DIR, f"{name}_backup.json")
    current_file = os.path.join(OUTPUT_DIR, f"{name}_current.json")

    if not os.path.exists(backup_file):
        return [f"  ERROR: Backup file not found: {backup_file}"]
    if not os.path.exists(current_file):
        return [f"  ERROR: Current file not found: {current_file}"]

    with open(backup_file, 'r') as f:
        backup = json.load(f)
    with open(current_file, 'r') as f:
        current = json.load(f)

    differences = []

    # Compare parent class
    if backup.get('ParentClass') != current.get('ParentClass'):
        differences.append(f"  PARENT CLASS: {backup.get('ParentClass')} -> {current.get('ParentClass')}")

    # Compare variables
    var_diffs = compare_variables(
        backup.get('Variables', []),
        current.get('Variables', [])
    )
    differences.extend(var_diffs)

    # Compare graphs
    graph_diffs = compare_graphs(
        backup.get('Graphs', []),
        current.get('Graphs', [])
    )
    differences.extend(graph_diffs)

    # Detailed AnimGraph comparison
    backup_graphs = {g['GraphName']: g for g in backup.get('Graphs', [])}
    current_graphs = {g['GraphName']: g for g in current.get('Graphs', [])}

    if 'AnimGraph' in backup_graphs and 'AnimGraph' in current_graphs:
        node_diffs = compare_nodes_detailed(
            backup_graphs['AnimGraph'],
            current_graphs['AnimGraph'],
            'AnimGraph'
        )
        differences.extend(node_diffs)

    return differences

def main():
    report = []
    report.append("=" * 60)
    report.append("ANIMBP MIGRATION DIFF REPORT")
    report.append("=" * 60)
    report.append("")

    all_ok = True

    for name in ANIMBPS:
        report.append(f"\n=== {name} ===")
        diffs = diff_animbp(name)

        if not diffs:
            report.append("  [OK] No differences")
        else:
            for diff in diffs:
                report.append(diff)
                if '[CRITICAL]' in diff or 'MISSING' in diff:
                    all_ok = False

    report.append("")
    report.append("=" * 60)
    if all_ok:
        report.append("RESULT: All AnimBPs match expected state")
    else:
        report.append("RESULT: CRITICAL DIFFERENCES FOUND - Requires fixes")
    report.append("=" * 60)

    report_text = "\n".join(report)

    with open(REPORT_FILE, 'w') as f:
        f.write(report_text)

    print(report_text)

if __name__ == "__main__":
    main()
