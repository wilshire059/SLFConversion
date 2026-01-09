"""
Check available enum types and their values
"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

def check_enum_types():
    log("=" * 80)
    log("CHECKING ENUM AND SOFT OBJECT TYPES")
    log("=" * 80)

    # Look for SLFItemCategory enum
    log("\nSearching for SLF enums...")
    for attr in dir(unreal):
        if 'SLF' in attr or 'ItemCategory' in attr:
            log(f"  Found: {attr}")
            try:
                obj = getattr(unreal, attr)
                if hasattr(obj, '__members__'):
                    log(f"    Members: {list(obj.__members__.keys())}")
            except:
                pass

    # Try different enum formats
    log("\n" + "=" * 80)
    log("TRYING ENUM ACCESS")
    log("=" * 80)

    enum_attempts = [
        'ESLFItemCategory',
        'SLFItemCategory',
        'E_SLFItemCategory',
    ]

    for attempt in enum_attempts:
        try:
            enum_class = getattr(unreal, attempt, None)
            if enum_class:
                log(f"\nFound {attempt}:")
                log(f"  Type: {type(enum_class)}")
                if hasattr(enum_class, '__members__'):
                    for name in enum_class.__members__:
                        log(f"    {name}")
        except Exception as e:
            log(f"  {attempt}: {e}")

    # Check soft object handling
    log("\n" + "=" * 80)
    log("TESTING SOFT OBJECT")
    log("=" * 80)

    texture_path = '/Game/SoulslikeFramework/Widgets/_Textures/T_Category_AllItems'

    # Try loading the texture
    try:
        texture = unreal.load_asset(texture_path)
        log(f"Loaded texture: {texture}")
        if texture:
            log(f"  Class: {texture.get_class().get_name()}")
            log(f"  Path: {texture.get_path_name()}")
    except Exception as e:
        log(f"Texture load error: {e}")

    # Test creating struct and setting properties
    log("\n" + "=" * 80)
    log("TESTING STRUCT PROPERTY SETTING")
    log("=" * 80)

    # Load widget and get struct
    widget_path = '/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory'
    widget_bp = unreal.load_asset(widget_path)

    if widget_bp:
        instance_path = f"{widget_bp.get_path_name()}:WidgetTree.AllItemsCategoryEntry"
        widget = unreal.find_object(None, instance_path)

        if widget:
            log(f"Found widget: {widget}")
            data = widget.get_editor_property('inventory_category_data')
            log(f"Current data: {data}")
            log(f"Data type: {type(data)}")

            # Get struct class info
            log(f"\nStruct attributes:")
            for attr in dir(data):
                if not attr.startswith('_'):
                    log(f"  {attr}")

            # Try to get property info
            try:
                cat_val = data.get_editor_property('category')
                log(f"\ncategory value: {cat_val}")
                log(f"category type: {type(cat_val)}")
            except Exception as e:
                log(f"category error: {e}")

            try:
                icon_val = data.get_editor_property('category_icon')
                log(f"\ncategory_icon value: {icon_val}")
                log(f"category_icon type: {type(icon_val)}")
            except Exception as e:
                log(f"category_icon error: {e}")

if __name__ == "__main__":
    check_enum_types()
