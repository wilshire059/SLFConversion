"""
Check if InventoryCategoryData is preserved after migration
"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

def check_category_data():
    log("=" * 80)
    log("CHECKING INVENTORY CATEGORY DATA AFTER MIGRATION")
    log("=" * 80)

    # Load W_Inventory_CategoryEntry and check its InventoryCategoryData
    cat_path = '/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory_CategoryEntry'
    cat_bp = unreal.load_asset(cat_path)

    if not cat_bp:
        log(f"ERROR: Failed to load {cat_path}")
        return

    log(f"Loaded: {cat_bp.get_name()}")

    # Get the generated class
    gen_class = cat_bp.generated_class()
    if gen_class:
        log(f"Generated class: {gen_class.get_name()}")

        # Get the parent class using get_class_default_object
        parent = gen_class.get_super_class() if hasattr(gen_class, 'get_super_class') else None
        if parent:
            log(f"Parent class: {parent.get_name()}")

    # Get the CDO
    cdo = unreal.get_default_object(gen_class)
    if cdo:
        log(f"CDO: {cdo.get_name()}")

        # Check InventoryCategoryData on CDO
        try:
            data = cdo.get_editor_property('inventory_category_data')
            log(f"InventoryCategoryData: {data}")
            if data:
                try:
                    cat = data.get_editor_property('category')
                    log(f"  .category: {cat}")
                except Exception as e:
                    log(f"  .category error: {e}")
                try:
                    icon = data.get_editor_property('category_icon')
                    log(f"  .category_icon: {icon}")
                    if icon:
                        log(f"  .category_icon path: {icon.get_path_name() if hasattr(icon, 'get_path_name') else str(icon)}")
                except Exception as e:
                    log(f"  .category_icon error: {e}")
        except Exception as e:
            log(f"InventoryCategoryData error: {e}")

    # Now check W_Inventory's AllItemsCategoryEntry widget instance
    log("=" * 80)
    log("CHECKING W_Inventory -> AllItemsCategoryEntry")
    log("=" * 80)

    inv_path = '/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory'
    inv_bp = unreal.load_asset(inv_path)

    if inv_bp:
        log(f"Loaded: {inv_bp.get_name()}")

        # Get the widget tree to find AllItemsCategoryEntry
        try:
            # Load the widget instance to check its properties
            # Widget tree is in the Blueprint, we need to look at the WidgetTree
            widget_tree = inv_bp.widget_tree
            if widget_tree:
                log(f"WidgetTree: {widget_tree}")

                # Try to find AllItemsCategoryEntry in the tree
                all_widgets = widget_tree.all_widgets
                log(f"Widget count: {len(all_widgets) if all_widgets else 0}")

                for w in all_widgets:
                    name = w.get_name()
                    if 'AllItems' in name or 'Category' in name:
                        log(f"  Found: {name} ({w.get_class().get_name()})")

                        # If it's a W_Inventory_CategoryEntry, check its data
                        if 'CategoryEntry' in w.get_class().get_name():
                            try:
                                data = w.get_editor_property('inventory_category_data')
                                log(f"    InventoryCategoryData: {data}")
                                if data:
                                    try:
                                        icon = data.get_editor_property('category_icon')
                                        log(f"    .category_icon: {icon}")
                                    except Exception as e2:
                                        log(f"    .category_icon error: {e2}")
                            except Exception as e:
                                log(f"    InventoryCategoryData error: {e}")
        except Exception as e:
            log(f"WidgetTree error: {e}")

if __name__ == "__main__":
    check_category_data()
