"""
Test Equipment Filter Logic

This script tests the GetItemsForEquipmentSlot filtering logic by:
1. Loading weapon data assets
2. Checking their Category/SubCategory values
3. Verifying the slot matching logic would work correctly
"""
import unreal

def get_item_category_info(item_path):
    """Get category info from a data asset"""
    asset = unreal.EditorAssetLibrary.load_asset(item_path)
    if not asset:
        return None

    try:
        # Read directly from asset instance, not CDO
        item_info = asset.get_editor_property('item_information')
        category_info = item_info.get_editor_property('category')
        category = category_info.get_editor_property('category')
        sub_category = category_info.get_editor_property('sub_category')

        return {
            'name': asset.get_name(),
            'category': str(category),
            'sub_category': str(sub_category),
            'category_int': int(category.value) if hasattr(category, 'value') else -1,
            'sub_category_int': int(sub_category.value) if hasattr(sub_category, 'value') else -1,
        }
    except Exception as e:
        return {'name': asset.get_name(), 'error': str(e)}

def test_slot_matching(item_info, slot_type):
    """Test if an item would match a slot based on the new filtering logic"""
    if 'error' in item_info:
        return False, "Error loading item"

    cat = item_info.get('category_int', -1)
    subcat = item_info.get('sub_category_int', -1)

    # Match logic from AC_InventoryManager::GetItemsForEquipmentSlot
    # ESLFItemCategory::Weapons = 6
    # ESLFItemCategory::Shields = 7
    # ESLFItemSubCategory::Sword = 12, Katana = 13, Mace = 15

    if slot_type == 'Right Hand':
        matches = (cat == 6) or (subcat in [12, 13, 15])  # Weapons category or weapon subcats
        return matches, f"Cat={cat}, SubCat={subcat}"
    elif slot_type == 'Left Hand':
        matches = (cat == 7)  # Shields category
        return matches, f"Cat={cat}, SubCat={subcat}"

    return False, "Unknown slot"

def main():
    print("=" * 60)
    print("Equipment Filter Test")
    print("=" * 60)

    # Test weapons (actual paths)
    weapon_paths = [
        '/Game/SoulslikeFramework/Data/Items/DA_Sword01',
        '/Game/SoulslikeFramework/Data/Items/DA_Sword02',
        '/Game/SoulslikeFramework/Data/Items/DA_Greatsword',
        '/Game/SoulslikeFramework/Data/Items/DA_PoisonSword',
        '/Game/SoulslikeFramework/Data/Items/DA_Katana',
        '/Game/SoulslikeFramework/Data/Items/DA_BossMace',
        '/Game/SoulslikeFramework/Data/Items/DA_Shield01',
    ]

    print("\nTesting Item Category Values:")
    print("-" * 60)

    for path in weapon_paths:
        info = get_item_category_info(path)
        if info:
            print(f"\n{info.get('name', 'Unknown')}:")
            if 'error' in info:
                print(f"  ERROR: {info['error']}")
            else:
                print(f"  Category: {info['category']} (int={info['category_int']})")
                print(f"  SubCategory: {info['sub_category']} (int={info['sub_category_int']})")

                # Test right hand matching
                matches_right, reason = test_slot_matching(info, 'Right Hand')
                print(f"  Matches Right Hand: {matches_right} ({reason})")

                # Test left hand matching
                matches_left, reason = test_slot_matching(info, 'Left Hand')
                print(f"  Matches Left Hand: {matches_left} ({reason})")

    print("\n" + "=" * 60)
    print("Test Complete")
    print("=" * 60)

if __name__ == '__main__':
    # Write output to file as well
    import sys
    output_path = 'C:/scripts/SLFConversion/migration_cache/equipment_filter_test.txt'
    with open(output_path, 'w') as f:
        # Capture print output
        old_stdout = sys.stdout
        sys.stdout = f
        main()
        sys.stdout = old_stdout
    unreal.log(f"Test results written to: {output_path}")
