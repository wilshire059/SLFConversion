"""
Check weapon item categories to diagnose why weapons aren't showing in equipment.
"""
import unreal
import json

def check_weapon_categories():
    """Check the Category and SubCategory of weapon items."""

    weapon_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
        "/Game/SoulslikeFramework/Data/Items/DA_Sword02",
        "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
        "/Game/SoulslikeFramework/Data/Items/DA_Katana",
        "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
        "/Game/SoulslikeFramework/Data/Items/DA_BossMace",
        "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
    ]

    unreal.log("=== Weapon Category Check ===")

    results = {}

    for path in weapon_paths:
        unreal.log(f"Loading: {path}")
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            unreal.log(f"  Found asset: {asset.get_name()}, class: {asset.get_class().get_name()}")
            try:
                # Get ItemInformation struct
                item_info = asset.get_editor_property("item_information")

                # Get category info
                category_info = item_info.get_editor_property("category")

                category = category_info.get_editor_property("category")
                sub_category = category_info.get_editor_property("sub_category")

                # Convert enum to string representation - will show name and value
                cat_str = str(category)
                subcat_str = str(sub_category)

                unreal.log(f"RESULT: {asset.get_name()}: Category={cat_str}, SubCategory={subcat_str}")

                results[asset.get_name()] = {
                    "category": cat_str,
                    "sub_category": subcat_str,
                }
            except Exception as e:
                unreal.log_warning(f"  {path}: Error reading properties - {e}")
                import traceback
                unreal.log_warning(traceback.format_exc())
        else:
            unreal.log_warning(f"  {path}: Not found")

    # Save results to file
    output_path = "C:/scripts/slfconversion/migration_cache/weapon_categories.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)

    unreal.log(f"Saved {len(results)} results to {output_path}")
    unreal.log("=== End Category Check ===")

if __name__ == "__main__":
    check_weapon_categories()
