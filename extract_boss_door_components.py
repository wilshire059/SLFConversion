"""
Extract B_BossDoor component data via export_text
"""
import unreal
import json
import os
import re

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/boss_door_bp_only_config.json"
TEXT_EXPORT_DIR = "C:/scripts/SLFConversion/migration_cache/boss_door_comparison"

def parse_scs_components(export_text):
    """Parse SCS (SimpleConstructionScript) component definitions from export text"""
    components = []

    # Find all SCS_Node entries with their components
    scs_pattern = re.compile(
        r'Begin Object Class=/Script/Engine\.SCS_Node Name="([^"]+)".*?'
        r'ComponentClass=Class\'"/Script/[^/]+\.([^"\']+)"\'.*?'
        r'End Object',
        re.DOTALL
    )

    # Also look for direct component templates
    template_pattern = re.compile(
        r'Begin Object Class=/Script/[^/]+\.(\w+) Name="([^"]+)"',
        re.MULTILINE
    )

    for match in template_pattern.finditer(export_text):
        class_name = match.group(1)
        name = match.group(2)

        # Skip non-component classes
        if not any(x in class_name for x in ['Component', 'Billboard', 'Niagara']):
            continue

        # Get the component section text
        start_pos = match.start()
        depth = 1
        end_pos = match.end()
        text_len = len(export_text)

        while depth > 0 and end_pos < text_len:
            next_begin = export_text.find('Begin Object', end_pos)
            next_end = export_text.find('End Object', end_pos)

            if next_end == -1:
                break

            if next_begin != -1 and next_begin < next_end:
                depth += 1
                end_pos = next_begin + 12
            else:
                depth -= 1
                end_pos = next_end + 10

        comp_text = export_text[start_pos:end_pos]

        comp_info = {
            "Name": name,
            "Class": class_name,
        }

        # Parse transform properties
        loc_match = re.search(r'RelativeLocation=\(X=([-\d.]+),\s*Y=([-\d.]+),\s*Z=([-\d.]+)\)', comp_text)
        if loc_match:
            comp_info["Location"] = {
                "X": float(loc_match.group(1)),
                "Y": float(loc_match.group(2)),
                "Z": float(loc_match.group(3))
            }

        rot_match = re.search(r'RelativeRotation=\(Pitch=([-\d.]+),\s*Yaw=([-\d.]+),\s*Roll=([-\d.]+)\)', comp_text)
        if rot_match:
            comp_info["Rotation"] = {
                "Pitch": float(rot_match.group(1)),
                "Yaw": float(rot_match.group(2)),
                "Roll": float(rot_match.group(3))
            }

        scale_match = re.search(r'RelativeScale3D=\(X=([-\d.]+),\s*Y=([-\d.]+),\s*Z=([-\d.]+)\)', comp_text)
        if scale_match:
            comp_info["Scale"] = {
                "X": float(scale_match.group(1)),
                "Y": float(scale_match.group(2)),
                "Z": float(scale_match.group(3))
            }

        # Parse static mesh
        mesh_match = re.search(r"StaticMesh=StaticMesh'\"([^\"]+)\"'", comp_text)
        if mesh_match:
            comp_info["StaticMesh"] = mesh_match.group(1)

        # Parse Niagara asset
        niagara_match = re.search(r"Asset=NiagaraSystem'\"([^\"]+)\"'", comp_text)
        if niagara_match:
            comp_info["NiagaraSystem"] = niagara_match.group(1)

        # Attachment parent
        attach_match = re.search(r"AttachParent=\w+Component'\"([^\"]+)\"'", comp_text)
        if attach_match:
            comp_info["AttachParent"] = attach_match.group(1)

        components.append(comp_info)
        print(f"    Component: {name} ({class_name})")
        if "StaticMesh" in comp_info:
            print(f"      Mesh: {comp_info['StaticMesh']}")
        if "NiagaraSystem" in comp_info:
            print(f"      Niagara: {comp_info['NiagaraSystem']}")

    return components

def extract_blueprint_data(bp_path, name):
    """Extract Blueprint data using export_text"""
    print(f"\n--- Processing {name} ---")

    # Use EditorAssetLibrary.export_text to get full asset data
    export_text = unreal.EditorAssetLibrary.export_text(bp_path)
    if not export_text:
        print(f"  ERROR: Could not export {bp_path}")
        return None

    # Save raw export for comparison
    os.makedirs(TEXT_EXPORT_DIR, exist_ok=True)
    export_path = os.path.join(TEXT_EXPORT_DIR, f"{name}_export.txt")
    with open(export_path, 'w', encoding='utf-8', errors='replace') as f:
        f.write(export_text)
    print(f"  Exported to: {export_path}")

    result = {
        "BlueprintPath": bp_path,
        "ClassName": f"{name}_C",
        "Components": [],
        "HasDoorFrame": "DoorFrame" in export_text,
        "HasFogGateMesh": "FogGateMesh" in export_text,
        "HasPortal_Front": "Portal_Front" in export_text,
        "HasPortal_Back": "Portal_Back" in export_text,
    }

    # Parse components
    result["Components"] = parse_scs_components(export_text)

    # Look for specific mesh references
    mesh_refs = []
    for match in re.finditer(r"StaticMesh=StaticMesh'\"([^\"]+)\"'", export_text):
        mesh_refs.append(match.group(1))
    result["MeshReferences"] = list(set(mesh_refs))

    return result

def main():
    print("=" * 60)
    print("Extracting B_BossDoor component configuration")
    print("=" * 60)

    # Process blueprints in order
    blueprints = [
        ("B_Interactable", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"),
        ("B_Door", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"),
        ("B_BossDoor", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"),
    ]

    all_data = {}

    for name, path in blueprints:
        data = extract_blueprint_data(path, name)
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
