"""
Check what class the item data assets are.
"""
import unreal
import json

def check_item_classes():
    """Check the class of each item data asset."""

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
    ]

    unreal.log("=== Item Class Check ===")

    results = {}

    for path in item_paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            class_obj = asset.get_class()
            class_name = class_obj.get_name() if class_obj else "None"
            class_path = class_obj.get_path_name() if class_obj else "None"

            unreal.log(f"RESULT: {asset.get_name()}: Class={class_name}, Path={class_path}")

            results[asset.get_name()] = {
                "class": class_name,
                "path": class_path,
            }
        else:
            unreal.log_warning(f"  {path}: Not found")

    output_path = "C:/scripts/slfconversion/migration_cache/item_classes.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)

    unreal.log(f"Saved {len(results)} results to {output_path}")

if __name__ == "__main__":
    check_item_classes()
