"""
Apply widget instance property data after migration.
This runs in Unreal Python AFTER the migration script has reparented widgets.

It uses find_object to locate widget instances in the WidgetTree and applies
the property values that were extracted from the backup.
"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

def apply_widget_instance_data():
    log("=" * 80)
    log("APPLYING WIDGET INSTANCE DATA AFTER MIGRATION")
    log("=" * 80)

    # Define the widget instance data to apply
    # AllItemsCategoryEntry uses Category=None (shows all items) with T_Category_AllItems icon
    WIDGET_INSTANCE_DATA = [
        {
            'widget_bp_path': '/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory',
            'instance_name': 'AllItemsCategoryEntry',
            'properties': {
                'inventory_category_data': {
                    'category': 'NONE',  # ESLFItemCategory::None (shows all items)
                    'category_icon': '/Game/SoulslikeFramework/Widgets/_Textures/T_Category_AllItems',
                    'display_name_override': ''
                }
            }
        }
    ]

    modified_assets = []

    for data in WIDGET_INSTANCE_DATA:
        widget_bp_path = data['widget_bp_path']
        instance_name = data['instance_name']
        properties = data['properties']

        log(f"\nProcessing: {widget_bp_path} -> {instance_name}")

        # Load the widget blueprint first to ensure it's in memory
        widget_bp = unreal.load_asset(widget_bp_path)
        if not widget_bp:
            log(f"  ERROR: Failed to load widget blueprint")
            continue

        log(f"  Loaded BP: {widget_bp.get_name()}")

        # Construct the full object path to the widget instance
        bp_full_path = widget_bp.get_path_name()
        instance_path = f"{bp_full_path}:WidgetTree.{instance_name}"

        log(f"  Looking for: {instance_path}")

        # Find the widget instance
        widget_instance = unreal.find_object(None, instance_path)

        if not widget_instance:
            log(f"  ERROR: Widget instance not found at {instance_path}")
            continue

        log(f"  Found widget: {widget_instance.get_name()} ({widget_instance.get_class().get_name()})")

        # Apply properties
        for prop_name, prop_data in properties.items():
            if prop_name == 'inventory_category_data':
                apply_inventory_category_data(widget_instance, prop_data)

        # Mark the blueprint as modified and save
        modified_assets.append((widget_bp, widget_bp_path))

    # Save modified assets
    if modified_assets:
        log("\n" + "=" * 80)
        log("SAVING MODIFIED ASSETS")
        log("=" * 80)

        for asset, asset_path in modified_assets:
            try:
                # Need to mark as dirty first
                asset.modify(True)

                # Save via EditorAssetLibrary
                log(f"Saving: {asset_path}")
                result = unreal.EditorAssetLibrary.save_asset(asset_path)
                if result:
                    log(f"  Saved successfully")
                else:
                    log(f"  WARNING: Save returned False")
            except Exception as e:
                log(f"  ERROR saving: {e}")

    log("\n" + "=" * 80)
    log("WIDGET INSTANCE DATA APPLICATION COMPLETE")
    log("=" * 80)

def apply_inventory_category_data(widget, data_dict):
    """Apply InventoryCategoryData to a widget instance"""
    log(f"  Applying InventoryCategoryData...")

    try:
        # Get current InventoryCategoryData struct
        current_data = widget.get_editor_property('inventory_category_data')
        log(f"    Current: {current_data}")

        # Set category (enum value)
        if 'category' in data_dict:
            try:
                cat_name = data_dict['category']
                new_category = getattr(unreal.SLFItemCategory, cat_name, None)
                if new_category is not None:
                    current_data.set_editor_property('category', new_category)
                    log(f"    Set category to: {cat_name}")
                else:
                    log(f"    WARNING: Category '{cat_name}' not found in enum")
            except Exception as e:
                log(f"    ERROR setting category: {e}")

        # Set category icon - load texture and pass directly
        if 'category_icon' in data_dict and data_dict['category_icon']:
            try:
                icon_path = data_dict['category_icon']
                texture = unreal.load_asset(icon_path)
                if texture:
                    current_data.set_editor_property('category_icon', texture)
                    log(f"    Set category_icon to: {icon_path}")
                else:
                    log(f"    WARNING: Could not load texture {icon_path}")
            except Exception as e:
                log(f"    ERROR setting category_icon: {e}")

        # Set display name override
        if 'display_name_override' in data_dict:
            try:
                display_name = data_dict['display_name_override']
                if display_name:
                    current_data.set_editor_property('display_name_override', unreal.Text(display_name))
                    log(f"    Set display_name_override to: {display_name}")
            except Exception as e:
                log(f"    ERROR setting display_name_override: {e}")

        # Apply the modified struct back to the widget
        widget.set_editor_property('inventory_category_data', current_data)

        # Verify
        verify_data = widget.get_editor_property('inventory_category_data')
        log(f"    Verified: {verify_data}")

    except Exception as e:
        log(f"  ERROR in apply_inventory_category_data: {e}")
        import traceback
        log(traceback.format_exc())

if __name__ == "__main__":
    apply_widget_instance_data()
