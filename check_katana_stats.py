"""
Check DA_Katana stats from bp_only project
Uses export_text() to see raw property values since Blueprint uses different naming
"""
import unreal
import re

def check_katana_stats():
    # Load the DA_Katana asset
    asset_path = "/Game/SoulslikeFramework/Data/Items/DA_Katana"

    asset = unreal.load_asset(asset_path)
    if not asset:
        print(f"ERROR: Could not load {asset_path}")
        return

    print(f"\n=== DA_Katana Stats Check ===")
    print(f"Asset: {asset.get_name()}")
    print(f"Class: {asset.get_class().get_name()}")

    # Use export_text to get the raw property dump
    export_text = unreal.EditorAssetLibrary.export_text(asset_path)

    # Save to file for analysis
    output_path = "C:/scripts/SLFConversion/migration_cache/katana_bp_only_export.txt"
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(export_text)
    print(f"\nExported to: {output_path}")

    # Search for stat-related properties
    print("\n=== Searching for Stat Properties ===")

    # Look for WeaponStatInfo section
    if "WeaponStatInfo" in export_text:
        print("\nWeaponStatInfo FOUND in export")
        # Extract the WeaponStatInfo section
        match = re.search(r'WeaponStatInfo=\((.*?)\)', export_text, re.DOTALL)
        if match:
            print(f"  Content: {match.group(1)[:500]}...")
    else:
        print("\nWeaponStatInfo NOT FOUND in export")

    # Look for bHasStatScaling
    if "bHasStatScaling" in export_text:
        match = re.search(r'bHasStatScaling=(\w+)', export_text)
        if match:
            print(f"\nbHasStatScaling: {match.group(1)}")

    # Look for bHasStatRequirement
    if "bHasStatRequirement" in export_text:
        match = re.search(r'bHasStatRequirement=(\w+)', export_text)
        if match:
            print(f"bHasStatRequirement: {match.group(1)}")

    # Look for ScalingInfo
    if "ScalingInfo" in export_text:
        print("\nScalingInfo FOUND")
        match = re.search(r'ScalingInfo=\((.*?)\)', export_text, re.DOTALL)
        if match:
            print(f"  Content: {match.group(1)[:300]}...")

    # Look for StatRequirementInfo
    if "StatRequirementInfo" in export_text:
        print("\nStatRequirementInfo FOUND")
        match = re.search(r'StatRequirementInfo=\((.*?)\)', export_text, re.DOTALL)
        if match:
            print(f"  Content: {match.group(1)[:300]}...")

    # Look for StatChanges
    if "StatChanges" in export_text:
        print("\nStatChanges FOUND")
        match = re.search(r'StatChanges=\((.*?)\)', export_text, re.DOTALL)
        if match:
            print(f"  Content: {match.group(1)[:300]}...")

    print("\n=== Done ===")

if __name__ == "__main__":
    check_katana_stats()
