#!/usr/bin/env python3
"""Export skeleton sockets directly from skeleton assets"""

import unreal
import json

def log(msg):
    unreal.log_warning(str(msg))

SKELETON_PATHS = [
    "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/SK_Mannequin",
    "/Game/SoulslikeFramework/Demo/SKM/Mannequin_UE4/Meshes/SK_Mannequin_Skeleton",
]

def export_skeleton(skeleton_path):
    """Export skeleton asset sockets"""
    log(f"\n=== {skeleton_path.split('/')[-1]} ===")

    skeleton = unreal.EditorAssetLibrary.load_asset(skeleton_path)
    if not skeleton:
        log(f"ERROR: Could not load skeleton")
        return None

    log(f"Asset type: {skeleton.get_class().get_name()}")

    result = {
        "path": skeleton_path,
        "asset_type": skeleton.get_class().get_name(),
        "sockets": []
    }

    # If it's a Skeleton asset
    if skeleton.get_class().get_name() == "Skeleton":
        try:
            sockets = skeleton.get_editor_property("sockets")
            if sockets:
                log(f"Found {len(sockets)} sockets:")
                for socket in sockets:
                    socket_name = str(socket.get_editor_property("socket_name"))
                    bone_name = str(socket.get_editor_property("bone_name"))
                    log(f"  - {socket_name} (bone: {bone_name})")

                    socket_data = {
                        "name": socket_name,
                        "bone": bone_name
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
            else:
                log("No sockets found on this skeleton")
        except Exception as e:
            log(f"Error getting sockets: {e}")
    # If it's a SkeletalMesh, get its skeleton
    elif skeleton.get_class().get_name() == "SkeletalMesh":
        try:
            actual_skeleton = skeleton.get_editor_property("skeleton")
            if actual_skeleton:
                log(f"SkeletalMesh uses skeleton: {actual_skeleton.get_name()}")
                result["referenced_skeleton"] = actual_skeleton.get_path_name()

                sockets = actual_skeleton.get_editor_property("sockets")
                if sockets:
                    log(f"Found {len(sockets)} sockets:")
                    for socket in sockets:
                        socket_name = str(socket.get_editor_property("socket_name"))
                        bone_name = str(socket.get_editor_property("bone_name"))
                        log(f"  - {socket_name} (bone: {bone_name})")

                        socket_data = {
                            "name": socket_name,
                            "bone": bone_name
                        }
                        result["sockets"].append(socket_data)
        except Exception as e:
            log(f"Error: {e}")

    return result

def main():
    log("=" * 70)
    log("SKELETON SOCKET EXPORT")
    log("=" * 70)

    all_results = {}

    for path in SKELETON_PATHS:
        result = export_skeleton(path)
        if result:
            all_results[path.split("/")[-1]] = result

    # Also check for hand sockets specifically
    log("\n=== SEARCHING FOR HAND SOCKETS ===")

    # Search all assets in Demo folder for skeletons with hand sockets
    asset_paths = unreal.EditorAssetLibrary.list_assets("/Game/SoulslikeFramework/Demo/", recursive=True)
    for asset_path in asset_paths:
        if "skeleton" in asset_path.lower():
            asset = unreal.EditorAssetLibrary.load_asset(asset_path.split(".")[0])
            if asset and asset.get_class().get_name() == "Skeleton":
                try:
                    sockets = asset.get_editor_property("sockets")
                    if sockets:
                        hand_sockets = [str(s.get_editor_property("socket_name")) for s in sockets
                                       if "hand" in str(s.get_editor_property("socket_name")).lower()]
                        if hand_sockets:
                            log(f"Found hand sockets in {asset_path}: {hand_sockets}")
                except: pass

    log("\n" + "=" * 70)

    # Save to file
    output_path = "C:/scripts/slfconversion/migration_cache/skeleton_sockets.json"
    with open(output_path, 'w') as f:
        json.dump(all_results, f, indent=2)
    log(f"Saved to: {output_path}")

if __name__ == "__main__":
    main()
