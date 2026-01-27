"""
Apply Weapon Categories and SubCategories

Restores weapon Category and SubCategory values using the proper enum types.
"""
import unreal

# Get enum classes
SLFItemCategory = unreal.SLFItemCategory
SLFItemSubCategory = unreal.SLFItemSubCategory

# Item data: item_name -> (category_enum, subcategory_enum)
ITEM_DATA = {
    'DA_Sword01': (SLFItemCategory.WEAPONS, SLFItemSubCategory.SWORD),
    'DA_Sword02': (SLFItemCategory.WEAPONS, SLFItemSubCategory.SWORD),
    'DA_Greatsword': (SLFItemCategory.WEAPONS, SLFItemSubCategory.SWORD),
    'DA_Katana': (SLFItemCategory.WEAPONS, SLFItemSubCategory.KATANA),
    'DA_PoisonSword': (SLFItemCategory.WEAPONS, SLFItemSubCategory.SWORD),
    'DA_BossMace': (SLFItemCategory.WEAPONS, SLFItemSubCategory.MACE),
    'DA_Shield01': (SLFItemCategory.SHIELDS, SLFItemSubCategory.MISCELLANEOUS),
}

ITEMS_PATH = "/Game/SoulslikeFramework/Data/Items/"

def main():
    unreal.log("=" * 60)
    unreal.log("APPLYING WEAPON CATEGORIES AND SUBCATEGORIES")
    unreal.log("=" * 60)

    success = 0
    fail = 0

    for item_name, (category_enum, subcategory_enum) in ITEM_DATA.items():
        asset_path = f"{ITEMS_PATH}{item_name}.{item_name}"

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            unreal.log_warning(f"Could not load: {asset_path}")
            fail += 1
            continue

        try:
            # Get ItemInformation struct from asset directly
            item_info = asset.get_editor_property('item_information')
            if item_info is None:
                unreal.log_error(f"{item_name}: No item_information")
                fail += 1
                continue

            # Get Category struct
            category_data = item_info.get_editor_property('category')
            if category_data is None:
                unreal.log_error(f"{item_name}: No category property")
                fail += 1
                continue

            # Log old values
            old_cat = category_data.get_editor_property('category')
            old_subcat = category_data.get_editor_property('sub_category')

            # Set new values
            category_data.set_editor_property('category', category_enum)
            category_data.set_editor_property('sub_category', subcategory_enum)

            # Update ItemInformation with new category data
            item_info.set_editor_property('category', category_data)

            # Update asset with new ItemInformation
            asset.set_editor_property('item_information', item_info)

            # Save
            unreal.EditorAssetLibrary.save_loaded_asset(asset)

            unreal.log(f"OK: {item_name}")
            unreal.log(f"    Category: {old_cat} -> {category_enum}")
            unreal.log(f"    SubCategory: {old_subcat} -> {subcategory_enum}")
            success += 1

        except Exception as e:
            unreal.log_error(f"FAIL: {item_name} - {e}")
            fail += 1

    unreal.log("=" * 60)
    unreal.log(f"Results: {success} succeeded, {fail} failed")
    unreal.log("=" * 60)

if __name__ == '__main__':
    main()
