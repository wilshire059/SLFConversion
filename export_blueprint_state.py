"""
Export comprehensive Blueprint state for comparison between projects.
Captures class hierarchy, components, and all property values.

Run this on BOTH bp_only and SLFConversion to compare.
"""
import unreal
import json
import os

# Blueprints to compare
BLUEPRINTS_TO_EXPORT = [
    # Enemies
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
    # AI Weapons (correct paths)
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_AI_Weapon",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace",
]

def get_class_hierarchy(uclass):
    """Get full class inheritance chain"""
    hierarchy = []
    current = uclass
    while current:
        hierarchy.append({
            "name": current.get_name(),
            "path": current.get_path_name()
        })
        try:
            current = current.get_super_class() if hasattr(current, 'get_super_class') else None
        except:
            # Try alternate method
            try:
                current = unreal.SystemLibrary.get_class_super_class(current)
            except:
                current = None
    return hierarchy

def get_component_info(component):
    """Extract all relevant info from a component"""
    info = {
        "name": component.get_name(),
        "class": component.get_class().get_name(),
        "class_path": component.get_class().get_path_name(),
    }

    # Get relative transform if it's a scene component
    if isinstance(component, unreal.SceneComponent):
        try:
            rel_loc = component.get_editor_property("relative_location")
            rel_rot = component.get_editor_property("relative_rotation")
            rel_scale = component.get_editor_property("relative_scale3d")
            info["relative_location"] = f"({rel_loc.x}, {rel_loc.y}, {rel_loc.z})"
            info["relative_rotation"] = f"({rel_rot.pitch}, {rel_rot.yaw}, {rel_rot.roll})"
            info["relative_scale"] = f"({rel_scale.x}, {rel_scale.y}, {rel_scale.z})"
        except:
            pass

        # Get attachment info
        try:
            attach_parent = component.get_attach_parent()
            if attach_parent:
                info["attach_parent"] = attach_parent.get_name()
            attach_socket = component.get_attach_socket_name()
            if attach_socket and str(attach_socket) != "None":
                info["attach_socket"] = str(attach_socket)
        except:
            pass

    # ChildActorComponent specific
    if isinstance(component, unreal.ChildActorComponent):
        try:
            child_class = component.get_editor_property("child_actor_class")
            if child_class:
                info["child_actor_class"] = child_class.get_name()
                info["child_actor_class_path"] = child_class.get_path_name()
        except Exception as e:
            info["child_actor_class_error"] = str(e)

    # SkeletalMeshComponent specific
    if isinstance(component, unreal.SkeletalMeshComponent):
        try:
            # Try new API first
            mesh = component.get_skinned_asset()
            if mesh:
                info["skeletal_mesh"] = mesh.get_name()
                info["skeletal_mesh_path"] = mesh.get_path_name()
        except:
            try:
                # Fallback to old API
                mesh = component.skeletal_mesh
                if mesh:
                    info["skeletal_mesh"] = mesh.get_name()
                    info["skeletal_mesh_path"] = mesh.get_path_name()
            except:
                pass

        try:
            anim_class = component.get_editor_property("anim_class")
            if anim_class:
                info["anim_class"] = anim_class.get_name()
        except:
            pass

    # StaticMeshComponent specific
    if isinstance(component, unreal.StaticMeshComponent):
        try:
            mesh = component.get_editor_property("static_mesh")
            if mesh:
                info["static_mesh"] = mesh.get_name()
                info["static_mesh_path"] = mesh.get_path_name()
        except:
            pass

    return info

def export_blueprint(bp_path):
    """Export comprehensive state of a Blueprint"""
    bp = unreal.load_asset(bp_path)
    if not bp:
        return {"error": f"Could not load {bp_path}"}

    result = {
        "path": bp_path,
        "blueprint_name": bp_path.split("/")[-1],
    }

    # Get generated class
    gen_class = bp.generated_class()
    if not gen_class:
        result["error"] = "No generated class"
        return result

    result["generated_class"] = gen_class.get_name()
    result["generated_class_path"] = gen_class.get_path_name()

    # Get class hierarchy
    result["class_hierarchy"] = get_class_hierarchy(gen_class)

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        result["error"] = "No CDO"
        return result

    # Get all components from CDO
    result["components"] = []
    try:
        all_components = cdo.get_components_by_class(unreal.ActorComponent)
        for comp in all_components:
            comp_info = get_component_info(comp)
            result["components"].append(comp_info)
    except Exception as e:
        result["components_error"] = str(e)

    # Try to get SCS (SimpleConstructionScript) components
    result["scs_components"] = []
    try:
        scs = bp.get_editor_property("simple_construction_script")
        if scs:
            all_nodes = scs.get_all_nodes()
            for node in all_nodes:
                try:
                    comp_template = node.get_editor_property("component_template")
                    if comp_template:
                        node_info = {
                            "variable_name": str(node.get_editor_property("internal_variable_name")),
                        }
                        node_info.update(get_component_info(comp_template))
                        result["scs_components"].append(node_info)
                except:
                    pass
    except:
        pass

    return result

def main():
    # Determine which project we're running in
    project_name = "unknown"
    try:
        # Check project path
        project_path = unreal.Paths.get_project_file_path()
        if "bp_only" in project_path.lower():
            project_name = "bp_only"
        elif "slfconversion" in project_path.lower():
            project_name = "SLFConversion"
    except:
        pass

    print(f"\n{'='*60}")
    print(f"BLUEPRINT STATE EXPORT - {project_name}")
    print(f"{'='*60}")

    all_exports = {}

    for bp_path in BLUEPRINTS_TO_EXPORT:
        print(f"\nExporting: {bp_path}")
        export_data = export_blueprint(bp_path)
        bp_name = bp_path.split("/")[-1]
        all_exports[bp_name] = export_data

        if "error" in export_data:
            print(f"  ERROR: {export_data['error']}")
        else:
            print(f"  Class: {export_data.get('generated_class', 'Unknown')}")
            print(f"  Components: {len(export_data.get('components', []))}")
            print(f"  SCS Components: {len(export_data.get('scs_components', []))}")

            # Print hierarchy
            hierarchy = export_data.get('class_hierarchy', [])
            if hierarchy:
                print(f"  Hierarchy: {' -> '.join([h['name'] for h in hierarchy[:5]])}")

    # Save to file
    output_file = f"C:/scripts/SLFConversion/migration_cache/blueprint_state_{project_name}.json"
    with open(output_file, 'w') as f:
        json.dump(all_exports, f, indent=2)

    print(f"\n{'='*60}")
    print(f"Saved to: {output_file}")
    print(f"{'='*60}")

if __name__ == "__main__":
    main()
