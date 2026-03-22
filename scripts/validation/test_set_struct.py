"""
Test setting struct properties with correct types
"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

def test_set_struct():
    log("=" * 80)
    log("TESTING STRUCT PROPERTY SETTING")
    log("=" * 80)

    # Check SLFItemCategory enum - use dir() to find values
    log("\nSLFItemCategory enum attributes:")
    enum_class = unreal.SLFItemCategory
    for attr in dir(enum_class):
        if not attr.startswith('_'):
            log(f"  {attr}")

    # Load texture
    texture_path = '/Game/SoulslikeFramework/Widgets/_Textures/T_Category_AllItems'
    texture = unreal.load_asset(texture_path)
    log(f"\nLoaded texture: {texture}")
    log(f"Texture path: {texture.get_path_name() if texture else 'None'}")

    # Load widget and get struct
    widget_path = '/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory'
    widget_bp = unreal.load_asset(widget_path)

    if not widget_bp:
        log("Failed to load widget")
        return

    instance_path = f"{widget_bp.get_path_name()}:WidgetTree.AllItemsCategoryEntry"
    widget = unreal.find_object(None, instance_path)

    if not widget:
        log("Failed to find widget instance")
        return

    log(f"\nFound widget: {widget}")

    # Get current data
    data = widget.get_editor_property('inventory_category_data')
    log(f"Current data: {data}")

    # Try setting category using enum member directly
    log("\n--- TRYING TO SET CATEGORY ---")
    try:
        # Try ALL_ITEMS
        new_category = unreal.SLFItemCategory.ALL_ITEMS
        log(f"New category value: {new_category}")
        data.set_editor_property('category', new_category)
        log(f"Set category successfully!")
    except Exception as e:
        log(f"ERROR with ALL_ITEMS: {e}")
        # Try other variations
        try:
            new_category = unreal.SLFItemCategory.ALLITEMS
            log(f"Trying ALLITEMS: {new_category}")
            data.set_editor_property('category', new_category)
            log(f"Set category with ALLITEMS!")
        except Exception as e2:
            log(f"ERROR with ALLITEMS: {e2}")

    # Try setting icon - pass the loaded texture object directly
    log("\n--- TRYING TO SET CATEGORY_ICON ---")
    try:
        # Pass the texture object directly (UE might convert to soft ptr)
        data.set_editor_property('category_icon', texture)
        log(f"Set category_icon with texture object!")
    except Exception as e:
        log(f"ERROR with texture object: {e}")

    # Apply modified struct back
    log("\n--- APPLYING STRUCT BACK ---")
    try:
        widget.set_editor_property('inventory_category_data', data)
        log("Applied struct back to widget")

        # Verify
        verify = widget.get_editor_property('inventory_category_data')
        log(f"Verified: {verify}")
    except Exception as e:
        log(f"ERROR applying struct: {e}")

    # Save
    log("\n--- SAVING ---")
    try:
        widget_bp.modify(True)
        result = unreal.EditorAssetLibrary.save_asset(widget_path)
        log(f"Save result: {result}")
    except Exception as e:
        log(f"Save error: {e}")

if __name__ == "__main__":
    test_set_struct()
