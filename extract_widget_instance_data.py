"""
Extract widget instance property data from BACKUP W_Inventory
This extracts InventoryCategoryData from CategoryEntry widget instances
for migration to C++ properties.
"""
import unreal
import json
import os

def log(msg):
    unreal.log_warning(str(msg))

def extract_widget_instance_data():
    log("=" * 80)
    log("EXTRACTING WIDGET INSTANCE DATA FROM BACKUP")
    log("=" * 80)

    # We need to load from the BACKUP content path
    # The backup is at C:/scripts/bp_only/Content/...
    # But Unreal can only load from project Content folder

    # So we need to:
    # 1. First run this BEFORE migration (on backup content)
    # 2. Or copy backup to a temp location in Content folder

    # For now, let's extract from current content (post-migration)
    # to understand the structure, then we'll handle backup separately

    inv_path = '/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory'
    inv_bp = unreal.load_asset(inv_path)

    if not inv_bp:
        log(f"ERROR: Failed to load {inv_path}")
        return None

    log(f"Loaded: {inv_bp.get_name()}")
    log(f"Class: {inv_bp.get_class().get_name()}")

    extracted_data = {}

    # Try to access the widget tree through EditorUtilityLibrary
    try:
        # Get all widgets from the blueprint
        # WidgetBlueprint has a WidgetTree property
        widget_tree = inv_bp.get_editor_property('widget_tree')
        log(f"WidgetTree: {widget_tree}")

        if widget_tree:
            # Get root widget
            root = widget_tree.get_editor_property('root_widget')
            log(f"Root widget: {root}")

            # Recursively find all category entry widgets
            all_widgets = []
            find_all_widgets(root, all_widgets)
            log(f"Found {len(all_widgets)} widgets total")

            for w in all_widgets:
                widget_name = w.get_name()
                widget_class = w.get_class().get_name()

                # Check if this is a CategoryEntry widget
                if 'CategoryEntry' in widget_class or 'CategoryEntry' in widget_name:
                    log(f"  Found CategoryEntry: {widget_name} ({widget_class})")

                    # Try to get InventoryCategoryData
                    try:
                        data = w.get_editor_property('inventory_category_data')
                        if data:
                            category = None
                            icon_path = None
                            display_name = ""

                            try:
                                category = str(data.get_editor_property('category'))
                            except:
                                pass

                            try:
                                icon = data.get_editor_property('category_icon')
                                if icon:
                                    icon_path = str(icon)
                            except:
                                pass

                            try:
                                display_name = str(data.get_editor_property('display_name_override'))
                            except:
                                pass

                            extracted_data[widget_name] = {
                                'category': category,
                                'category_icon': icon_path,
                                'display_name_override': display_name
                            }
                            log(f"    Category: {category}")
                            log(f"    CategoryIcon: {icon_path}")
                            log(f"    DisplayNameOverride: {display_name}")
                    except Exception as e:
                        log(f"    Error getting InventoryCategoryData: {e}")

    except Exception as e:
        log(f"Error accessing widget tree: {e}")

        # Alternative: try using WidgetBlueprintLibrary
        try:
            log("Trying alternative approach with EditorSubsystem...")
            subsystem = unreal.get_editor_subsystem(unreal.AssetEditorSubsystem)
            if subsystem:
                log(f"AssetEditorSubsystem: {subsystem}")
        except Exception as e2:
            log(f"Alternative approach error: {e2}")

    return extracted_data

def find_all_widgets(widget, result_list):
    """Recursively find all widgets in the tree"""
    if widget is None:
        return

    result_list.append(widget)

    # Check if this widget is a panel/container
    try:
        # For PanelWidget (has slots)
        if hasattr(widget, 'get_child_at'):
            child_count = widget.get_children_count() if hasattr(widget, 'get_children_count') else 0
            for i in range(child_count):
                child = widget.get_child_at(i)
                find_all_widgets(child, result_list)
    except:
        pass

    # Try named slot
    try:
        if hasattr(widget, 'get_content_slot'):
            content = widget.get_content_slot()
            if content:
                find_all_widgets(content.content, result_list)
    except:
        pass

if __name__ == "__main__":
    data = extract_widget_instance_data()
    if data:
        log(f"Extracted data: {json.dumps(data, indent=2)}")
    else:
        log("No data extracted")
