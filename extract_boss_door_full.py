"""
Extract complete B_BossDoor configuration from bp_only project.
Uses export_text and SCS traversal to get ALL component data.
"""
import unreal
import json
import os
import re

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/boss_door_bp_only_config.json"

def vector_to_dict(vec):
    """Convert FVector to dict"""
    return {"X": float(vec.x), "Y": float(vec.y), "Z": float(vec.z)}

def rotator_to_dict(rot):
    """Convert FRotator to dict"""
    return {"Pitch": float(rot.pitch), "Yaw": float(rot.yaw), "Roll": float(rot.roll)}

def get_safe_property(obj, prop_name, default=None):
    """Safely get a property value"""
    try:
        return obj.get_editor_property(prop_name)
    except:
        return default

def extract_component_from_scs(node, bp_asset):
    """Extract detailed component info from SCS node"""
    info = {}

    template = node.component_template
    if not template:
        return None

    info = {
        "Name": template.get_name(),
        "Class": template.get_class().get_name(),
        "ClassPath": template.get_class().get_path_name()
    }

    # Get relative location/rotation/scale
    rel_loc = get_safe_property(template, "relative_location")
    if rel_loc:
        info["RelativeLocation"] = vector_to_dict(rel_loc)

    rel_rot = get_safe_property(template, "relative_rotation")
    if rel_rot:
        info["RelativeRotation"] = rotator_to_dict(rel_rot)

    rel_scale = get_safe_property(template, "relative_scale3d")
    if rel_scale:
        info["RelativeScale3D"] = vector_to_dict(rel_scale)

    # Get parent attachment
    try:
        parent_name = node.get_editor_property("parent_component_or_variable_name")
        if parent_name:
            info["AttachedTo"] = str(parent_name)
    except:
        pass

    # Class-specific properties
    class_name = template.get_class().get_name()

    if class_name == "StaticMeshComponent":
        mesh = get_safe_property(template, "static_mesh")
        if mesh:
            info["StaticMesh"] = mesh.get_path_name()

        # Collision
        try:
            info["CollisionEnabled"] = str(template.get_collision_enabled())
        except:
            pass

        body = get_safe_property(template, "body_instance")
        if body:
            try:
                info["CollisionProfileName"] = str(body.get_editor_property("collision_profile_name"))
            except:
                pass

    elif class_name == "NiagaraComponent":
        asset = get_safe_property(template, "asset")
        if asset:
            info["NiagaraAsset"] = asset.get_path_name()

    elif class_name == "BillboardComponent":
        sprite = get_safe_property(template, "sprite")
        if sprite:
            info["Sprite"] = sprite.get_path_name()

    elif class_name == "SkeletalMeshComponent":
        mesh = get_safe_property(template, "skeletal_mesh")
        if mesh:
            info["SkeletalMesh"] = mesh.get_path_name()

    elif class_name == "ArrowComponent":
        info["ArrowLength"] = get_safe_property(template, "arrow_length")
        info["ArrowSize"] = get_safe_property(template, "arrow_size")

    return info

def export_blueprint_full(bp_path, name):
    """Export full Blueprint data including parent SCS"""
    print(f"\n--- Processing {name} ---")

    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        print(f"  ERROR: Could not load {bp_path}")
        return None

    gen_class = bp_asset.generated_class()
    if not gen_class:
        print(f"  ERROR: No generated class")
        return None

    result = {
        "BlueprintPath": bp_path,
        "ClassName": gen_class.get_name(),
        "ParentClass": None,
        "Components": [],
        "InheritedComponents": [],
        "Variables": {}
    }

    # Get parent class name
    try:
        parent = gen_class.get_super_class()
        if parent:
            result["ParentClass"] = parent.get_name()
    except:
        pass

    # Get SCS components for this Blueprint
    scs = get_safe_property(bp_asset, "simple_construction_script")
    if scs:
        try:
            # Get root nodes only (top-level components)
            root_nodes = scs.get_editor_property("root_nodes")
            print(f"  Root nodes: {len(root_nodes) if root_nodes else 0}")

            # Get all nodes
            all_nodes = scs.get_all_nodes()
            print(f"  All SCS nodes: {len(all_nodes) if all_nodes else 0}")

            if all_nodes:
                for node in all_nodes:
                    comp_info = extract_component_from_scs(node, bp_asset)
                    if comp_info:
                        result["Components"].append(comp_info)
                        print(f"    - {comp_info['Name']} ({comp_info['Class']})")
                        if 'RelativeScale3D' in comp_info:
                            s = comp_info['RelativeScale3D']
                            print(f"      Scale: {s['X']}, {s['Y']}, {s['Z']}")
                        if 'StaticMesh' in comp_info:
                            print(f"      Mesh: {comp_info['StaticMesh']}")
        except Exception as e:
            print(f"  Error extracting SCS: {e}")
    else:
        print("  No SCS found")

    # Also check inherited components from parent SCS
    if result["ParentClass"]:
        parent_bp_path = None
        if result["ParentClass"] == "B_Door_C":
            parent_bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
        elif result["ParentClass"] == "B_Interactable_C":
            parent_bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"

        if parent_bp_path:
            parent_bp = unreal.load_asset(parent_bp_path)
            if parent_bp:
                parent_scs = get_safe_property(parent_bp, "simple_construction_script")
                if parent_scs:
                    try:
                        parent_nodes = parent_scs.get_all_nodes()
                        if parent_nodes:
                            print(f"  Inherited from {result['ParentClass']}:")
                            for node in parent_nodes:
                                comp_info = extract_component_from_scs(node, parent_bp)
                                if comp_info:
                                    comp_info["InheritedFrom"] = result["ParentClass"]
                                    result["InheritedComponents"].append(comp_info)
                                    print(f"    - {comp_info['Name']} ({comp_info['Class']})")
                                    if 'RelativeScale3D' in comp_info:
                                        s = comp_info['RelativeScale3D']
                                        print(f"      Scale: {s['X']}, {s['Y']}, {s['Z']}")
                                    if 'StaticMesh' in comp_info:
                                        print(f"      Mesh: {comp_info['StaticMesh']}")
                    except Exception as e:
                        print(f"  Error getting parent SCS: {e}")

    # Get CDO for variables
    cdo = unreal.get_default_object(gen_class)
    if cdo:
        common_props = [
            ("can_be_traced", "CanBeTraced"),
            ("is_activated", "IsActivated"),
            ("interactable_display_name", "InteractableDisplayName"),
            ("interaction_text", "InteractionText"),
            ("yaw_rotation_amount", "YawRotationAmount"),
            ("play_rate", "PlayRate"),
            ("move_to_distance", "MoveToDistance"),
        ]

        for py_name, display_name in common_props:
            val = get_safe_property(cdo, py_name)
            if val is not None:
                result["Variables"][display_name] = str(val)

    return result

def main():
    print("=" * 60)
    print("Extracting B_BossDoor configuration from bp_only")
    print("=" * 60)

    # Process in order: base -> derived
    blueprints = [
        ("B_Interactable", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"),
        ("B_Door", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"),
        ("B_BossDoor", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"),
    ]

    all_data = {}

    for name, path in blueprints:
        data = export_blueprint_full(path, name)
        if data:
            all_data[name] = data

    # Save
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(all_data, f, indent=2)

    print(f"\n\nSaved to: {OUTPUT_PATH}")
    print("=" * 60)

if __name__ == "__main__":
    main()
