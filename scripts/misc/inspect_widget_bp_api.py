"""
Inspect WidgetBlueprint API to find correct way to access widget tree
"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

def inspect_widget_bp():
    log("=" * 80)
    log("INSPECTING WIDGET BLUEPRINT API")
    log("=" * 80)

    # Load widget blueprint
    widget_path = '/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory'
    widget_bp = unreal.load_asset(widget_path)

    if not widget_bp:
        log("ERROR: Failed to load widget blueprint")
        return

    log(f"Loaded: {widget_bp.get_name()}")
    log(f"Class: {widget_bp.get_class().get_name()}")

    # List all attributes
    log("\nAll attributes (no underscore):")
    attrs = [a for a in dir(widget_bp) if not a.startswith('_')]
    for attr in attrs:
        log(f"  {attr}")

    # Try specific methods
    log("\n" + "=" * 80)
    log("TRYING SPECIFIC METHODS")
    log("=" * 80)

    # Try get_member_variable
    try:
        all_items = widget_bp.get_member_variable("AllItemsCategoryEntry")
        log(f"get_member_variable('AllItemsCategoryEntry'): {all_items}")
    except Exception as e:
        log(f"get_member_variable error: {e}")

    # Try generated_class CDO
    try:
        gen_class = widget_bp.generated_class()
        log(f"\nGenerated class: {gen_class}")

        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            log(f"CDO: {cdo}")

            if cdo:
                # Try to get AllItemsCategoryEntry from CDO
                try:
                    all_items = cdo.get_editor_property('all_items_category_entry')
                    log(f"CDO.all_items_category_entry: {all_items}")

                    if all_items:
                        log(f"  Type: {all_items.get_class().get_name()}")
                        # Try to get its InventoryCategoryData
                        try:
                            data = all_items.get_editor_property('inventory_category_data')
                            log(f"  inventory_category_data: {data}")
                        except Exception as e:
                            log(f"  inventory_category_data error: {e}")
                except Exception as e:
                    log(f"CDO.all_items_category_entry error: {e}")

                # List CDO properties
                log("\nCDO attributes containing 'category' or 'items':")
                cdo_attrs = [a for a in dir(cdo) if not a.startswith('_')]
                for attr in cdo_attrs:
                    if 'category' in attr.lower() or 'items' in attr.lower() or 'all' in attr.lower():
                        log(f"  {attr}")
    except Exception as e:
        log(f"Generated class error: {e}")

    # Try WidgetBlueprintLibrary
    log("\n" + "=" * 80)
    log("TRYING WIDGET UTILITY LIBRARIES")
    log("=" * 80)

    try:
        # Check if there's a way to create widget instance
        log("Checking unreal module for widget utilities...")
        widget_utils = [a for a in dir(unreal) if 'widget' in a.lower()]
        log(f"Widget-related in unreal: {widget_utils[:20]}")
    except Exception as e:
        log(f"Error: {e}")

if __name__ == "__main__":
    inspect_widget_bp()
