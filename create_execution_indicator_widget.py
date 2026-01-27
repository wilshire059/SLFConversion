"""Create W_TargetExecutionIndicator widget Blueprint."""
import unreal

def create_execution_indicator_widget():
    """Create the W_TargetExecutionIndicator widget Blueprint."""

    # Check if C++ class exists
    cpp_class_path = '/Script/SLFConversion.W_TargetExecutionIndicator'
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        print("ERROR: C++ class not found: " + cpp_class_path)
        print("Make sure the C++ project is compiled.")
        return False

    print("OK: C++ class found: " + cpp_class_path)

    # Create widget Blueprint
    package_path = '/Game/SoulslikeFramework/Widgets/HUD'
    asset_name = 'W_TargetExecutionIndicator'
    full_path = package_path + '/' + asset_name

    # Check if already exists
    existing = unreal.load_asset(full_path)
    if existing:
        print("Widget already exists: " + full_path)
        return True

    # Create the widget Blueprint
    factory = unreal.WidgetBlueprintFactory()
    factory.set_editor_property('parent_class', cpp_class)

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    widget_bp = asset_tools.create_asset(asset_name, package_path, unreal.WidgetBlueprint, factory)

    if widget_bp:
        print("CREATED: " + full_path)

        # Save the asset
        unreal.EditorAssetLibrary.save_asset(full_path)
        print("SAVED: " + full_path)
        return True
    else:
        print("ERROR: Failed to create widget Blueprint")
        return False

if __name__ == "__main__":
    success = create_execution_indicator_widget()
    if success:
        print("=== Widget creation complete ===")
    else:
        print("=== Widget creation FAILED ===")
