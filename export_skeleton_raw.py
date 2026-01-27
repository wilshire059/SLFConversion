#!/usr/bin/env python3
"""Export skeleton raw text to find socket data"""

import unreal
import re
import json

def log(msg):
    unreal.log_warning(str(msg))

SKELETON_PATHS = [
    "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/SK_Mannequin",
    "/Game/SoulslikeFramework/Demo/SKM/Mannequin_UE4/Meshes/SK_Mannequin_Skeleton",
]

MESH_PATHS = [
    "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/SKM_Quinn",
    "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/SKM_Manny",
]

def extract_sockets_from_text(text, asset_name):
    """Extract socket info from export_text output"""
    sockets = []

    # Look for SkeletalMeshSocket entries
    # Format: Begin Object Class=/Script/Engine.SkeletalMeshSocket Name="hand_r"
    socket_blocks = re.findall(
        r'Begin Object.*?SkeletalMeshSocket.*?Name="([^"]+)"(.*?)End Object',
        text, re.DOTALL | re.IGNORECASE
    )

    for name, block in socket_blocks:
        socket = {"name": name}

        # Extract SocketName (may differ from object name)
        socket_name_match = re.search(r'SocketName="?([^"\n]+)"?', block)
        if socket_name_match:
            socket["socket_name"] = socket_name_match.group(1).strip()

        # Extract BoneName
        bone_match = re.search(r'BoneName="?([^"\n]+)"?', block)
        if bone_match:
            socket["bone_name"] = bone_match.group(1).strip()

        # Extract RelativeLocation
        loc_match = re.search(r'RelativeLocation=\(X=([-\d.]+),Y=([-\d.]+),Z=([-\d.]+)\)', block)
        if loc_match:
            socket["location"] = {
                "x": float(loc_match.group(1)),
                "y": float(loc_match.group(2)),
                "z": float(loc_match.group(3))
            }

        # Extract RelativeRotation
        rot_match = re.search(r'RelativeRotation=\(Pitch=([-\d.]+),Yaw=([-\d.]+),Roll=([-\d.]+)\)', block)
        if rot_match:
            socket["rotation"] = {
                "pitch": float(rot_match.group(1)),
                "yaw": float(rot_match.group(2)),
                "roll": float(rot_match.group(3))
            }

        sockets.append(socket)
        log(f"  Socket: {socket.get('socket_name', name)} on bone {socket.get('bone_name', 'unknown')}")
        if "location" in socket:
            loc = socket["location"]
            log(f"    Location: X={loc['x']:.2f}, Y={loc['y']:.2f}, Z={loc['z']:.2f}")
        if "rotation" in socket:
            rot = socket["rotation"]
            log(f"    Rotation: P={rot['pitch']:.2f}, Y={rot['yaw']:.2f}, R={rot['roll']:.2f}")

    # Also search for Sockets array format
    sockets_array_match = re.search(r'Sockets\s*\((.*?)\)', text, re.DOTALL)
    if sockets_array_match:
        array_content = sockets_array_match.group(1)
        log(f"\nFound Sockets array in {asset_name}")

    return sockets

def main():
    log("=" * 70)
    log("SKELETON RAW TEXT EXPORT")
    log("=" * 70)

    results = {}

    # Export skeletons
    log("\n=== EXPORTING SKELETONS ===")
    for path in SKELETON_PATHS:
        name = path.split("/")[-1]
        log(f"\n--- {name} ---")

        try:
            export = unreal.EditorAssetLibrary.export_text(path)
            if export:
                log(f"Export length: {len(export)} chars")

                # Save raw export for analysis
                output_file = f"C:/scripts/slfconversion/migration_cache/{name}_export.txt"
                with open(output_file, 'w', encoding='utf-8') as f:
                    f.write(export)
                log(f"Saved to: {output_file}")

                # Extract sockets
                sockets = extract_sockets_from_text(export, name)
                results[name] = {
                    "path": path,
                    "type": "Skeleton",
                    "sockets": sockets
                }

                # Quick search for hand-related content
                hand_matches = re.findall(r'.{0,50}(hand_[rl]|weapon_[rl]|RightHand|LeftHand).{0,50}', export, re.IGNORECASE)
                if hand_matches:
                    log(f"\nFound hand-related content:")
                    for match in hand_matches[:10]:
                        log(f"  {match.strip()[:100]}")
            else:
                log("Export returned empty")
        except Exception as e:
            log(f"Error: {e}")

    # Export meshes
    log("\n=== EXPORTING MESHES ===")
    for path in MESH_PATHS:
        name = path.split("/")[-1]
        log(f"\n--- {name} ---")

        try:
            export = unreal.EditorAssetLibrary.export_text(path)
            if export:
                log(f"Export length: {len(export)} chars")

                # Save raw export for analysis
                output_file = f"C:/scripts/slfconversion/migration_cache/{name}_export.txt"
                with open(output_file, 'w', encoding='utf-8') as f:
                    f.write(export)
                log(f"Saved to: {output_file}")

                # Extract sockets
                sockets = extract_sockets_from_text(export, name)
                results[name] = {
                    "path": path,
                    "type": "SkeletalMesh",
                    "sockets": sockets
                }

                # Search for Skeleton reference
                skeleton_match = re.search(r'Skeleton=([^\s]+)', export)
                if skeleton_match:
                    results[name]["skeleton_ref"] = skeleton_match.group(1)
                    log(f"References skeleton: {skeleton_match.group(1)}")
            else:
                log("Export returned empty")
        except Exception as e:
            log(f"Error: {e}")

    # Save results
    output_path = "C:/scripts/slfconversion/migration_cache/skeleton_raw_export.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\n\nSaved results to: {output_path}")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    main()
