"""
Extract B_BossDoor configuration using asset text export
"""
import unreal
import json
import os
import re

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/boss_door_exports"
SUMMARY_PATH = "C:/scripts/SLFConversion/migration_cache/boss_door_bp_only_config.json"

def export_bp_text(bp_path, output_filename):
    """Export Blueprint to text file"""
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        print(f"ERROR: Could not load {bp_path}")
        return None

    # Create output path
    output_path = os.path.join(OUTPUT_DIR, output_filename)

    # Export to file
    result = unreal.AssetExportTask()
    result.object = bp_asset
    result.filename = output_path
    result.automated = True
    result.prompt = False
    result.write_empty_files = False

    success = unreal.Exporter.run_asset_export_task(result)

    if success:
        print(f"Exported {bp_path} to {output_path}")
        return output_path
    else:
        print(f"Failed to export {bp_path}")
        return None

def parse_component_from_text(text, component_name):
    """Parse component properties from export text"""
    # Look for component definition section
    pattern = rf'Begin Object Class=.*? Name="{re.escape(component_name)}"'
    match = re.search(pattern, text)
    if not match:
        return None

    # Find the matching End Object
    start = match.start()
    depth = 1
    pos = match.end()
    while depth > 0 and pos < len(text):
        begin_match = re.search(r'Begin Object', text[pos:])
        end_match = re.search(r'End Object', text[pos:])

        if end_match and (not begin_match or end_match.start() < begin_match.start()):
            depth -= 1
            pos = pos + end_match.end()
        elif begin_match:
            depth += 1
            pos = pos + begin_match.end()
        else:
            break

    component_text = text[start:pos]
    return component_text

def parse_scale_from_text(text):
    """Parse RelativeScale3D from component text"""
    # Look for RelativeScale3D=(X=...,Y=...,Z=...)
    scale_match = re.search(r'RelativeScale3D=\(X=([-\d.]+),Y=([-\d.]+),Z=([-\d.]+)\)', text)
    if scale_match:
        return {
            "X": float(scale_match.group(1)),
            "Y": float(scale_match.group(2)),
            "Z": float(scale_match.group(3))
        }
    return None

def parse_location_from_text(text):
    """Parse RelativeLocation from component text"""
    loc_match = re.search(r'RelativeLocation=\(X=([-\d.]+),Y=([-\d.]+),Z=([-\d.]+)\)', text)
    if loc_match:
        return {
            "X": float(loc_match.group(1)),
            "Y": float(loc_match.group(2)),
            "Z": float(loc_match.group(3))
        }
    return None

def parse_rotation_from_text(text):
    """Parse RelativeRotation from component text"""
    rot_match = re.search(r'RelativeRotation=\(Pitch=([-\d.]+),Yaw=([-\d.]+),Roll=([-\d.]+)\)', text)
    if rot_match:
        return {
            "Pitch": float(rot_match.group(1)),
            "Yaw": float(rot_match.group(2)),
            "Roll": float(rot_match.group(3))
        }
    return None

def parse_mesh_from_text(text):
    """Parse StaticMesh reference from component text"""
    mesh_match = re.search(r"StaticMesh='?\"?([^'\"]+)\"?'?", text)
    if mesh_match:
        return mesh_match.group(1)
    return None

def main():
    print("=" * 60)
    print("Extracting B_BossDoor configuration via text export")
    print("=" * 60)

    # Create output directory
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    # Export blueprints
    blueprints = [
        ("B_Interactable", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"),
        ("B_Door", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"),
        ("B_BossDoor", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"),
    ]

    all_data = {}

    for name, path in blueprints:
        print(f"\n--- {name} ---")
        output_file = export_bp_text(path, f"{name}_export.txt")

        result = {
            "BlueprintPath": path,
            "ClassName": f"{name}_C",
            "Components": []
        }

        if output_file and os.path.exists(output_file):
            with open(output_file, 'r', encoding='utf-8', errors='ignore') as f:
                text = f.read()

            # Find all component definitions
            component_matches = re.findall(r'Begin Object Class=(/Script/[^.]+\.)?(\w+) Name="([^"]+)"', text)
            print(f"  Found {len(component_matches)} component definitions")

            for class_prefix, class_name, comp_name in component_matches:
                print(f"    Component: {comp_name} ({class_name})")

                comp_text = parse_component_from_text(text, comp_name)
                if comp_text:
                    comp_info = {
                        "Name": comp_name,
                        "Class": class_name
                    }

                    scale = parse_scale_from_text(comp_text)
                    if scale:
                        comp_info["RelativeScale3D"] = scale
                        print(f"      Scale: {scale}")

                    loc = parse_location_from_text(comp_text)
                    if loc:
                        comp_info["RelativeLocation"] = loc
                        print(f"      Location: {loc}")

                    rot = parse_rotation_from_text(comp_text)
                    if rot:
                        comp_info["RelativeRotation"] = rot
                        print(f"      Rotation: {rot}")

                    mesh = parse_mesh_from_text(comp_text)
                    if mesh:
                        comp_info["StaticMesh"] = mesh
                        print(f"      Mesh: {mesh}")

                    result["Components"].append(comp_info)

        all_data[name] = result

    # Save summary
    with open(SUMMARY_PATH, 'w') as f:
        json.dump(all_data, f, indent=2)

    print(f"\n\nSaved summary to: {SUMMARY_PATH}")
    print(f"Raw exports in: {OUTPUT_DIR}")
    print("=" * 60)

if __name__ == "__main__":
    main()
