# export_property_access_paths.py
# Export PropertyAccess node paths from AnimBP

import unreal

def log(msg):
    print(f"[PropAccess] {msg}")
    unreal.log_warning(f"[PropAccess] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/property_access_paths.txt"

def main():
    log("=" * 80)
    log("PROPERTY ACCESS PATH EXPORT")
    log("=" * 80)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"Loaded: {bp.get_name()}")

    # Get all graphs and look for PropertyAccess nodes
    results = []
    graphs = bp.get_all_graphs()
    log(f"Found {len(graphs)} graphs")

    for graph in graphs:
        graph_name = graph.get_name()
        for node in graph.nodes:
            node_class = node.get_class().get_name()
            if 'PropertyAccess' in node_class:
                results.append(f"\n=== Graph: {graph_name} ===")
                results.append(f"Node: {node.get_name()}")
                results.append(f"Class: {node_class}")

                # Try to get path properties
                if hasattr(node, 'path'):
                    results.append(f"Path: {node.path}")
                if hasattr(node, 'text_path'):
                    results.append(f"TextPath: {node.text_path}")

                # Get all properties via editor properties
                try:
                    for prop_name in ['Path', 'TextPath', 'ResolvedPinType', 'GeneratedPropertyName']:
                        prop_name_snake = prop_name.lower()
                        if hasattr(node, 'get_editor_property'):
                            try:
                                val = node.get_editor_property(prop_name_snake)
                                if val:
                                    results.append(f"{prop_name}: {val}")
                            except:
                                pass
                except Exception as e:
                    results.append(f"Error getting properties: {e}")

                # Get pin info
                for pin in node.pins:
                    pin_name = pin.get_name()
                    linked = len(pin.linked_to) if hasattr(pin, 'linked_to') else 0
                    results.append(f"  Pin: {pin_name}, LinkedTo: {linked}")

    output = '\n'.join(results)
    log(f"\nResults:\n{output}")

    with open(OUTPUT_PATH, 'w') as f:
        f.write(output)
    log(f"\nSaved to {OUTPUT_PATH}")

if __name__ == "__main__":
    main()
