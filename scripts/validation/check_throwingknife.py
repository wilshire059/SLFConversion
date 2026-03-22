"""
Check throwing knife data asset for category and subcategory.
"""
import unreal

def check_throwing_knife():
    paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleThrowingKnife",
        "/Game/SoulslikeFramework/Data/Items/DA_ThrowingKnife"
    ]

    for path in paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            print(f"\n=== {path} ===")
            print(f"Class: {asset.get_class().get_name()}")

            try:
                item_info = asset.get_editor_property('item_information')
                print(f"ItemInformation: {item_info}")

                try:
                    category_info = item_info.get_editor_property('category')
                    print(f"Category struct: {category_info}")

                    try:
                        category = category_info.get_editor_property('category')
                        print(f"  Category: {category}")
                    except Exception as e:
                        print(f"  Category error: {e}")

                    try:
                        subcategory = category_info.get_editor_property('sub_category')
                        print(f"  SubCategory: {subcategory}")
                    except Exception as e:
                        print(f"  SubCategory error: {e}")

                except Exception as e:
                    print(f"Category error: {e}")

            except Exception as e:
                print(f"ItemInformation error: {e}")
        else:
            print(f"NOT FOUND: {path}")

if __name__ == "__main__":
    check_throwing_knife()
