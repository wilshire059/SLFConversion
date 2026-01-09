"""
Check PDA_Item data to see if ItemInformation has valid icons and descriptions
"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

def check_item_data():
    log("=" * 80)
    log("CHECKING DA_ITEM DATA")
    log("=" * 80)

    # Correct paths for item data assets
    test_items = [
        '/Game/SoulslikeFramework/Data/Items/DA_ThrowingKnife',
        '/Game/SoulslikeFramework/Data/Items/DA_HealthFlask',
        '/Game/SoulslikeFramework/Data/Items/DA_Sword01',
        '/Game/SoulslikeFramework/Data/Items/DA_Apple',
    ]

    for item_path in test_items:
        log(f"\n--- Checking: {item_path} ---")

        item = unreal.load_asset(item_path)
        if not item:
            log(f"  ERROR: Could not load asset")
            continue

        log(f"  Loaded: {item.get_name()}")
        log(f"  Class: {item.get_class().get_name()}")

        # Check if it has ItemInformation
        try:
            item_info = item.get_editor_property('item_information')
            log(f"\n  ItemInformation: {item_info}")

            if item_info:
                # Try to get specific properties
                try:
                    display_name = item_info.get_editor_property('display_name')
                    log(f"    DisplayName: '{display_name}'")
                except Exception as e:
                    log(f"    DisplayName error: {e}")

                try:
                    short_desc = item_info.get_editor_property('short_description')
                    # Truncate long text
                    short_str = str(short_desc)[:100] if short_desc else "None"
                    log(f"    ShortDescription: '{short_str}'")
                except Exception as e:
                    log(f"    ShortDescription error: {e}")

                try:
                    icon_small = item_info.get_editor_property('icon_small')
                    log(f"    IconSmall: {icon_small}")
                except Exception as e:
                    log(f"    IconSmall error: {e}")

                try:
                    max_amount = item_info.get_editor_property('max_amount')
                    log(f"    MaxAmount: {max_amount}")
                except Exception as e:
                    log(f"    MaxAmount error: {e}")

                try:
                    category = item_info.get_editor_property('category')
                    log(f"    Category: {category}")
                except Exception as e:
                    log(f"    Category error: {e}")

        except Exception as e:
            log(f"  ItemInformation error: {e}")

if __name__ == "__main__":
    check_item_data()
