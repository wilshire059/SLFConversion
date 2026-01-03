"""
Analyze all Blueprint JSON exports and generate comprehensive migration tracker.
Run headlessly via UnrealEditor-Cmd or standalone Python.
"""
import json
import os
from pathlib import Path
from collections import defaultdict

DNA_PATH = Path(r"C:/scripts/SLFConversion_Migration/Backups/SoulsClean/Exports/BlueprintDNA")
OUTPUT_PATH = Path(r"C:/scripts/SLFConversion/MASTER_MIGRATION_TRACKER.md")

def count_items(data: dict) -> dict:
    """Count all logic-bearing items in a Blueprint JSON."""
    counts = {
        "variables": 0,
        "functions": 0,
        "event_dispatchers": 0,
        "interface_functions": 0,
        "graphs": 0,
        "macros": 0,
        "events": 0
    }

    # Variables
    if "Variables" in data and "List" in data["Variables"]:
        counts["variables"] = len(data["Variables"]["List"])

    # Event Dispatchers
    if "EventDispatchers" in data and "List" in data["EventDispatchers"]:
        counts["event_dispatchers"] = len(data["EventDispatchers"]["List"])

    # Interface function signatures (for interface BPs)
    if "FunctionSignatures" in data and "Functions" in data["FunctionSignatures"]:
        counts["interface_functions"] = len(data["FunctionSignatures"]["Functions"])

    # Graphs
    if "Graphs" in data:
        graphs = data["Graphs"]
        for graph_name, graph_data in graphs.items():
            if graph_name == "EventGraph":
                # Count events in event graph
                if "Nodes" in graph_data:
                    for node in graph_data["Nodes"]:
                        node_class = node.get("Class", "")
                        if "K2Node_Event" in node_class or "K2Node_CustomEvent" in node_class:
                            counts["events"] += 1
            elif "Macro" in graph_name:
                counts["macros"] += 1
            else:
                counts["functions"] += 1
            counts["graphs"] += 1

    # Functions array (sometimes separate from Graphs)
    if "Functions" in data:
        if isinstance(data["Functions"], list):
            counts["functions"] += len(data["Functions"])

    return counts

def analyze_category(category_path: Path, category_name: str) -> list:
    """Analyze all JSONs in a category."""
    results = []

    if not category_path.exists():
        return results

    for json_file in sorted(category_path.glob("*.json")):
        try:
            with open(json_file, 'r', encoding='utf-8') as f:
                data = json.load(f)

            name = data.get("Name", json_file.stem)
            counts = count_items(data)
            total_items = sum(counts.values())

            results.append({
                "name": name,
                "file": json_file.name,
                "category": category_name,
                "parent": data.get("ParentClass", "Unknown"),
                "counts": counts,
                "total_items": total_items,
                "interfaces": data.get("Interfaces", {}).get("List", [])
            })
        except Exception as e:
            print(f"Error processing {json_file}: {e}")

    return results

def generate_tracker():
    """Generate the comprehensive migration tracker."""
    categories = {
        "Interface": "Layer 1 - Interfaces",
        "Component": "Layer 2 - Actor Components",
        "Blueprint": "Layer 3 - Blueprint Classes",
        "GameFramework": "Layer 4 - Game Framework",
        "AnimBlueprint": "Layer 5 - Animation Blueprints",
        "WidgetBlueprint": "Layer 6 - Widget Blueprints",
        "AI": "Layer 7 - AI/Behavior Trees"
    }

    all_results = []
    layer_totals = defaultdict(lambda: {"blueprints": 0, "items": 0})

    for category, layer_name in categories.items():
        category_path = DNA_PATH / category
        results = analyze_category(category_path, layer_name)
        all_results.extend(results)

        for r in results:
            layer_totals[layer_name]["blueprints"] += 1
            layer_totals[layer_name]["items"] += r["total_items"]

    # Generate markdown
    md_lines = []
    md_lines.append("# MASTER MIGRATION TRACKER")
    md_lines.append("")
    md_lines.append(f"**Generated:** Autonomous Session 2026-01-01")
    md_lines.append(f"**Total Blueprints:** {len(all_results)}")
    md_lines.append(f"**Total Logic Items:** {sum(r['total_items'] for r in all_results)}")
    md_lines.append("")
    md_lines.append("---")
    md_lines.append("")

    # Summary by layer
    md_lines.append("## Summary by Layer")
    md_lines.append("")
    md_lines.append("| Layer | Blueprints | Items | Status |")
    md_lines.append("|-------|------------|-------|--------|")
    for layer_name, totals in sorted(layer_totals.items()):
        md_lines.append(f"| {layer_name} | {totals['blueprints']} | {totals['items']} | Not Started |")
    md_lines.append("")
    md_lines.append("---")
    md_lines.append("")

    # Detailed by layer
    current_layer = None
    for result in sorted(all_results, key=lambda x: (x["category"], x["name"])):
        if result["category"] != current_layer:
            current_layer = result["category"]
            md_lines.append(f"## {current_layer}")
            md_lines.append("")

        counts = result["counts"]
        md_lines.append(f"### {result['name']}")
        md_lines.append(f"- **File:** `{result['file']}`")
        md_lines.append(f"- **Parent:** {result['parent']}")
        md_lines.append(f"- **Total Items:** {result['total_items']}")

        if counts["variables"] > 0:
            md_lines.append(f"- Variables: {counts['variables']}")
        if counts["functions"] > 0:
            md_lines.append(f"- Functions: {counts['functions']}")
        if counts["interface_functions"] > 0:
            md_lines.append(f"- Interface Functions: {counts['interface_functions']}")
        if counts["event_dispatchers"] > 0:
            md_lines.append(f"- Event Dispatchers: {counts['event_dispatchers']}")
        if counts["graphs"] > 0:
            md_lines.append(f"- Graphs: {counts['graphs']}")
        if counts["macros"] > 0:
            md_lines.append(f"- Macros: {counts['macros']}")
        if counts["events"] > 0:
            md_lines.append(f"- Events: {counts['events']}")

        if result["interfaces"]:
            iface_names = [i.get("Name", str(i)) if isinstance(i, dict) else str(i) for i in result["interfaces"]]
            md_lines.append(f"- Implements: {', '.join(iface_names)}")

        md_lines.append("")
        md_lines.append("**20-Pass Validation:**")
        md_lines.append("- [ ] Pass 1-5: JSON Analysis")
        md_lines.append("- [ ] Pass 6-10: Implementation")
        md_lines.append("- [ ] Pass 11-15: Verification")
        md_lines.append("- [ ] Pass 16-20: Testing")
        md_lines.append("")
        md_lines.append("---")
        md_lines.append("")

    # Write output
    with open(OUTPUT_PATH, 'w', encoding='utf-8') as f:
        f.write('\n'.join(md_lines))

    print(f"Generated tracker with {len(all_results)} blueprints")
    print(f"Total logic items: {sum(r['total_items'] for r in all_results)}")

    return all_results

if __name__ == "__main__":
    results = generate_tracker()
    print(f"\nTracker saved to: {OUTPUT_PATH}")
