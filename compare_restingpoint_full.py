import unreal
import json
import os

def export_restingpoint_full():
    """Export FULL B_RestingPoint Blueprint data for comparison"""

    output_path = "C:/scripts/slfconversion/migration_cache/restingpoint_full_export.json"

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return

    data = {
        "blueprint_path": bp_path,
        "parent_class": None,
        "components_scs": [],
        "variables": [],
        "cdo_properties": {},
        "event_dispatchers": [],
        "functions": []
    }

    # Get generated class
    gen_class = bp.generated_class()
    if not gen_class:
        print("ERROR: No generated class")
        return

    # Get parent class via Blueprint's parent_class property
    try:
        parent = bp.get_editor_property('parent_class')
        if parent:
            data["parent_class"] = str(parent)
            print(f"Parent class: {data['parent_class']}")
    except:
        print("Could not get parent class")

    # Get CDO
    cdo = unreal.get_default_object(gen_class)

    # Get SCS components using the subsystem
    print("\n=== SIMPLE CONSTRUCTION SCRIPT COMPONENTS ===")

    # Try to get components from CDO
    if cdo:
        try:
            components = cdo.get_components_by_class(unreal.ActorComponent)
            print(f"Found {len(components)} components via CDO")

            for comp in components:
                comp_info = {
                    "name": comp.get_name(),
                    "class": comp.get_class().get_name(),
                    "class_path": comp.get_class().get_path_name(),
                    "properties": {}
                }

                # Get specific properties based on component type
                class_name = comp.get_class().get_name()

                if "NiagaraComponent" in class_name:
                    try:
                        asset = comp.get_editor_property('asset')
                        if asset:
                            comp_info["properties"]["asset"] = asset.get_path_name()
                            print(f"  {comp.get_name()} -> NiagaraSystem: {asset.get_path_name()}")
                        else:
                            comp_info["properties"]["asset"] = None
                            print(f"  {comp.get_name()} -> NiagaraSystem: NONE")

                        auto_activate = comp.get_editor_property('auto_activate')
                        comp_info["properties"]["auto_activate"] = auto_activate
                    except Exception as e:
                        comp_info["properties"]["error"] = str(e)
                        print(f"  {comp.get_name()} -> ERROR: {e}")

                elif "SceneComponent" in class_name or "BillboardComponent" in class_name:
                    try:
                        rel_loc = comp.get_editor_property('relative_location')
                        rel_rot = comp.get_editor_property('relative_rotation')
                        comp_info["properties"]["relative_location"] = f"{rel_loc.x},{rel_loc.y},{rel_loc.z}"
                        comp_info["properties"]["relative_rotation"] = f"{rel_rot.pitch},{rel_rot.yaw},{rel_rot.roll}"
                        print(f"  {comp.get_name()} -> Loc: {rel_loc}, Rot: {rel_rot}")
                    except Exception as e:
                        comp_info["properties"]["error"] = str(e)

                elif "SpringArmComponent" in class_name:
                    try:
                        arm_length = comp.get_editor_property('target_arm_length')
                        comp_info["properties"]["target_arm_length"] = arm_length
                        print(f"  {comp.get_name()} -> ArmLength: {arm_length}")
                    except Exception as e:
                        comp_info["properties"]["error"] = str(e)

                elif "PointLightComponent" in class_name:
                    try:
                        intensity = comp.get_editor_property('intensity')
                        light_color = comp.get_editor_property('light_color')
                        comp_info["properties"]["intensity"] = intensity
                        comp_info["properties"]["light_color"] = f"{light_color.r},{light_color.g},{light_color.b},{light_color.a}"
                        print(f"  {comp.get_name()} -> Intensity: {intensity}")
                    except Exception as e:
                        comp_info["properties"]["error"] = str(e)

                elif "CameraComponent" in class_name:
                    try:
                        fov = comp.get_editor_property('field_of_view')
                        comp_info["properties"]["field_of_view"] = fov
                        print(f"  {comp.get_name()} -> FOV: {fov}")
                    except Exception as e:
                        comp_info["properties"]["error"] = str(e)

                elif "StaticMeshComponent" in class_name:
                    try:
                        mesh = comp.get_editor_property('static_mesh')
                        if mesh:
                            comp_info["properties"]["static_mesh"] = mesh.get_path_name()
                            print(f"  {comp.get_name()} -> Mesh: {mesh.get_path_name()}")
                        else:
                            comp_info["properties"]["static_mesh"] = None
                            print(f"  {comp.get_name()} -> Mesh: NONE")
                    except Exception as e:
                        comp_info["properties"]["error"] = str(e)

                elif "SphereComponent" in class_name:
                    try:
                        radius = comp.get_editor_property('sphere_radius')
                        comp_info["properties"]["sphere_radius"] = radius
                        print(f"  {comp.get_name()} -> Radius: {radius}")
                    except Exception as e:
                        comp_info["properties"]["error"] = str(e)

                data["components_scs"].append(comp_info)

        except Exception as e:
            print(f"ERROR getting components: {e}")

    # Get CDO properties
    print("\n=== CDO PROPERTIES ===")
    cdo_props = [
        'is_activated', 'can_be_traced', 'interaction_text', 'interactable_display_name',
        'location_name', 'sitting_angle', 'force_face_sitting_actor',
        'camera_distance', 'camera_offset', 'camera_rotation',
        'items_to_replenish', 'stats_to_replenish', 'discover_interaction_montage'
    ]

    if cdo:
        for prop in cdo_props:
            try:
                val = cdo.get_editor_property(prop)
                if val is not None:
                    if hasattr(val, 'get_path_name'):
                        data["cdo_properties"][prop] = val.get_path_name()
                    elif hasattr(val, '__str__'):
                        data["cdo_properties"][prop] = str(val)
                    else:
                        data["cdo_properties"][prop] = repr(val)
                    print(f"  {prop}: {data['cdo_properties'][prop]}")
                else:
                    data["cdo_properties"][prop] = None
                    print(f"  {prop}: None")
            except Exception as e:
                data["cdo_properties"][prop] = f"ERROR: {e}"

    # Save to JSON
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=2)

    print(f"\n\nSaved to: {output_path}")

    # Also print summary
    print("\n" + "="*80)
    print("SUMMARY")
    print("="*80)
    print(f"Parent Class: {data['parent_class']}")
    print(f"SCS Components: {len(data['components_scs'])}")
    for comp in data['components_scs']:
        print(f"  - {comp['name']} ({comp['class']})")
        if 'asset' in comp.get('properties', {}):
            print(f"      Asset: {comp['properties']['asset']}")

if __name__ == "__main__":
    export_restingpoint_full()
