# find_weapon_meshes.py
# Find all weapon mesh assets in the project

import unreal

def log(msg):
    unreal.log_warning(str(msg))

def find_meshes():
    log("=" * 70)
    log("SEARCHING FOR WEAPON MESHES")
    log("=" * 70)

    # Search for static meshes with weapon-related names
    all_meshes = unreal.EditorAssetLibrary.list_assets("/Game/", recursive=True)

    weapon_meshes = []
    for asset in all_meshes:
        asset_lower = asset.lower()
        if "weapon" in asset_lower or "sword" in asset_lower or "shield" in asset_lower or "katana" in asset_lower or "mace" in asset_lower or "greatsword" in asset_lower:
            if ".staticmesh" in asset_lower or "sm_" in asset_lower:
                weapon_meshes.append(asset)

    log(f"\nFound {len(weapon_meshes)} potential weapon meshes:")
    for mesh in sorted(weapon_meshes):
        log(f"  {mesh}")

    # Also look in specific folders
    log("\n\nChecking Demo/Meshes folder:")
    try:
        demo_meshes = unreal.EditorAssetLibrary.list_assets("/Game/SoulslikeFramework/Demo/Meshes/", recursive=True)
        for mesh in sorted(demo_meshes):
            log(f"  {mesh}")
    except:
        log("  Could not list Demo/Meshes folder")

    log("\n\nChecking Weapons folder:")
    try:
        weapon_assets = unreal.EditorAssetLibrary.list_assets("/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/", recursive=True)
        for asset in sorted(weapon_assets):
            if "mesh" in asset.lower() or "sm_" in asset.lower():
                log(f"  {asset}")
    except:
        log("  Could not list Weapons folder")

if __name__ == "__main__":
    find_meshes()
