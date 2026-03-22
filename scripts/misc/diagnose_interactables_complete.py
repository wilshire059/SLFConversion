# diagnose_interactables_complete.py
# Thorough diagnostic of interactable Blueprints AFTER migration
# Compares: current state, working backup (Copy 9), and bp_only

import unreal
import json

BLUEPRINTS_TO_CHECK = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField",
]

def check_blueprint(bp_path):
    """Check a single Blueprint thoroughly."""
    bp_name = bp_path.split('/')[-1]
    result = {"name": bp_name, "path": bp_path}

    bp = unreal.load_asset(bp_path)
    if not bp:
        result["error"] = "Could not load Blueprint"
        return result

    # Get generated class
    gen_class = bp.generated_class()
    if not gen_class:
        result["error"] = "No generated class"
        return result

    result["generated_class"] = gen_class.get_name()

    # Get parent class using static_class approach
    try:
        parent_class = bp.get_editor_property("parent_class")
        result["parent_class"] = parent_class.get_name() if parent_class else "None"
        result["parent_class_path"] = parent_class.get_path_name() if parent_class else "None"
        # Check if parent is C++ (not Blueprint)
        if parent_class:
            parent_path = parent_class.get_path_name()
            result["parent_is_cpp"] = parent_path.startswith("/Script/")
    except Exception as e:
        result["parent_class"] = f"Error: {e}"
        result["parent_class_path"] = "Unknown"
        result["parent_is_cpp"] = "Unknown"

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        result["error"] = "No CDO"
        return result

    # Check interfaces on CDO
    result["implements_SLFInteractable"] = cdo.implements_interface(
        unreal.load_class(None, "/Script/SLFConversion.SLFInteractableInterface")
    ) if unreal.load_class(None, "/Script/SLFConversion.SLFInteractableInterface") else "Interface class not found"

    # Try BPI_Interactable (old Blueprint interface)
    try:
        bpi_class = unreal.load_class(None, "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Interactable.BPI_Interactable_C")
        if bpi_class:
            result["implements_BPI_Interactable"] = cdo.implements_interface(bpi_class)
        else:
            result["implements_BPI_Interactable"] = "BPI class not found"
    except:
        result["implements_BPI_Interactable"] = "Error checking BPI"

    # Get all components from CDO
    try:
        components = cdo.get_components_by_class(unreal.ActorComponent)
        result["cdo_component_count"] = len(components)
        result["cdo_components"] = []
        for comp in components:
            comp_info = {
                "name": comp.get_name(),
                "class": comp.get_class().get_name()
            }
            # Check collision settings for sphere/box components
            if isinstance(comp, (unreal.SphereComponent, unreal.BoxComponent)):
                try:
                    comp_info["collision_enabled"] = str(comp.get_editor_property("collision_enabled"))
                    comp_info["collision_object_type"] = str(comp.get_editor_property("collision_object_type"))
                except:
                    pass
            result["cdo_components"].append(comp_info)
    except Exception as e:
        result["cdo_components_error"] = str(e)

    # Check root component
    try:
        root = cdo.get_editor_property('root_component')
        if root:
            result["root_component"] = root.get_name()
            result["root_component_class"] = root.get_class().get_name()
        else:
            result["root_component"] = "None"
    except Exception as e:
        result["root_component_error"] = str(e)

    # Check event graph node count (are graphs cleared?)
    try:
        graph_info = []
        for graph in bp.ubergraph_pages:
            graph_name = graph.get_name()
            node_count = len(list(graph.nodes))
            graph_info.append({"name": graph_name, "node_count": node_count})
        result["event_graphs"] = graph_info
        result["total_event_graph_nodes"] = sum(g["node_count"] for g in graph_info)
    except Exception as e:
        result["event_graphs_error"] = str(e)

    # Check function graphs
    try:
        func_info = []
        for graph in bp.function_graphs:
            graph_name = graph.get_name()
            node_count = len(list(graph.nodes))
            func_info.append({"name": graph_name, "node_count": node_count})
        result["function_graphs"] = func_info
        result["total_function_graph_nodes"] = sum(g["node_count"] for g in func_info)
    except Exception as e:
        result["function_graphs_error"] = str(e)

    # Check SCS (Simple Construction Script) components
    try:
        scs = bp.simple_construction_script
        if scs:
            scs_nodes = scs.get_all_nodes()
            result["scs_node_count"] = len(scs_nodes)
            result["scs_components"] = []
            for node in scs_nodes:
                comp_template = node.component_template
                if comp_template:
                    result["scs_components"].append({
                        "name": comp_template.get_name(),
                        "class": comp_template.get_class().get_name()
                    })
        else:
            result["scs_node_count"] = 0
    except Exception as e:
        result["scs_error"] = str(e)

    # Check key properties that should exist
    key_props = ["InteractionCollision", "CachedInteractableSM", "CachedInteractableSK",
                 "bCanInteract", "InteractionPrompt", "InteractionText"]
    result["properties"] = {}
    for prop_name in key_props:
        try:
            val = cdo.get_editor_property(prop_name.lower().replace("cached", "cached_"))
            result["properties"][prop_name] = "Found"
        except:
            # Try exact name
            try:
                val = cdo.get_editor_property(prop_name)
                result["properties"][prop_name] = "Found"
            except:
                result["properties"][prop_name] = "NOT FOUND"

    return result


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("COMPLETE INTERACTABLE DIAGNOSTIC")
    unreal.log_warning("=" * 70)

    all_results = {}

    for bp_path in BLUEPRINTS_TO_CHECK:
        result = check_blueprint(bp_path)
        all_results[result["name"]] = result

        # Print summary
        unreal.log_warning(f"\n{'='*60}")
        unreal.log_warning(f"Blueprint: {result['name']}")
        unreal.log_warning(f"{'='*60}")

        if "error" in result:
            unreal.log_error(f"  ERROR: {result['error']}")
            continue

        unreal.log_warning(f"  Parent Class: {result.get('parent_class', 'Unknown')}")
        unreal.log_warning(f"  Parent Path: {result.get('parent_class_path', 'Unknown')}")
        unreal.log_warning(f"  Parent is C++: {result.get('parent_is_cpp', 'Unknown')}")
        unreal.log_warning(f"  Root Component: {result.get('root_component', 'Unknown')} ({result.get('root_component_class', 'Unknown')})")

        # Interfaces
        unreal.log_warning(f"\n  INTERFACES:")
        unreal.log_warning(f"    ISLFInteractableInterface: {result.get('implements_SLFInteractable', 'Unknown')}")
        unreal.log_warning(f"    BPI_Interactable: {result.get('implements_BPI_Interactable', 'Unknown')}")

        # Components
        unreal.log_warning(f"\n  CDO COMPONENTS ({result.get('cdo_component_count', 0)}):")
        for comp in result.get("cdo_components", []):
            collision_info = ""
            if "collision_enabled" in comp:
                collision_info = f" [Collision: {comp['collision_enabled']}, Type: {comp['collision_object_type']}]"
            unreal.log_warning(f"    - {comp['name']} ({comp['class']}){collision_info}")

        # SCS Components
        unreal.log_warning(f"\n  SCS COMPONENTS ({result.get('scs_node_count', 0)}):")
        for comp in result.get("scs_components", []):
            unreal.log_warning(f"    - {comp['name']} ({comp['class']})")

        # Event Graphs
        unreal.log_warning(f"\n  EVENT GRAPHS (Total nodes: {result.get('total_event_graph_nodes', 'Unknown')}):")
        for graph in result.get("event_graphs", []):
            status = "CLEARED" if graph["node_count"] == 0 else f"HAS {graph['node_count']} NODES"
            unreal.log_warning(f"    - {graph['name']}: {status}")

        # Function Graphs
        unreal.log_warning(f"\n  FUNCTION GRAPHS (Total nodes: {result.get('total_function_graph_nodes', 'Unknown')}):")
        for graph in result.get("function_graphs", []):
            unreal.log_warning(f"    - {graph['name']}: {graph['node_count']} nodes")

        # Properties
        unreal.log_warning(f"\n  KEY PROPERTIES:")
        for prop, status in result.get("properties", {}).items():
            unreal.log_warning(f"    - {prop}: {status}")

    # Save to file
    output_path = "C:/scripts/SLFConversion/migration_cache/interactable_diagnostic.json"
    with open(output_path, 'w') as f:
        json.dump(all_results, f, indent=2, default=str)
    unreal.log_warning(f"\n\nDiagnostic saved to: {output_path}")


if __name__ == "__main__":
    main()
