"""
Find widget subobjects in WidgetBlueprint to access widget instances
"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

def find_widget_subobjects():
    log("=" * 80)
    log("FINDING WIDGET SUBOBJECTS")
    log("=" * 80)

    # Load widget blueprint
    widget_path = '/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory'
    widget_bp = unreal.load_asset(widget_path)

    if not widget_bp:
        log("ERROR: Failed to load widget blueprint")
        return

    log(f"Loaded: {widget_bp.get_name()}")
    log(f"Full path: {widget_bp.get_path_name()}")

    # Try to get inner objects using find_object
    log("\n" + "=" * 80)
    log("SEARCHING FOR WIDGET TREE OBJECTS")
    log("=" * 80)

    # The widget tree root is usually named "WidgetTree"
    widget_tree_path = f"{widget_bp.get_path_name()}:WidgetTree"
    log(f"Trying to find: {widget_tree_path}")

    widget_tree = unreal.find_object(None, widget_tree_path)
    if widget_tree:
        log(f"Found WidgetTree: {widget_tree}")
        log(f"  Class: {widget_tree.get_class().get_name()}")

        # Try to get root widget
        try:
            root = widget_tree.get_editor_property('root_widget')
            log(f"  Root widget: {root}")
        except Exception as e:
            log(f"  Root widget error: {e}")
    else:
        log("  WidgetTree not found directly")

    # Try searching for AllItemsCategoryEntry directly
    log("\n" + "=" * 80)
    log("SEARCHING FOR AllItemsCategoryEntry")
    log("=" * 80)

    # Try various paths
    search_paths = [
        f"{widget_bp.get_path_name()}:AllItemsCategoryEntry",
        f"{widget_bp.get_path_name()}.AllItemsCategoryEntry",
        f"{widget_bp.get_path_name()}:WidgetTree.AllItemsCategoryEntry",
        f"/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory.AllItemsCategoryEntry",
    ]

    for path in search_paths:
        obj = unreal.find_object(None, path)
        if obj:
            log(f"FOUND at {path}: {obj}")
            log(f"  Class: {obj.get_class().get_name()}")

            # Try to get its properties
            try:
                data = obj.get_editor_property('inventory_category_data')
                log(f"  inventory_category_data: {data}")
            except Exception as e:
                log(f"  inventory_category_data error: {e}")
        else:
            log(f"Not found: {path}")

    # Try using ObjectIterator to find all W_Inventory_CategoryEntry instances
    log("\n" + "=" * 80)
    log("SEARCHING ALL OBJECTS")
    log("=" * 80)

    try:
        # Load the category entry class
        cat_entry_path = '/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory_CategoryEntry'
        cat_entry_bp = unreal.load_asset(cat_entry_path)

        if cat_entry_bp:
            cat_class = cat_entry_bp.generated_class()
            log(f"W_Inventory_CategoryEntry class: {cat_class}")

            # Use object iterator isn't directly available, but we can try finding by name
            # Let's try using EditorAssetLibrary
            try:
                # Get all objects in the package
                package_path = '/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory'
                # This won't work for inner objects...

                # Alternative: Use the blueprint's export text
                log("\nTrying export_text...")
                export_text = widget_bp.export_text()
                if export_text:
                    log(f"Export text length: {len(export_text)}")

                    # Search for AllItemsCategoryEntry in export
                    lines = export_text.split('\n')
                    for i, line in enumerate(lines):
                        if 'AllItemsCategoryEntry' in line or 'T_Category' in line or 'InventoryCategoryData' in line:
                            log(f"  Line {i}: {line[:200]}")
            except Exception as e:
                log(f"Export text error: {e}")

    except Exception as e:
        log(f"Error: {e}")

if __name__ == "__main__":
    find_widget_subobjects()
