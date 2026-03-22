"""
Compare B_Door between SLFConversion - extract full component and mesh data.
"""
import unreal
import json

results = {"door_data": {}}

def extract_door_data(bp_path):
    """Extract comprehensive door data"""
    data = {
        "path": bp_path,
        "exists": False,
        "parent_class": None,
        "components": [],
        "scs_components": [],
        "properties": {}
    }

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        data["error"] = f"Could not load {bp_path}"
        return data

    data["exists"] = True

    # Get parent class name from Blueprint
    try:
        parent_class = bp.get_editor_property('parent_class')
        if parent_class:
            data["parent_class"] = str(parent_class.get_name())
    except:
        pass

    gen_class = bp.generated_class()
    if not gen_class:
        data["error"] = "No generated class"
        return data

    data["class_name"] = gen_class.get_name()

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        data["error"] = "No CDO"
        return data

    # Get all components from CDO
    all_comps = cdo.get_components_by_class(unreal.ActorComponent)
    for comp in all_comps:
        comp_data = {
            "name": comp.get_name(),
            "class": comp.get_class().get_name(),
        }

        # Check for StaticMeshComponent
        if isinstance(comp, unreal.StaticMeshComponent):
            mesh = comp.static_mesh
            comp_data["static_mesh"] = mesh.get_path_name() if mesh else "None"
            comp_data["mesh_name"] = mesh.get_name() if mesh else "NO_MESH"
            comp_data["visible"] = comp.is_visible()
            comp_data["collision_enabled"] = str(comp.get_collision_enabled())
            rel_loc = comp.get_relative_location()
            rel_rot = comp.get_relative_rotation()
            comp_data["relative_location"] = f"X={rel_loc.x:.1f} Y={rel_loc.y:.1f} Z={rel_loc.z:.1f}"
            comp_data["relative_rotation"] = f"P={rel_rot.pitch:.1f} Y={rel_rot.yaw:.1f} R={rel_rot.roll:.1f}"
        elif isinstance(comp, unreal.SceneComponent):
            rel_loc = comp.get_relative_location()
            comp_data["relative_location"] = f"X={rel_loc.x:.1f} Y={rel_loc.y:.1f} Z={rel_loc.z:.1f}"

        data["components"].append(comp_data)

    # Try to get SCS (SimpleConstructionScript) components
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            all_nodes = scs.get_all_nodes()
            for node in all_nodes:
                if node:
                    template = node.component_template
                    if template:
                        scs_data = {
                            "name": template.get_name(),
                            "class": template.get_class().get_name()
                        }
                        if isinstance(template, unreal.StaticMeshComponent):
                            mesh = template.static_mesh
                            scs_data["static_mesh"] = mesh.get_path_name() if mesh else "None"
                            scs_data["mesh_name"] = mesh.get_name() if mesh else "NO_MESH"
                        data["scs_components"].append(scs_data)
    except Exception as e:
        data["scs_error"] = str(e)

    # Check specific door properties
    props_to_check = [
        ('default_door_mesh', 'TSoftObjectPtr'),
        ('default_door_frame_mesh', 'TSoftObjectPtr'),
        ('move_to_distance', 'double'),
        ('yaw_rotation_amount', 'double'),
        ('play_rate', 'double'),
        ('open_forwards', 'bool'),
    ]

    for prop_name, prop_type in props_to_check:
        try:
            val = cdo.get_editor_property(prop_name)
            if val is not None:
                if 'Soft' in prop_type:
                    data["properties"][prop_name] = str(val)
                else:
                    data["properties"][prop_name] = str(val)
            else:
                data["properties"][prop_name] = "None"
        except Exception as e:
            data["properties"][prop_name] = f"N/A"

    return data

# Extract B_Door
unreal.log("=" * 70)
unreal.log("EXTRACTING B_Door DATA")
unreal.log("=" * 70)

results["door_data"] = extract_door_data(
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
)

door = results["door_data"]
unreal.log(f"Blueprint: {door.get('path')}")
unreal.log(f"Parent Class: {door.get('parent_class', 'Unknown')}")
unreal.log(f"Generated Class: {door.get('class_name', 'Unknown')}")
unreal.log("")
unreal.log("CDO COMPONENTS:")
for comp in door.get("components", []):
    mesh = comp.get("mesh_name", comp.get("static_mesh", "N/A"))
    visible = comp.get("visible", "N/A")
    unreal.log(f"  [{comp['class']}] {comp['name']}")
    if "static_mesh" in comp:
        unreal.log(f"      Mesh: {mesh}")
        unreal.log(f"      Visible: {visible}")
        unreal.log(f"      Location: {comp.get('relative_location', 'N/A')}")

unreal.log("")
unreal.log("SCS COMPONENTS (Blueprint-owned):")
for scs in door.get("scs_components", []):
    mesh = scs.get("mesh_name", "N/A")
    unreal.log(f"  [{scs['class']}] {scs['name']}: {mesh}")

unreal.log("")
unreal.log("PROPERTIES:")
for prop, val in door.get("properties", {}).items():
    unreal.log(f"  {prop}: {val}")

# Save
output_path = "C:/scripts/SLFConversion/migration_cache/door_slfconversion_full.json"
with open(output_path, "w") as f:
    json.dump(results, f, indent=2)

unreal.log("")
unreal.log(f"Saved to {output_path}")
