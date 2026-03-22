"""
Deep comparison of B_Door vs B_BossDoor - saves results to JSON file
"""
import unreal
import json

results = {"B_Door": {}, "B_BossDoor": {}}

def analyze_blueprint(bp_path, name):
    data = {"path": bp_path, "exists": False, "compile_info": {}, "scs": {}, "blueprint_cdo": [], "cpp_cdo": []}

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        data["error"] = f"Could not load {bp_path}"
        return data

    data["exists"] = True
    data["blueprint_name"] = bp.get_name()
    data["blueprint_class"] = bp.get_class().get_name()

    # Check compile status
    try:
        status = bp.get_editor_property('status')
        data["compile_info"]["status"] = str(status)
    except Exception as e:
        data["compile_info"]["status_error"] = str(e)

    # Check UbergraphPages (EventGraph)
    try:
        ubergraph_pages = bp.get_editor_property('ubergraph_pages')
        data["compile_info"]["ubergraph_pages_count"] = len(ubergraph_pages) if ubergraph_pages else 0
    except Exception as e:
        data["compile_info"]["ubergraph_error"] = str(e)

    # Check FunctionGraphs
    try:
        func_graphs = bp.get_editor_property('function_graphs')
        data["compile_info"]["function_graphs_count"] = len(func_graphs) if func_graphs else 0
    except Exception as e:
        data["compile_info"]["function_graphs_error"] = str(e)

    # Check SCS
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            all_nodes = scs.get_editor_property('all_nodes')
            root_nodes = scs.get_editor_property('root_nodes')
            default_root = scs.get_editor_property('default_scene_root_node')

            data["scs"]["all_nodes_count"] = len(all_nodes) if all_nodes else 0
            data["scs"]["root_nodes_count"] = len(root_nodes) if root_nodes else 0
            data["scs"]["default_root"] = default_root.get_name() if default_root else None

            # List SCS node details
            if all_nodes:
                data["scs"]["nodes"] = []
                for node in all_nodes:
                    try:
                        var_name = str(node.get_editor_property('internal_variable_name'))
                        comp_template = node.get_editor_property('component_template')
                        comp_class = comp_template.get_class().get_name() if comp_template else "None"
                        data["scs"]["nodes"].append({"var_name": var_name, "class": comp_class})
                    except Exception as e:
                        data["scs"]["nodes"].append({"error": str(e)})
        else:
            data["scs"]["status"] = "SCS is None"
    except Exception as e:
        data["scs"]["error"] = str(e)

    # Check generated class
    gen_class = bp.generated_class()
    if gen_class:
        data["generated_class"] = gen_class.get_name()

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # List ALL components
            all_comps = cdo.get_components_by_class(unreal.ActorComponent)
            for comp in all_comps:
                comp_info = {
                    "name": comp.get_name(),
                    "class": comp.get_class().get_name(),
                }
                if isinstance(comp, unreal.SceneComponent):
                    comp_info["visible"] = comp.is_visible()
                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    comp_info["mesh"] = mesh.get_name() if mesh else "NO_MESH"
                data["blueprint_cdo"].append(comp_info)

    # Also check C++ parent CDO for comparison
    if name == "B_Door":
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.B_Door")
    else:
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.B_BossDoor")

    if cpp_class:
        data["cpp_class"] = cpp_class.get_name()
        cpp_cdo = unreal.get_default_object(cpp_class)
        if cpp_cdo:
            all_comps = cpp_cdo.get_components_by_class(unreal.ActorComponent)
            for comp in all_comps:
                comp_info = {
                    "name": comp.get_name(),
                    "class": comp.get_class().get_name(),
                }
                if isinstance(comp, unreal.SceneComponent):
                    comp_info["visible"] = comp.is_visible()
                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    comp_info["mesh"] = mesh.get_name() if mesh else "NO_MESH"
                data["cpp_cdo"].append(comp_info)

    return data

# Analyze both
results["B_Door"] = analyze_blueprint("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door", "B_Door")
results["B_BossDoor"] = analyze_blueprint("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor", "B_BossDoor")

# Add summary comparison
results["comparison"] = {
    "b_door_bp_cdo_components": len(results["B_Door"].get("blueprint_cdo", [])),
    "b_door_cpp_cdo_components": len(results["B_Door"].get("cpp_cdo", [])),
    "b_bossdoor_bp_cdo_components": len(results["B_BossDoor"].get("blueprint_cdo", [])),
    "b_bossdoor_cpp_cdo_components": len(results["B_BossDoor"].get("cpp_cdo", [])),
    "b_door_scs_empty": results["B_Door"].get("scs", {}).get("all_nodes_count", -1) == 0,
    "b_bossdoor_scs_empty": results["B_BossDoor"].get("scs", {}).get("all_nodes_count", -1) == 0,
}

# Write to file
with open("C:/scripts/SLFConversion/migration_cache/door_comparison.json", "w") as f:
    json.dump(results, f, indent=2)

unreal.log(f"Results saved to migration_cache/door_comparison.json")
unreal.log(f"B_Door BP CDO: {results['comparison']['b_door_bp_cdo_components']} components")
unreal.log(f"B_Door C++ CDO: {results['comparison']['b_door_cpp_cdo_components']} components")
unreal.log(f"B_BossDoor BP CDO: {results['comparison']['b_bossdoor_bp_cdo_components']} components")
unreal.log(f"B_BossDoor C++ CDO: {results['comparison']['b_bossdoor_cpp_cdo_components']} components")
