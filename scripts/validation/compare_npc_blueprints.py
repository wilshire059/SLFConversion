"""
Compare NPC Blueprints between bp_only and SLFConversion.
Exports SCS components, AnimBP, variables using compatible API.
"""
import unreal
import json
import os

NPC_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
]

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/npc_comparison"

def ensure_dir(path):
    if not os.path.exists(path):
        os.makedirs(path)

def export_npc_data(bp_path):
    """Export detailed NPC Blueprint data."""
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        return {"error": f"Could not load {bp_path}"}

    data = {
        "path": bp_path,
        "name": bp.get_name(),
        "parent_class": None,
        "scs_components": [],
        "has_ac_ai_interaction_manager": False,
        "has_look_at_radius": False,
        "anim_class": None,
        "eventgraph_node_count": 0,
    }

    # Get parent class using parent_class property
    try:
        parent = bp.get_editor_property("parent_class")
        if parent:
            data["parent_class"] = str(parent.get_path_name())
    except Exception as e:
        data["parent_class_error"] = str(e)

    # Get SCS components
    try:
        scs = bp.simple_construction_script
        if scs:
            all_nodes = scs.get_all_nodes()
            for node in all_nodes:
                if node:
                    comp_template = node.component_template
                    if comp_template:
                        comp_name = comp_template.get_name()
                        comp_class = comp_template.get_class().get_name()

                        comp_info = {
                            "name": comp_name,
                            "class": comp_class,
                        }

                        # Check for AC_AI_InteractionManager
                        if "AI_InteractionManager" in comp_class or "AI_InteractionManager" in comp_name:
                            data["has_ac_ai_interaction_manager"] = True
                            comp_info["is_dialog_component"] = True

                        # Check for LookAtRadius
                        if "LookAt" in comp_name or "Look At" in comp_name:
                            data["has_look_at_radius"] = True
                            comp_info["is_look_at"] = True

                        # Check for SkeletalMeshComponent and get AnimClass
                        if comp_class == "SkeletalMeshComponent":
                            try:
                                anim_class = comp_template.get_editor_property("anim_class")
                                if anim_class:
                                    data["anim_class"] = str(anim_class.get_path_name())
                                    comp_info["anim_class"] = data["anim_class"]
                            except:
                                pass

                        data["scs_components"].append(comp_info)
    except Exception as e:
        data["scs_error"] = str(e)

    # Check EventGraph nodes
    try:
        for graph in bp.ubergraph_pages:
            if graph and graph.get_name() == "EventGraph":
                nodes = graph.nodes
                data["eventgraph_node_count"] = len(nodes) if nodes else 0
                break
    except Exception as e:
        data["eventgraph_error"] = str(e)

    return data

def main():
    ensure_dir(OUTPUT_DIR)

    unreal.log_warning("\n=== NPC Blueprint Comparison ===\n")

    # Determine which project we're in
    project_name = "unknown"
    try:
        # Check if SLFAutomationLibrary exists (only in SLFConversion)
        if hasattr(unreal, 'SLFAutomationLibrary'):
            project_name = "slfconversion"
        else:
            project_name = "bp_only"
    except:
        pass

    unreal.log_warning(f"Project: {project_name}")

    # Export data for each NPC
    results = {}
    for bp_path in NPC_PATHS:
        bp_name = bp_path.split("/")[-1]
        unreal.log_warning(f"\nExporting: {bp_name}")

        data = export_npc_data(bp_path)
        results[bp_name] = data

        # Log key findings
        unreal.log_warning(f"  Parent: {data.get('parent_class', 'UNKNOWN')}")
        unreal.log_warning(f"  AC_AI_InteractionManager: {'YES' if data.get('has_ac_ai_interaction_manager') else 'NO'}")
        unreal.log_warning(f"  LookAtRadius: {'YES' if data.get('has_look_at_radius') else 'NO'}")
        unreal.log_warning(f"  AnimClass: {data.get('anim_class', 'NONE')}")
        unreal.log_warning(f"  EventGraph nodes: {data.get('eventgraph_node_count', 'UNKNOWN')}")
        unreal.log_warning(f"  SCS Components ({len(data.get('scs_components', []))}):")
        for comp in data.get("scs_components", []):
            unreal.log_warning(f"    - {comp.get('name')} ({comp.get('class')})")

    # Save results
    output_file = f"{project_name}_npcs.json"
    output_path = os.path.join(OUTPUT_DIR, output_file)
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2, default=str)

    unreal.log_warning(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    main()
