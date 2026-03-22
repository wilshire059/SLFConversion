# extract_bp_only_mesh_transform.py
# Extract actual mesh component transforms from bp_only using export_text

import unreal
import re
import json

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

def log(msg):
    print(f"[Extract] {msg}")
    unreal.log_warning(f"[Extract] {msg}")

def main():
    log("=" * 70)
    log("EXTRACTING BP_ONLY MESH TRANSFORMS VIA EXPORT_TEXT")
    log("=" * 70)

    results = {}

    for bp_path in WEAPONS:
        bp_name = bp_path.split('/')[-1]
        log(f"\n--- {bp_name} ---")

        bp = unreal.load_asset(bp_path)
        if not bp:
            log(f"  Could not load: {bp_path}")
            continue

        # Export to text
        try:
            export_text = unreal.EditorAssetLibrary.export_text(bp_path)
            if not export_text:
                log(f"  Export failed")
                continue

            # Save raw export for debugging
            with open(f"C:/scripts/SLFConversion/migration_cache/weapon_debug/{bp_name}_export.txt", 'w', encoding='utf-8') as f:
                f.write(export_text)

            # Parse for StaticMesh component transforms
            # Look for patterns like:
            # RelativeLocation=(X=0.000000,Y=0.000000,Z=36.494421)
            # RelativeRotation=(Pitch=0.000000,Yaw=0.000000,Roll=180.000000)

            results[bp_name] = {"path": bp_path}

            # Find StaticMesh component section
            # Components are defined in Begin Object blocks
            mesh_pattern = r'Begin Object.*?Name="StaticMesh".*?End Object'
            mesh_match = re.search(mesh_pattern, export_text, re.DOTALL | re.IGNORECASE)

            if mesh_match:
                mesh_section = mesh_match.group(0)
                log(f"  Found StaticMesh component section ({len(mesh_section)} chars)")

                # Extract RelativeLocation
                loc_match = re.search(r'RelativeLocation=\(X=([-\d.]+),Y=([-\d.]+),Z=([-\d.]+)\)', mesh_section)
                if loc_match:
                    x, y, z = float(loc_match.group(1)), float(loc_match.group(2)), float(loc_match.group(3))
                    results[bp_name]["scs_location"] = {"x": x, "y": y, "z": z}
                    log(f"  SCS RelativeLocation: X={x:.4f}, Y={y:.4f}, Z={z:.4f}")

                # Extract RelativeRotation
                rot_match = re.search(r'RelativeRotation=\(Pitch=([-\d.]+),Yaw=([-\d.]+),Roll=([-\d.]+)\)', mesh_section)
                if rot_match:
                    p, ya, r = float(rot_match.group(1)), float(rot_match.group(2)), float(rot_match.group(3))
                    results[bp_name]["scs_rotation"] = {"pitch": p, "yaw": ya, "roll": r}
                    log(f"  SCS RelativeRotation: P={p:.2f}, Y={ya:.2f}, R={r:.2f}")
            else:
                log(f"  StaticMesh component section NOT found")
                # Try to find any StaticMesh or Mesh component
                all_objects = re.findall(r'Begin Object.*?Name="(\w+)"', export_text, re.IGNORECASE)
                log(f"  Found objects: {all_objects[:10]}...")

        except Exception as e:
            log(f"  Error: {e}")

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/bp_only_scs_transforms.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    main()
