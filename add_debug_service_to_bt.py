# add_debug_service_to_bt.py
# Add BTS_DebugLog service to BT_Enemy root node

import unreal

def add_debug_service():
    """Add BTS_DebugLog to BT_Enemy root node."""

    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"
    bt = unreal.load_asset(bt_path)

    if not bt:
        print(f"ERROR: Could not load {bt_path}")
        return False

    print(f"Loaded BT: {bt.get_name()}")

    # Load the debug service class
    debug_service_class = unreal.load_class(None, "/Script/SLFConversion.BTS_DebugLog")
    if not debug_service_class:
        print("ERROR: Could not load BTS_DebugLog class")
        return False

    print(f"Loaded debug service class: {debug_service_class.get_name()}")

    # Use EditorUtilityLibrary or BTEditorExtensions if available
    # Try using unreal.BehaviorTreeEditorLibrary if it exists
    try:
        # Try to get root node via reflection
        root_ref = bt.get_editor_property("root_node")
        print(f"Root node ref: {root_ref}")
    except Exception as e:
        print(f"Could not get root_node: {e}")

    # Alternative: Use asset tools to modify
    print("\nChecking for available methods to modify BT...")

    # List BehaviorTree methods
    print("\nBehaviorTree methods/properties:")
    for attr in dir(bt):
        if not attr.startswith('_'):
            print(f"  {attr}")

    return True

def check_bt_structure():
    """Check BT_Enemy structure via export."""

    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"

    # Try to export BT as text
    try:
        exporter = unreal.Exporter()
        # Export to text format
        exported_text = unreal.EditorAssetLibrary.get_metadata_tag(bt_path, "")
        print(f"\nMetadata: {exported_text}")
    except Exception as e:
        print(f"Error exporting: {e}")

    # Load BT and check its subobjects
    bt = unreal.load_asset(bt_path)
    if bt:
        print(f"\nBT Class: {bt.get_class().get_name()}")

        # Try to iterate subobjects
        try:
            # UObject::GetDefaultSubobjects doesn't exist in Python
            # Try asset registry
            registry = unreal.AssetRegistryHelpers.get_asset_registry()
            deps = registry.get_dependencies(bt_path)
            print(f"Dependencies: {deps}")
        except Exception as e:
            print(f"Error: {e}")

if __name__ == "__main__":
    add_debug_service()
    check_bt_structure()
