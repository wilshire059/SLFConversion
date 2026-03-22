"""
Extract WeaponAbility property from bp_only weapon data assets.
Run this on bp_only project to extract which weapons have abilities assigned.
"""
import unreal
import json
import os

# Weapons that should have abilities in bp_only
WEAPON_ASSETS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_BossMace",
]

def extract_weapon_abilities():
    """Extract WeaponAbility from weapon data assets."""
    result = {}

    for asset_path in WEAPON_ASSETS:
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            print(f"SKIP: Could not load {asset_path}")
            continue

        asset_name = asset_path.split("/")[-1]

        # Export to text to find WeaponAbility property
        export_path = f"C:/scripts/slfconversion/migration_cache/weapon_{asset_name}_export.txt"

        # Use export_text to get full property dump
        try:
            export_task = unreal.AssetExportTask()
            export_task.object = asset
            export_task.filename = export_path
            export_task.exporter = None
            export_task.automated = True
            export_task.prompt = False
            export_task.write_empty_files = False

            unreal.Exporter.run_asset_export_task(export_task)

            # Read and parse the export
            if os.path.exists(export_path):
                with open(export_path, 'r', encoding='utf-8') as f:
                    content = f.read()

                # Look for WeaponAbility reference
                # Pattern: WeaponAbility_XX_GUID=PDA_WeaponAbility'"/Game/..."'
                import re
                pattern = r'WeaponAbility[^=]*=\s*(?:PDA_WeaponAbility\')?([^\'"\s]+)'
                matches = re.findall(pattern, content)

                if matches:
                    for match in matches:
                        if "DA_" in match or "/Game/" in match:
                            # Clean up the path
                            ability_path = match.strip("'\"")
                            if not ability_path.startswith("/Game/"):
                                # Search for full path in content
                                full_pattern = r'/Game/[^\'"\s]+'
                                full_matches = re.findall(full_pattern, content)
                                for fm in full_matches:
                                    if "WeaponAbility" in fm or "DA_Example" in fm:
                                        ability_path = fm
                                        break

                            result[asset_name] = ability_path
                            print(f"OK: {asset_name} -> {ability_path}")
                            break

                if asset_name not in result:
                    # Try alternative pattern for soft object reference
                    pattern2 = r'/Game/SoulslikeFramework/Data/WeaponAbilities/[^\'"\s\)]+'
                    matches2 = re.findall(pattern2, content)
                    if matches2:
                        result[asset_name] = matches2[0]
                        print(f"OK: {asset_name} -> {matches2[0]}")
                    else:
                        print(f"WARN: {asset_name} - No WeaponAbility found in export")
                        # Print a sample of content for debugging
                        if "WeaponAbility" in content:
                            idx = content.find("WeaponAbility")
                            print(f"  Context: ...{content[max(0,idx-50):idx+200]}...")
            else:
                print(f"WARN: Export file not created for {asset_name}")

        except Exception as e:
            print(f"ERROR: {asset_name} - {e}")

    # Save results
    output_path = "C:/scripts/slfconversion/migration_cache/weapon_abilities.json"
    with open(output_path, 'w') as f:
        json.dump(result, f, indent=2)

    print(f"\nSaved {len(result)} weapon abilities to {output_path}")
    return result

if __name__ == "__main__":
    extract_weapon_abilities()
