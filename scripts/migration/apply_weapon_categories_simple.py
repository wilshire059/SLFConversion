"""
Apply Weapon Categories (Simplified)
Sets weapon categories directly using enum integer values.
"""
import unreal

# Category data: item_name -> (category_int, subcategory_int)
# ESLFItemCategory: WEAPONS=6, SHIELDS=7
# ESLFItemSubCategory: SWORD=12, KATANA=13, MACE=15, MISCELLANEOUS=4
CATEGORY_DATA = {
    'DA_Sword01': (6, 12),      # WEAPONS, SWORD
    'DA_Sword02': (6, 12),      # WEAPONS, SWORD
    'DA_Greatsword': (6, 12),   # WEAPONS, SWORD
    'DA_Katana': (6, 13),       # WEAPONS, KATANA
    'DA_PoisonSword': (6, 12),  # WEAPONS, SWORD
    'DA_BossMace': (6, 15),     # WEAPONS, MACE
    'DA_Shield01': (7, 4),      # SHIELDS, MISCELLANEOUS
}

def main():
    success = 0
    fail = 0

    for item_name, (cat_val, subcat_val) in CATEGORY_DATA.items():
        item_path = f'/Game/SoulslikeFramework/Data/Items/{item_name}'

        try:
            asset = unreal.EditorAssetLibrary.load_asset(item_path)
            if not asset:
                unreal.log_warning(f"Could not load: {item_path}")
                fail += 1
                continue

            gen_class = asset.get_class()
            cdo = unreal.get_default_object(gen_class)

            # Get the nested structs
            item_info = cdo.get_editor_property('item_information')
            category_struct = item_info.get_editor_property('category')

            # Set enum values by integer
            category_struct.set_editor_property('category', cat_val)
            category_struct.set_editor_property('sub_category', subcat_val)

            # Write back
            item_info.set_editor_property('category', category_struct)
            cdo.set_editor_property('item_information', item_info)

            # Save
            unreal.EditorAssetLibrary.save_asset(item_path)
            unreal.log(f"OK: {item_name} Cat={cat_val} SubCat={subcat_val}")
            success += 1

        except Exception as e:
            unreal.log_error(f"FAIL: {item_name} - {str(e)}")
            fail += 1

    unreal.log(f"Done: {success} success, {fail} fail")

if __name__ == '__main__':
    main()
