"""
Extract complete B_BossDoor configuration from bp_only project.
Run against bp_only project to get all component transforms, meshes, and CDO defaults.
"""
import unreal
import json
import os

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/boss_door_bp_only_config.json"

def vector_to_dict(vec):
    """Convert FVector to dict"""
    return {"X": vec.x, "Y": vec.y, "Z": vec.z}

def rotator_to_dict(rot):
    """Convert FRotator to dict"""
    return {"Pitch": rot.pitch, "Yaw": rot.yaw, "Roll": rot.roll}

def transform_to_dict(transform):
    """Convert FTransform to dict"""
    return {
        "Location": vector_to_dict(transform.translation),
        "Rotation": rotator_to_dict(transform.rotation.rotator()),
        "Scale3D": vector_to_dict(transform.scale3d)
    }

def get_safe_property(obj, prop_name, default=None):
    """Safely get a property value"""
    try:
        return obj.get_editor_property(prop_name)
    except:
        return default

def extract_component_details(component):
    """Extract detailed information from a component"""
    info = {
        "Name": component.get_name(),
        "Class": component.get_class().get_name(),
        "ClassPath": component.get_class().get_path_name()
    }

    # Get relative location/rotation/scale directly
    rel_loc = get_safe_property(component, "relative_location")
    if rel_loc:
        info["RelativeLocation"] = vector_to_dict(rel_loc)

    rel_rot = get_safe_property(component, "relative_rotation")
    if rel_rot:
        info["RelativeRotation"] = rotator_to_dict(rel_rot)

    rel_scale = get_safe_property(component, "relative_scale3d")
    if rel_scale:
        info["RelativeScale3D"] = vector_to_dict(rel_scale)

    # For StaticMeshComponent - get mesh and collision
    class_name = component.get_class().get_name()

    if class_name == "StaticMeshComponent":
        mesh = get_safe_property(component, "static_mesh")
        if mesh:
            info["StaticMesh"] = mesh.get_path_name()

        coll_enabled = get_safe_property(component, "collision_enabled")
        if coll_enabled is not None:
            info["CollisionEnabled"] = str(coll_enabled)

        body_collision = get_safe_property(component, "body_instance")
        if body_collision:
            try:
                coll_profile = body_collision.get_editor_property("collision_profile_name")
                info["CollisionProfileName"] = str(coll_profile)
            except:
                pass
            try:
                object_type = body_collision.get_editor_property("object_type")
                info["ObjectType"] = str(object_type)
            except:
                pass

    # For NiagaraComponent - get asset
    if class_name == "NiagaraComponent":
        asset = get_safe_property(component, "asset")
        if asset:
            info["NiagaraAsset"] = asset.get_path_name()

    # For BillboardComponent
    if class_name == "BillboardComponent":
        sprite = get_safe_property(component, "sprite")
        if sprite:
            info["Sprite"] = sprite.get_path_name()

    return info

def extract_scs_node_details(node):
    """Extract details from a SimpleConstructionScript node"""
    info = {}

    template = node.component_template
    if template:
        info = extract_component_details(template)

        # Check for parent attachment
        parent_name = get_safe_property(node, "parent_component_or_variable_name")
        if parent_name:
            info["AttachedTo"] = str(parent_name)

    return info

def export_blueprint_cdo(bp_path):
    """Export CDO properties for a Blueprint"""
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        print(f"ERROR: Could not load {bp_path}")
        return None

    gen_class = bp_asset.generated_class()
    if not gen_class:
        print(f"ERROR: No generated class for {bp_path}")
        return None

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        print(f"ERROR: No CDO for {bp_path}")
        return None

    # Get parent class name
    parent_class_name = None
    try:
        parent = gen_class.get_super_class()
        if parent:
            parent_class_name = parent.get_name()
    except:
        pass

    # Use EditorAssetLibrary to export asset text
    cdo_text = ""
    try:
        cdo_text = unreal.EditorAssetLibrary.export_text(bp_path)[:30000] if hasattr(unreal.EditorAssetLibrary, 'export_text') else ""
    except:
        pass

    result = {
        "BlueprintPath": bp_path,
        "ClassName": gen_class.get_name(),
        "ParentClass": parent_class_name,
        "Components": [],
        "Variables": {}
    }

    # Extract SCS components
    scs = get_safe_property(bp_asset, "simple_construction_script")
    if scs:
        try:
            nodes = scs.get_all_nodes()
            for node in nodes:
                template = node.component_template
                if template:
                    comp_info = extract_scs_node_details(node)
                    result["Components"].append(comp_info)
        except Exception as e:
            print(f"  Error extracting SCS: {e}")

    # Try to get common properties from CDO
    common_props = [
        ("can_be_traced", "CanBeTraced"),
        ("is_activated", "IsActivated"),
        ("interactable_display_name", "InteractableDisplayName"),
        ("interaction_text", "InteractionText"),
        ("yaw_rotation_amount", "YawRotationAmount"),
        ("play_rate", "PlayRate"),
        ("move_to_distance", "MoveToDistance"),
    ]

    for python_prop, display_name in common_props:
        value = get_safe_property(cdo, python_prop)
        if value is not None:
            result["Variables"][display_name] = str(value)

    return result

def main():
    print("=" * 60)
    print("Extracting B_BossDoor configuration from bp_only")
    print("=" * 60)

    blueprints = {
        "B_BossDoor": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor",
        "B_Door": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door",
        "B_Interactable": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"
    }

    all_data = {}

    for name, path in blueprints.items():
        print(f"\n--- Processing {name} ---")
        data = export_blueprint_cdo(path)
        if data:
            all_data[name] = data
            print(f"  Components found: {len(data['Components'])}")
            for comp in data['Components']:
                print(f"    - {comp['Name']} ({comp['Class']})")
                if 'RelativeScale3D' in comp:
                    scale = comp['RelativeScale3D']
                    print(f"      Scale: X={scale['X']}, Y={scale['Y']}, Z={scale['Z']}")
                if 'RelativeLocation' in comp:
                    loc = comp['RelativeLocation']
                    print(f"      Location: X={loc['X']}, Y={loc['Y']}, Z={loc['Z']}")
                if 'StaticMesh' in comp:
                    print(f"      Mesh: {comp['StaticMesh']}")
                if 'NiagaraAsset' in comp:
                    print(f"      Niagara: {comp['NiagaraAsset']}")

            # Print variables
            if data['Variables']:
                print(f"  Variables:")
                for k, v in data['Variables'].items():
                    print(f"    {k}: {v}")

    # Save to JSON
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(all_data, f, indent=2)

    print(f"\n\nSaved configuration to: {OUTPUT_PATH}")
    print("=" * 60)

if __name__ == "__main__":
    main()
