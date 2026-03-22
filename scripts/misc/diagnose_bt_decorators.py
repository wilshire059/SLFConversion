# diagnose_bt_decorators.py
# Examine BT_Enemy structure and decorator configurations

import unreal

def diagnose_bt_enemy():
    """Diagnose the BT_Enemy behavior tree structure and decorators."""

    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"
    bt = unreal.load_asset(bt_path)

    if not bt:
        print(f"ERROR: Could not load {bt_path}")
        return

    print(f"\n=== BT_Enemy Diagnosis ===")
    print(f"Class: {bt.get_class().get_name()}")

    # List all properties
    print("\nBehaviorTree properties:")
    for prop in dir(bt):
        if not prop.startswith('_'):
            try:
                val = getattr(bt, prop)
                if not callable(val):
                    print(f"  {prop}: {type(val).__name__}")
            except:
                pass

    # Try getting blackboard
    try:
        bb_asset = bt.get_editor_property("blackboard_asset")
        if bb_asset:
            print(f"\nBlackboard: {bb_asset.get_name()}")
            diagnose_blackboard(bb_asset)
    except Exception as e:
        print(f"Error getting blackboard: {e}")

def diagnose_blackboard(bb_asset):
    """Diagnose blackboard keys."""
    print(f"\n=== Blackboard Keys ===")

    # Try to get keys array
    try:
        keys = bb_asset.get_editor_property("keys")
        if keys:
            for key in keys:
                try:
                    key_name = key.get_editor_property("entry_name")
                    key_class = key.get_class().get_name()
                    print(f"  Key: {key_name} ({key_class})")

                    # If it's an enum key, try to get enum type
                    if "Enum" in key_class:
                        try:
                            enum_type = key.get_editor_property("enum_type")
                            print(f"    EnumType: {enum_type}")
                        except:
                            pass
                        try:
                            enum_name = key.get_editor_property("enum_name")
                            print(f"    EnumName: {enum_name}")
                        except:
                            pass
                except Exception as e:
                    print(f"  Error reading key: {e}")
    except Exception as e:
        print(f"Error getting keys: {e}")

    # List all BB properties
    print("\nBlackboard properties:")
    for prop in dir(bb_asset):
        if not prop.startswith('_'):
            try:
                val = getattr(bb_asset, prop)
                if not callable(val):
                    print(f"  {prop}: {val}")
            except:
                pass

def export_bt_via_text():
    """Export BT via export_text to see its structure."""
    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"

    # Use automation library if available
    try:
        result = unreal.SLFAutomationLibrary.export_object_properties(bt_path)
        if result:
            print("\n=== BT_Enemy Export Text ===")
            # Look for decorator-related content
            lines = result.split('\n')
            in_decorator = False
            for i, line in enumerate(lines):
                if "Decorator" in line or "IntValue" in line or "BlackboardKey" in line or "FlowAbortMode" in line:
                    print(line)
    except Exception as e:
        print(f"Error using SLFAutomationLibrary: {e}")

if __name__ == "__main__":
    diagnose_bt_enemy()
    export_bt_via_text()
