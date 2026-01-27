"""
Compare enemy weapon configurations between bp_only and SLFConversion.
Extracts ChildActorClass and attachment settings from enemy Blueprints.

Run on BOTH projects to compare.
"""
import unreal
import json
import os

# Enemies to compare
ENEMY_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
]

def get_component_details(bp):
    """Get detailed component information from Blueprint SCS"""
    components = []

    try:
        scs = bp.get_editor_property("simple_construction_script")
        if not scs:
            return components

        all_nodes = scs.get_all_nodes()
        for node in all_nodes:
            try:
                comp_template = node.get_editor_property("component_template")
                if not comp_template:
                    continue

                comp_info = {
                    "variable_name": str(node.get_editor_property("internal_variable_name")),
                    "name": comp_template.get_name(),
                    "class": comp_template.get_class().get_name(),
                }

                # Get attachment info for scene components
                if isinstance(comp_template, unreal.SceneComponent):
                    try:
                        parent_name = node.get_editor_property("parent_component_or_variable_name")
                        if parent_name:
                            comp_info["attach_to"] = str(parent_name)

                        socket = node.get_editor_property("attach_to_name")
                        if socket and str(socket) != "None":
                            comp_info["attach_socket"] = str(socket)

                        # Relative transform
                        rel_loc = comp_template.get_editor_property("relative_location")
                        rel_rot = comp_template.get_editor_property("relative_rotation")
                        rel_scale = comp_template.get_editor_property("relative_scale3d")

                        comp_info["relative_location"] = f"({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})"
                        comp_info["relative_rotation"] = f"({rel_rot.pitch:.2f}, {rel_rot.yaw:.2f}, {rel_rot.roll:.2f})"
                        comp_info["relative_scale"] = f"({rel_scale.x:.2f}, {rel_scale.y:.2f}, {rel_scale.z:.2f})"
                    except Exception as e:
                        comp_info["transform_error"] = str(e)

                # ChildActorComponent - get the child actor class
                if isinstance(comp_template, unreal.ChildActorComponent):
                    try:
                        child_class = comp_template.get_editor_property("child_actor_class")
                        if child_class:
                            comp_info["child_actor_class"] = child_class.get_name()
                            comp_info["child_actor_class_path"] = child_class.get_path_name()
                    except Exception as e:
                        comp_info["child_actor_error"] = str(e)

                # Get component class defaults for any property values
                if comp_info.get("class") == "ChildActorComponent":
                    try:
                        # Also try to get from component's direct properties
                        all_props = dir(comp_template)
                        relevant_props = [p for p in all_props if 'child' in p.lower() or 'actor' in p.lower()]
                        comp_info["available_props"] = relevant_props[:10]  # First 10
                    except:
                        pass

                components.append(comp_info)
            except Exception as e:
                components.append({"error": str(e)})
    except Exception as e:
        return [{"scs_error": str(e)}]

    return components

def export_enemy_config(bp_path):
    """Export enemy configuration including weapon setup"""
    bp = unreal.load_asset(bp_path)
    if not bp:
        return {"error": f"Could not load {bp_path}"}

    result = {
        "path": bp_path,
        "name": bp_path.split("/")[-1],
    }

    # Get generated class
    gen_class = bp.generated_class()
    if not gen_class:
        result["error"] = "No generated class"
        return result

    result["generated_class"] = gen_class.get_name()
    result["generated_class_path"] = gen_class.get_path_name()

    # Get parent class
    try:
        parent = gen_class.get_super_class()
        if parent:
            result["parent_class"] = parent.get_name()
            result["parent_class_path"] = parent.get_path_name()
    except:
        pass

    # Get SCS components (including Weapon)
    result["scs_components"] = get_component_details(bp)

    # Get CDO properties
    cdo = unreal.get_default_object(gen_class)
    if cdo:
        result["cdo_class"] = cdo.get_class().get_name()

        # Try to get mesh
        try:
            mesh = cdo.get_editor_property("mesh")
            if mesh:
                result["mesh_component"] = mesh.get_name()
                try:
                    skeletal_mesh = mesh.get_skinned_asset()
                    if skeletal_mesh:
                        result["skeletal_mesh"] = skeletal_mesh.get_name()
                except:
                    pass

                # Animation class
                try:
                    anim_class = mesh.get_editor_property("anim_class")
                    if anim_class:
                        result["anim_class"] = anim_class.get_name()
                except:
                    pass
        except:
            pass

    return result

def main():
    # Determine project
    project_name = "unknown"
    try:
        project_path = unreal.Paths.get_project_file_path()
        if "bp_only" in project_path.lower():
            project_name = "bp_only"
        elif "slfconversion" in project_path.lower():
            project_name = "SLFConversion"
    except:
        pass

    print(f"\n{'='*70}")
    print(f"ENEMY WEAPON COMPARISON - {project_name}")
    print(f"{'='*70}")

    all_exports = {}

    for bp_path in ENEMY_BLUEPRINTS:
        print(f"\n--- {bp_path.split('/')[-1]} ---")
        export_data = export_enemy_config(bp_path)
        bp_name = bp_path.split("/")[-1]
        all_exports[bp_name] = export_data

        if "error" in export_data:
            print(f"  ERROR: {export_data['error']}")
        else:
            print(f"  Parent: {export_data.get('parent_class', 'Unknown')}")
            print(f"  AnimClass: {export_data.get('anim_class', 'None')}")
            print(f"  Mesh: {export_data.get('skeletal_mesh', 'None')}")

            # Print SCS components
            scs = export_data.get('scs_components', [])
            print(f"  SCS Components: {len(scs)}")
            for comp in scs:
                comp_class = comp.get('class', 'Unknown')
                comp_name = comp.get('name', 'Unknown')
                if comp_class == "ChildActorComponent":
                    child_class = comp.get('child_actor_class', 'NONE')
                    attach_socket = comp.get('attach_socket', 'NONE')
                    rel_rot = comp.get('relative_rotation', 'N/A')
                    print(f"    - {comp_name} (ChildActor)")
                    print(f"        ChildActorClass: {child_class}")
                    print(f"        AttachSocket: {attach_socket}")
                    print(f"        RelativeRotation: {rel_rot}")
                else:
                    print(f"    - {comp_name} ({comp_class})")

    # Save to file
    output_file = f"C:/scripts/SLFConversion/migration_cache/enemy_weapons_{project_name}.json"
    with open(output_file, 'w') as f:
        json.dump(all_exports, f, indent=2)

    print(f"\n{'='*70}")
    print(f"Saved to: {output_file}")
    print(f"{'='*70}")

if __name__ == "__main__":
    main()
