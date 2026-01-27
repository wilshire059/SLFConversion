# export_weapon_scs_data.py
# Run on bp_only project to export ALL weapon component data
# Uses AssetExportTask to get full Blueprint data

import unreal
import json
import re
import os

def log(msg):
    unreal.log_warning(str(msg))

WEAPON_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/weapon_scs_full_export.json"
RAW_EXPORT_DIR = "C:/scripts/SLFConversion/migration_cache/weapon_exports"

def parse_vector(text):
    """Parse X=1.0,Y=2.0,Z=3.0 format"""
    match = re.search(r'X=([^,]+),Y=([^,]+),Z=([^\)]+)', text)
    if match:
        return {
            "x": float(match.group(1)),
            "y": float(match.group(2)),
            "z": float(match.group(3))
        }
    return {"x": 0, "y": 0, "z": 0}

def parse_rotator(text):
    """Parse Pitch=1.0,Yaw=2.0,Roll=3.0 format"""
    match = re.search(r'Pitch=([^,]+),Yaw=([^,]+),Roll=([^\)]+)', text)
    if match:
        return {
            "pitch": float(match.group(1)),
            "yaw": float(match.group(2)),
            "roll": float(match.group(3))
        }
    return {"pitch": 0, "yaw": 0, "roll": 0}

def export_asset_to_text(asset, output_path):
    """Export asset to T3D text format"""
    task = unreal.AssetExportTask()
    task.set_editor_property('object', asset)
    task.set_editor_property('filename', output_path)
    task.set_editor_property('automated', True)
    task.set_editor_property('prompt', False)
    task.set_editor_property('replace_identical', True)

    # Find the COPY exporter for Blueprints
    exporters = unreal.AssetToolsHelpers.get_asset_tools().get_asset_tools().get_exporters()
    # Use default exporter
    result = unreal.Exporter.run_asset_export_task(task)
    return result

def export_weapons():
    log("=" * 70)
    log("EXPORTING WEAPON DATA FROM BP_ONLY")
    log("=" * 70)

    os.makedirs(RAW_EXPORT_DIR, exist_ok=True)

    all_data = {}

    for bp_path in WEAPON_PATHS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n[{bp_name}]")

        # Load Blueprint
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load Blueprint")
            continue

        # Export to COPY text format
        export_path = f"{RAW_EXPORT_DIR}/{bp_name}.COPY"
        task = unreal.AssetExportTask()
        task.set_editor_property('object', bp)
        task.set_editor_property('filename', export_path)
        task.set_editor_property('automated', True)
        task.set_editor_property('prompt', False)
        task.set_editor_property('replace_identical', True)
        task.set_editor_property('exporter', None)  # Use default

        result = unreal.Exporter.run_asset_export_task(task)
        log(f"  Export result: {result}")

        if os.path.exists(export_path):
            with open(export_path, 'r', encoding='utf-8', errors='ignore') as f:
                export_text = f.read()
            log(f"  Exported {len(export_text)} chars to {export_path}")

            weapon_data = {
                "path": bp_path,
                "components": []
            }

            # Parse StaticMeshComponent data from export text
            smc_pattern = r'Begin Object Class=/Script/Engine\.StaticMeshComponent Name="([^"]+)"(.*?)End Object'
            smc_matches = re.findall(smc_pattern, export_text, re.DOTALL)

            for comp_name, comp_body in smc_matches:
                log(f"  Found StaticMeshComponent: {comp_name}")

                comp_data = {
                    "name": comp_name,
                    "class": "StaticMeshComponent"
                }

                # Parse RelativeLocation
                loc_match = re.search(r'RelativeLocation=\(([^)]+)\)', comp_body)
                if loc_match:
                    comp_data["relative_location"] = parse_vector(loc_match.group(1))
                    log(f"    Location: {comp_data['relative_location']}")

                # Parse RelativeRotation
                rot_match = re.search(r'RelativeRotation=\(([^)]+)\)', comp_body)
                if rot_match:
                    comp_data["relative_rotation"] = parse_rotator(rot_match.group(1))
                    log(f"    Rotation: {comp_data['relative_rotation']}")

                # Parse RelativeScale3D
                scale_match = re.search(r'RelativeScale3D=\(([^)]+)\)', comp_body)
                if scale_match:
                    comp_data["relative_scale3d"] = parse_vector(scale_match.group(1))
                    log(f"    Scale: {comp_data['relative_scale3d']}")

                # Parse StaticMesh
                mesh_match = re.search(r'StaticMesh=StaticMesh\'"([^"\']+)\'?"', comp_body)
                if mesh_match:
                    comp_data["static_mesh"] = mesh_match.group(1)
                    log(f"    Mesh: {mesh_match.group(1).split('.')[-1]}")

                weapon_data["components"].append(comp_data)

            all_data[bp_name] = weapon_data
        else:
            log(f"  ERROR: Export file not created")

    # Save to JSON
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(all_data, f, indent=2)

    log(f"\n\nExported to: {OUTPUT_FILE}")
    return all_data

if __name__ == "__main__":
    export_weapons()
    log("\nEXPORT COMPLETE")
