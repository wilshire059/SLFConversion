"""
Find all W_Inventory_CategoryEntry instances in W_Inventory widget tree
"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

def find_all_category_entries():
    log("=" * 80)
    log("FINDING ALL CATEGORY ENTRY INSTANCES IN W_INVENTORY")
    log("=" * 80)

    # Load W_Inventory
    widget_path = '/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory'
    widget_bp = unreal.load_asset(widget_path)

    if not widget_bp:
        log("ERROR: Failed to load W_Inventory")
        return

    log(f"Loaded: {widget_bp.get_name()}")

    # Get WidgetTree
    widget_tree_path = f"{widget_bp.get_path_name()}:WidgetTree"
    widget_tree = unreal.find_object(None, widget_tree_path)

    if not widget_tree:
        log("ERROR: Could not find WidgetTree")
        return

    log(f"Found WidgetTree: {widget_tree}")

    # Search for all CategoryEntry widgets
    # Widget names from the JSON export
    possible_names = [
        'AllItemsCategoryEntry',
        'WeaponsCategoryEntry',
        'ShieldsCategoryEntry',
        'ArmorCategoryEntry',
        'ToolsCategoryEntry',
        'BolsteringCategoryEntry',
        'KeyItemsCategoryEntry',
        'AbilitiesCategoryEntry',
        'CraftingCategoryEntry',
    ]

    log("\nSearching for CategoryEntry instances:")

    found_entries = []
    for name in possible_names:
        instance_path = f"{widget_bp.get_path_name()}:WidgetTree.{name}"
        widget = unreal.find_object(None, instance_path)

        if widget:
            log(f"\n  {name}:")
            log(f"    Class: {widget.get_class().get_name()}")

            # Check InventoryCategoryData
            try:
                data = widget.get_editor_property('inventory_category_data')
                category = data.get_editor_property('category')
                icon = data.get_editor_property('category_icon')
                log(f"    category: {category}")
                log(f"    category_icon: {icon if icon else 'None'}")
                found_entries.append((name, category, icon))
            except Exception as e:
                log(f"    ERROR: {e}")

    log(f"\n\nFound {len(found_entries)} CategoryEntry instances")

if __name__ == "__main__":
    find_all_category_entries()
