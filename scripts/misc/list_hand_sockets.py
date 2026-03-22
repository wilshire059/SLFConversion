# list_hand_sockets.py
# Find all skeletons and list their hand-related sockets

import unreal
import json

def log(msg):
    print(f"[Socket] {msg}")
    unreal.log_warning(f"[Socket] {msg}")

def main():
    log("=" * 70)
    log("FINDING ALL SKELETONS AND HAND SOCKETS")
    log("=" * 70)

    # Find all skeleton assets
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Search for Skeleton assets
    filter = unreal.ARFilter()
    filter.class_paths = [unreal.TopLevelAssetPath("/Script/Engine", "Skeleton")]
    filter.recursive_paths = True
    filter.package_paths = ["/Game/"]

    assets = asset_registry.get_assets(filter)
    log(f"Found {len(assets)} Skeleton assets")

    results = {}

    for asset_data in assets:
        asset_path = str(asset_data.get_editor_property('package_name')) + "." + str(asset_data.get_editor_property('asset_name'))
        log(f"\n--- {asset_path} ---")

        skeleton = unreal.load_asset(asset_path)
        if not skeleton:
            log(f"  Could not load")
            continue

        socket_count = skeleton.num_sockets()
        log(f"  Socket count: {socket_count}")

        hand_sockets = []
        for i in range(socket_count):
            try:
                socket = skeleton.get_socket_by_index(i)
                if socket:
                    socket_name = socket.get_name()
                    if 'hand' in socket_name.lower() or 'weapon' in socket_name.lower() or socket_name in ['hand_r', 'hand_l']:
                        rel_loc = socket.get_editor_property('relative_location')
                        rel_rot = socket.get_editor_property('relative_rotation')
                        bone_name = socket.get_editor_property('bone_name')
                        log(f"    Socket: {socket_name}")
                        log(f"      Bone: {bone_name}")
                        log(f"      RelLoc: X={rel_loc.x:.4f}, Y={rel_loc.y:.4f}, Z={rel_loc.z:.4f}")
                        log(f"      RelRot: P={rel_rot.pitch:.2f}, Y={rel_rot.yaw:.2f}, R={rel_rot.roll:.2f}")

                        hand_sockets.append({
                            "name": socket_name,
                            "bone": str(bone_name),
                            "location": {"x": rel_loc.x, "y": rel_loc.y, "z": rel_loc.z},
                            "rotation": {"pitch": rel_rot.pitch, "yaw": rel_rot.yaw, "roll": rel_rot.roll}
                        })
            except Exception as e:
                log(f"    Error getting socket {i}: {e}")

        if hand_sockets:
            results[asset_path] = hand_sockets

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/hand_sockets.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    main()
