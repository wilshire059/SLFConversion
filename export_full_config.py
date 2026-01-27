#!/usr/bin/env python3
"""
COMPREHENSIVE CONFIG EXPORT
Exports character, weapon, socket, and IK configuration for comparison
"""

import unreal
import json
import os

OUTPUT_DIR = "C:/scripts/slfconversion/migration_cache/config_export/"

def log(msg):
    unreal.log_warning(str(msg))

def ensure_dir(path):
    if not os.path.exists(path):
        os.makedirs(path)

def export_skeleton_sockets(skeleton_path):
    """Export all socket names and transforms from a skeleton"""
    skeleton = unreal.EditorAssetLibrary.load_asset(skeleton_path)
    if not skeleton:
        return {"error": f"Could not load skeleton: {skeleton_path}"}

    result = {
        "path": skeleton_path,
        "sockets": []
    }

    # Get socket count and data
    try:
        # Get all sockets from the skeleton
        socket_names = skeleton.get_editor_property("sockets")
        if socket_names:
            for socket in socket_names:
                socket_data = {
                    "name": str(socket.get_editor_property("socket_name")),
                    "bone_name": str(socket.get_editor_property("bone_name")),
                    "relative_location": None,
                    "relative_rotation": None,
                    "relative_scale": None
                }

                try:
                    loc = socket.get_editor_property("relative_location")
                    socket_data["relative_location"] = {"x": loc.x, "y": loc.y, "z": loc.z}
                except: pass

                try:
                    rot = socket.get_editor_property("relative_rotation")
                    socket_data["relative_rotation"] = {"pitch": rot.pitch, "yaw": rot.yaw, "roll": rot.roll}
                except: pass

                try:
                    scale = socket.get_editor_property("relative_scale")
                    socket_data["relative_scale"] = {"x": scale.x, "y": scale.y, "z": scale.z}
                except: pass

                result["sockets"].append(socket_data)
    except Exception as e:
        result["socket_error"] = str(e)

    return result

def export_skeletal_mesh_sockets(mesh_path):
    """Export sockets from a skeletal mesh asset"""
    mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
    if not mesh:
        return {"error": f"Could not load mesh: {mesh_path}"}

    result = {
        "path": mesh_path,
        "sockets": []
    }

    try:
        # Get skeleton
        skeleton = mesh.get_editor_property("skeleton")
        if skeleton:
            result["skeleton_path"] = skeleton.get_path_name()

            # Get sockets from skeleton
            sockets = skeleton.get_editor_property("sockets")
            if sockets:
                for socket in sockets:
                    socket_data = {
                        "name": str(socket.get_editor_property("socket_name")),
                        "bone_name": str(socket.get_editor_property("bone_name")),
                    }

                    try:
                        loc = socket.get_editor_property("relative_location")
                        socket_data["location"] = {"x": loc.x, "y": loc.y, "z": loc.z}
                    except: pass

                    try:
                        rot = socket.get_editor_property("relative_rotation")
                        socket_data["rotation"] = {"pitch": rot.pitch, "yaw": rot.yaw, "roll": rot.roll}
                    except: pass

                    result["sockets"].append(socket_data)
    except Exception as e:
        result["error"] = str(e)

    return result

def export_character_config(bp_path):
    """Export full character Blueprint configuration"""
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        return {"error": f"Could not load: {bp_path}"}

    result = {
        "path": bp_path,
        "name": bp_path.split("/")[-1],
        "parent_class": None,
        "class_hierarchy": [],
        "components": [],
        "mesh_info": {},
        "anim_instance": None,
        "ik_settings": {}
    }

    gen_class = bp.generated_class()
    if not gen_class:
        result["error"] = "No generated class"
        return result

    # Get class hierarchy
    current = gen_class
    depth = 0
    while current and depth < 15:
        result["class_hierarchy"].append(current.get_name())
        try:
            parent = bp.get_editor_property("parent_class") if depth == 0 else None
            if parent:
                result["parent_class"] = parent.get_name()
        except: pass
        current = current.get_super_class() if hasattr(current, 'get_super_class') else None
        depth += 1

    # Spawn instance to get runtime component info
    try:
        world = unreal.EditorLevelLibrary.get_editor_world()
        if world:
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, 20000))
            if actor:
                # Get all components
                all_comps = actor.get_components_by_class(unreal.ActorComponent)
                for comp in all_comps:
                    comp_info = {
                        "name": comp.get_name(),
                        "class": comp.get_class().get_name(),
                    }

                    # Scene component transforms
                    if isinstance(comp, unreal.SceneComponent):
                        loc = comp.get_editor_property("relative_location")
                        rot = comp.get_editor_property("relative_rotation")
                        comp_info["location"] = {"x": loc.x, "y": loc.y, "z": loc.z}
                        comp_info["rotation"] = {"pitch": rot.pitch, "yaw": rot.yaw, "roll": rot.roll}

                    # Skeletal mesh specific
                    if isinstance(comp, unreal.SkeletalMeshComponent):
                        mesh = comp.get_editor_property("skeletal_mesh_asset")
                        if mesh:
                            comp_info["skeletal_mesh"] = mesh.get_name()
                            comp_info["skeletal_mesh_path"] = mesh.get_path_name()

                            # Get skeleton info
                            try:
                                skeleton = mesh.get_editor_property("skeleton")
                                if skeleton:
                                    comp_info["skeleton"] = skeleton.get_name()
                                    comp_info["skeleton_path"] = skeleton.get_path_name()
                            except: pass

                        # Get anim instance class
                        try:
                            anim_class = comp.get_editor_property("anim_class")
                            if anim_class:
                                result["anim_instance"] = anim_class.get_name()
                                comp_info["anim_class"] = anim_class.get_name()
                        except: pass

                        # Check socket existence
                        for socket_name in ["hand_r", "hand_l", "weapon_r", "weapon_l", "RightHand", "LeftHand"]:
                            if comp.does_socket_exist(socket_name):
                                if "sockets_found" not in comp_info:
                                    comp_info["sockets_found"] = []

                                # Get socket transform
                                socket_transform = comp.get_socket_transform(socket_name, unreal.RelativeTransformSpace.WORLD)
                                socket_loc = socket_transform.translation
                                socket_rot = socket_transform.rotation.rotator()

                                comp_info["sockets_found"].append({
                                    "name": socket_name,
                                    "world_location": {"x": socket_loc.x, "y": socket_loc.y, "z": socket_loc.z},
                                    "world_rotation": {"pitch": socket_rot.pitch, "yaw": socket_rot.yaw, "roll": socket_rot.roll}
                                })

                        result["mesh_info"] = {
                            "mesh": comp_info.get("skeletal_mesh"),
                            "skeleton": comp_info.get("skeleton"),
                            "anim_class": comp_info.get("anim_class"),
                            "sockets": comp_info.get("sockets_found", [])
                        }

                    result["components"].append(comp_info)

                actor.destroy_actor()
    except Exception as e:
        result["spawn_error"] = str(e)

    return result

def export_weapon_config(bp_path):
    """Export weapon Blueprint configuration including runtime transforms"""
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        return {"error": f"Could not load: {bp_path}"}

    result = {
        "path": bp_path,
        "name": bp_path.split("/")[-1],
        "class_hierarchy": [],
        "cdo_properties": {},
        "runtime_components": []
    }

    gen_class = bp.generated_class()
    if not gen_class:
        result["error"] = "No generated class"
        return result

    # Class hierarchy
    current = gen_class
    depth = 0
    while current and depth < 10:
        result["class_hierarchy"].append(current.get_name())
        current = current.get_super_class() if hasattr(current, 'get_super_class') else None
        depth += 1

    # CDO properties
    cdo = unreal.get_default_object(gen_class)
    if cdo:
        props_to_check = [
            "default_weapon_mesh",
            "default_mesh_relative_location",
            "default_mesh_relative_rotation",
            "default_attachment_rotation_offset",
            "item_info",
        ]

        for prop in props_to_check:
            try:
                value = cdo.get_editor_property(prop)
                if value is not None:
                    if hasattr(value, 'x'):  # Vector
                        result["cdo_properties"][prop] = {"x": value.x, "y": value.y, "z": value.z}
                    elif hasattr(value, 'pitch'):  # Rotator
                        result["cdo_properties"][prop] = {"pitch": value.pitch, "yaw": value.yaw, "roll": value.roll}
                    elif hasattr(value, 'get_name'):  # Object
                        result["cdo_properties"][prop] = value.get_name()
                    else:
                        result["cdo_properties"][prop] = str(value)
            except Exception as e:
                result["cdo_properties"][prop] = f"ERROR: {e}"

    # Spawn and check runtime
    try:
        world = unreal.EditorLevelLibrary.get_editor_world()
        if world:
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, 20000))
            if actor:
                all_comps = actor.get_components_by_class(unreal.SceneComponent)
                for comp in all_comps:
                    comp_info = {
                        "name": comp.get_name(),
                        "class": comp.get_class().get_name(),
                    }

                    loc = comp.get_editor_property("relative_location")
                    rot = comp.get_editor_property("relative_rotation")
                    comp_info["location"] = {"x": loc.x, "y": loc.y, "z": loc.z}
                    comp_info["rotation"] = {"pitch": rot.pitch, "yaw": rot.yaw, "roll": rot.roll}

                    if comp.get_class().get_name() == "StaticMeshComponent":
                        mesh = comp.get_editor_property("static_mesh")
                        comp_info["mesh"] = mesh.get_name() if mesh else "None"

                    result["runtime_components"].append(comp_info)

                actor.destroy_actor()
    except Exception as e:
        result["spawn_error"] = str(e)

    return result

def export_animbp_config(bp_path):
    """Export AnimBlueprint configuration"""
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        return {"error": f"Could not load: {bp_path}"}

    result = {
        "path": bp_path,
        "name": bp_path.split("/")[-1],
        "parent_class": None,
        "target_skeleton": None,
        "variables": []
    }

    try:
        parent = bp.get_editor_property("parent_class")
        if parent:
            result["parent_class"] = parent.get_name()
    except: pass

    try:
        skeleton = bp.get_editor_property("target_skeleton")
        if skeleton:
            result["target_skeleton"] = skeleton.get_name()
            result["target_skeleton_path"] = skeleton.get_path_name()
    except: pass

    return result

def main():
    ensure_dir(OUTPUT_DIR)

    log("=" * 70)
    log("COMPREHENSIVE CONFIG EXPORT")
    log("=" * 70)

    full_export = {
        "project": "SLFConversion",
        "characters": {},
        "weapons": {},
        "animbps": {},
        "skeletons": {}
    }

    # Character paths
    CHARACTER_PATHS = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
        "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter",
    ]

    # Weapon paths
    WEAPON_PATHS = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
    ]

    # AnimBP paths
    ANIMBP_PATHS = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
    ]

    # Skeleton paths
    SKELETON_MESH_PATHS = [
        "/Game/SoulslikeFramework/Demo/_Animations/Mannequin/SKM_Quinn",
        "/Game/SoulslikeFramework/Demo/_Animations/Mannequin/SKM_Manny",
    ]

    # Export characters
    log("\n=== EXPORTING CHARACTERS ===")
    for path in CHARACTER_PATHS:
        name = path.split("/")[-1]
        log(f"Exporting: {name}")
        full_export["characters"][name] = export_character_config(path)

    # Export weapons
    log("\n=== EXPORTING WEAPONS ===")
    for path in WEAPON_PATHS:
        name = path.split("/")[-1]
        log(f"Exporting: {name}")
        full_export["weapons"][name] = export_weapon_config(path)

    # Export AnimBPs
    log("\n=== EXPORTING ANIMBPS ===")
    for path in ANIMBP_PATHS:
        name = path.split("/")[-1]
        log(f"Exporting: {name}")
        full_export["animbps"][name] = export_animbp_config(path)

    # Export skeleton sockets
    log("\n=== EXPORTING SKELETON SOCKETS ===")
    for path in SKELETON_MESH_PATHS:
        name = path.split("/")[-1]
        log(f"Exporting: {name}")
        full_export["skeletons"][name] = export_skeletal_mesh_sockets(path)

    # Save to file
    output_path = OUTPUT_DIR + "slfconversion_full_config.json"
    with open(output_path, 'w') as f:
        json.dump(full_export, f, indent=2)

    log(f"\n=== EXPORT COMPLETE ===")
    log(f"Saved to: {output_path}")

    # Print summary
    log("\n=== QUICK SUMMARY ===")

    char_data = full_export["characters"].get("B_Soulslike_Character", {})
    mesh_info = char_data.get("mesh_info", {})
    log(f"Character mesh: {mesh_info.get('mesh', 'UNKNOWN')}")
    log(f"Character skeleton: {mesh_info.get('skeleton', 'UNKNOWN')}")
    log(f"Character anim class: {mesh_info.get('anim_class', 'UNKNOWN')}")
    log(f"Character sockets found: {[s.get('name') for s in mesh_info.get('sockets', [])]}")

    for wname, wdata in full_export["weapons"].items():
        cdo = wdata.get("cdo_properties", {})
        log(f"\n{wname}:")
        log(f"  Mesh: {cdo.get('default_weapon_mesh', 'UNKNOWN')}")
        log(f"  Location: {cdo.get('default_mesh_relative_location', 'UNKNOWN')}")
        log(f"  Rotation: {cdo.get('default_mesh_relative_rotation', 'UNKNOWN')}")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    main()
