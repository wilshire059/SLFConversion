"""
Check BT_Enemy decorator IntValues
"""
import unreal

def log(msg):
    unreal.log_warning(f"[CHECK_BT] {msg}")

def check_bt_decorators():
    bt = unreal.load_asset("/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy")
    if not bt:
        log("Could not load BT_Enemy")
        return

    log(f"Checking BT_Enemy: {bt.get_name()}")
    log(f"  Class: {bt.get_class().get_name()}")

    # Get root node
    root = bt.get_editor_property("root_node")
    if root:
        log(f"  Root: {root.get_name()} ({root.get_class().get_name()})")

    # Try to enumerate all nodes
    # BehaviorTree doesn't expose nodes directly in Python, but we can try
    try:
        # Check blackboard asset
        bb = bt.get_editor_property("blackboard_asset")
        if bb:
            log(f"  Blackboard: {bb.get_name()}")

            # Check keys
            keys = bb.get_editor_property("keys")
            log(f"  Keys count: {len(keys) if keys else 0}")
            for key in keys:
                key_name = key.get_editor_property("entry_name")
                key_class = key.get_class().get_name()
                log(f"    Key: {key_name} ({key_class})")

                # If it's the State key, check its type
                if "State" in str(key_name):
                    try:
                        # Try to get enum type
                        enum_type = key.get_editor_property("enum_type")
                        log(f"      EnumType: {enum_type}")
                    except:
                        pass
    except Exception as e:
        log(f"  Error: {e}")

def main():
    log("=" * 70)
    check_bt_decorators()
    log("=" * 70)

if __name__ == "__main__":
    main()
