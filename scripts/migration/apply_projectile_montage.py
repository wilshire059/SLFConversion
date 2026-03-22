"""
Check and apply throw montage data to DA_Action_Projectile.
Also verify throwing knife has correct subcategory.
"""
import unreal

# The throw montage asset path
THROW_MONTAGE_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Combat/Montages/AM_SLF_ThrowProjectile"

def check_and_fix():
    print("\n=== Checking Throw Projectile Action Data ===\n")

    # 1. Check DA_Action_Projectile
    action_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Projectile"
    action = unreal.EditorAssetLibrary.load_asset(action_path)

    if not action:
        print(f"ERROR: Could not load {action_path}")
        return

    print(f"Loaded: {action.get_name()}")
    print(f"Class: {action.get_class().get_name()}")

    # Check RelevantData
    try:
        relevant_data = action.get_editor_property('relevant_data')
        print(f"RelevantData: {relevant_data}")
        print(f"RelevantData valid: {relevant_data.is_valid() if hasattr(relevant_data, 'is_valid') else 'N/A'}")
    except Exception as e:
        print(f"RelevantData error: {e}")

    # Check ActionMontage property
    try:
        action_montage = action.get_editor_property('action_montage')
        print(f"ActionMontage: {action_montage}")
    except Exception as e:
        print(f"ActionMontage error: {e}")

    # 2. Check throwing knife
    print("\n=== Checking Throwing Knife Item Data ===\n")

    knife_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleThrowingKnife",
        "/Game/SoulslikeFramework/Data/Items/Consumables/DA_ExampleThrowingKnife",
    ]

    for knife_path in knife_paths:
        knife = unreal.EditorAssetLibrary.load_asset(knife_path)
        if knife:
            print(f"Found: {knife_path}")
            print(f"Class: {knife.get_class().get_name()}")

            try:
                item_info = knife.get_editor_property('item_information')
                print(f"ItemInformation: {item_info}")

                try:
                    category_info = item_info.get_editor_property('category')
                    print(f"CategoryInfo: {category_info}")

                    try:
                        category = category_info.get_editor_property('category')
                        subcategory = category_info.get_editor_property('sub_category')
                        print(f"  Category: {category}")
                        print(f"  SubCategory: {subcategory}")

                        # Check if subcategory is Projectiles (value 2)
                        if str(subcategory) != "ESLFItemSubCategory.PROJECTILES":
                            print(f"  WARNING: SubCategory is NOT Projectiles!")
                    except Exception as e:
                        print(f"  Category/SubCategory error: {e}")
                except Exception as e:
                    print(f"CategoryInfo error: {e}")
            except Exception as e:
                print(f"ItemInformation error: {e}")
            break
        else:
            print(f"Not found: {knife_path}")

    # 3. Check the throw montage exists
    print("\n=== Checking Throw Montage ===\n")
    montage = unreal.EditorAssetLibrary.load_asset(THROW_MONTAGE_PATH)
    if montage:
        print(f"Throw montage exists: {THROW_MONTAGE_PATH}")
        print(f"Class: {montage.get_class().get_name()}")
    else:
        print(f"ERROR: Throw montage not found at {THROW_MONTAGE_PATH}")

    print("\n=== Done ===")

if __name__ == "__main__":
    check_and_fix()
