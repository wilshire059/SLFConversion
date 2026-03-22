"""
Verify the widget instance data fix
"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

def verify_widget_fix():
    log("=" * 80)
    log("VERIFYING WIDGET INSTANCE FIX")
    log("=" * 80)

    # Load W_Inventory and find AllItemsCategoryEntry
    widget_path = '/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory'
    widget_bp = unreal.load_asset(widget_path)

    if not widget_bp:
        log("ERROR: Failed to load W_Inventory")
        return

    log(f"Loaded: {widget_bp.get_name()}")

    # Find the widget instance
    instance_path = f"{widget_bp.get_path_name()}:WidgetTree.AllItemsCategoryEntry"
    widget = unreal.find_object(None, instance_path)

    if not widget:
        log("ERROR: Failed to find AllItemsCategoryEntry")
        return

    log(f"Found: {widget.get_name()}")

    # Check its InventoryCategoryData
    data = widget.get_editor_property('inventory_category_data')
    log(f"\nInventoryCategoryData:")
    log(f"  Full: {data}")

    category = data.get_editor_property('category')
    icon = data.get_editor_property('category_icon')
    display_name = data.get_editor_property('display_name_override')

    log(f"\n  category: {category}")
    log(f"  category_icon: {icon}")
    log(f"  display_name_override: {display_name}")

    # Check if icon is set correctly
    if icon and 'T_Category_AllItems' in str(icon):
        log("\n*** SUCCESS: CategoryIcon is set to T_Category_AllItems! ***")
    else:
        log("\n*** WARNING: CategoryIcon is NOT set correctly! ***")

if __name__ == "__main__":
    verify_widget_fix()
