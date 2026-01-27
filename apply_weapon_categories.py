"""
Apply Weapon Categories

Restores weapon Category and SubCategory values from cached data.
This data was lost during Blueprint reparenting and needs to be reapplied.
"""
import unreal
import json

def main():
    # Load cached category data
    cache_path = 'C:/scripts/SLFConversion/migration_cache/weapon_categories.json'
    with open(cache_path, 'r') as f:
        cached_data = json.load(f)

    unreal.log(f"Loaded {len(cached_data)} items from cache")

    # Map string enum names to values
    category_map = {
        '<SLFItemCategory.NONE: 0>': 0,
        '<SLFItemCategory.TOOLS: 1>': 1,
        '<SLFItemCategory.ABILITIES: 2>': 2,
        '<SLFItemCategory.BOLSTERING: 3>': 3,
        '<SLFItemCategory.CONSUMABLES: 4>': 4,
        '<SLFItemCategory.MATERIALS: 5>': 5,
        '<SLFItemCategory.WEAPONS: 6>': 6,
        '<SLFItemCategory.SHIELDS: 7>': 7,
        '<SLFItemCategory.ARMOR: 8>': 8,
    }

    subcategory_map = {
        '<SLFItemSubCategory.MISCELLANEOUS: 4>': 4,
        '<SLFItemSubCategory.SWORD: 12>': 12,
        '<SLFItemSubCategory.KATANA: 13>': 13,
        '<SLFItemSubCategory.AXE: 14>': 14,
        '<SLFItemSubCategory.MACE: 15>': 15,
        '<SLFItemSubCategory.STAFF: 16>': 16,
        '<SLFItemSubCategory.HEAD: 6>': 6,
        '<SLFItemSubCategory.CHEST: 7>': 7,
        '<SLFItemSubCategory.ARMS: 8>': 8,
        '<SLFItemSubCategory.LEGS: 9>': 9,
        '<SLFItemSubCategory.TALISMANS: 10>': 10,
        '<SLFItemSubCategory.PROJECTILES: 2>': 2,
    }

    success_count = 0
    error_count = 0

    for item_name, data in cached_data.items():
        item_path = f'/Game/SoulslikeFramework/Data/Items/{item_name}'

        try:
            asset = unreal.EditorAssetLibrary.load_asset(item_path)
            if not asset:
                unreal.log_warning(f"Could not load: {item_path}")
                error_count += 1
                continue

            gen_class = asset.get_class()
            cdo = unreal.get_default_object(gen_class)

            # Get ItemInformation struct
            item_info = cdo.get_editor_property('item_information')
            category_info = item_info.get_editor_property('category')

            # Get enum values from cache
            cat_str = data.get('category', '<SLFItemCategory.NONE: 0>')
            subcat_str = data.get('sub_category', '<SLFItemSubCategory.MISCELLANEOUS: 4>')

            cat_val = category_map.get(cat_str, 0)
            subcat_val = subcategory_map.get(subcat_str, 4)

            # Create new category enum values
            # ESLFItemCategory enum
            category_enum = unreal.SLFItemCategory(cat_val)
            subcategory_enum = unreal.SLFItemSubCategory(subcat_val)

            # Set properties
            category_info.set_editor_property('category', category_enum)
            category_info.set_editor_property('sub_category', subcategory_enum)
            item_info.set_editor_property('category', category_info)
            cdo.set_editor_property('item_information', item_info)

            # Save
            unreal.EditorAssetLibrary.save_asset(item_path)
            unreal.log(f"OK: {item_name} -> Category={cat_val}, SubCategory={subcat_val}")
            success_count += 1

        except Exception as e:
            unreal.log_error(f"FAIL: {item_name} - {e}")
            error_count += 1

    unreal.log(f"\nResults: {success_count} succeeded, {error_count} failed")

if __name__ == '__main__':
    main()
