"""
Check what items are in the player's inventory.
This needs to run during gameplay (PIE) to get accurate results.
For now, just check what data assets exist.
"""
import unreal
import json

def check_inventory_items():
    """Check all item data assets and their categories."""

    # Find all item data assets
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Search for PDA_Item assets
    item_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
        "/Game/SoulslikeFramework/Data/Items/DA_Sword02",
        "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
        "/Game/SoulslikeFramework/Data/Items/DA_Katana",
        "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
        "/Game/SoulslikeFramework/Data/Items/DA_BossMace",
        "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
        "/Game/SoulslikeFramework/Data/Items/DA_Apple",
        "/Game/SoulslikeFramework/Data/Items/DA_HealthFlask",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleTalisman",
    ]

    unreal.log("=== Inventory Item Category Check ===")

    results = {}

    for path in item_paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            try:
                item_info = asset.get_editor_property("item_information")
                category_info = item_info.get_editor_property("category")

                category = category_info.get_editor_property("category")
                sub_category = category_info.get_editor_property("sub_category")

                unreal.log(f"RESULT: {asset.get_name()}: Category={category}, SubCategory={sub_category}")

                results[asset.get_name()] = {
                    "category": str(category),
                    "sub_category": str(sub_category),
                }
            except Exception as e:
                unreal.log_warning(f"  {path}: Error - {e}")
        else:
            unreal.log_warning(f"  {path}: Not found")

    # Check what would match for Right Hand slot
    unreal.log("")
    unreal.log("=== Right Hand Slot Match Analysis ===")
    unreal.log("Would match for Category == Weapons (6): ")
    for name, data in results.items():
        if "WEAPONS" in data["category"].upper():
            unreal.log(f"  - {name}")

    unreal.log("Would match for Category == Shields (7): ")
    for name, data in results.items():
        if "SHIELDS" in data["category"].upper():
            unreal.log(f"  - {name}")

    output_path = "C:/scripts/slfconversion/migration_cache/inventory_items.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)

    unreal.log(f"Saved {len(results)} results to {output_path}")

if __name__ == "__main__":
    check_inventory_items()
