"""
Diagnose BT Decorator IntValues for State key comparison
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_DIAG] {msg}")

def check_bt(bt_path):
    bt = unreal.load_asset(bt_path)
    if not bt:
        log(f"Could not load: {bt_path}")
        return

    log(f"")
    log(f"=== {bt.get_name()} ===")

    # Get root decorators
    try:
        root_decs = bt.get_editor_property("root_decorators")
        log(f"Root decorators count: {len(root_decs)}")

        for i, dec in enumerate(root_decs):
            dec_class = dec.get_class().get_name()
            node_name = dec.get_editor_property("node_name") if hasattr(dec, "node_name") else "Unknown"

            log(f"  [{i}] {dec_class}: {node_name}")

            # Check if it's a Blackboard decorator
            if "Blackboard" in dec_class:
                try:
                    # Get the key selector
                    bb_key = dec.get_editor_property("blackboard_key")
                    if bb_key:
                        key_name = bb_key.get_editor_property("selected_key_name")
                        log(f"      Key: {key_name}")

                    # Get IntValue (protected, use reflection info from C++ func)
                    int_value = dec.get_editor_property("int_value")
                    log(f"      IntValue: {int_value}")

                    # Get operation type
                    try:
                        op = dec.get_editor_property("operation_type")
                        log(f"      Operation: {op}")
                    except:
                        pass

                except Exception as e:
                    log(f"      Error reading decorator: {e}")

    except Exception as e:
        log(f"Error getting root_decorators: {e}")

def main():
    log("=" * 70)
    log("DIAGNOSING BT DECORATOR INTVALUES")
    log("=" * 70)

    # Main enemy BT
    check_bt("/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy")

    # Combat subtree
    check_bt("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat")

    # Idle subtree
    check_bt("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle")

    # Patrolling subtree
    check_bt("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Patrolling")

    log("")
    log("=" * 70)
    log("Expected State Values:")
    log("  0 = Idle")
    log("  1 = RandomRoam")
    log("  2 = Patrolling")
    log("  3 = Investigating")
    log("  4 = Combat")
    log("  5 = PoiseBroken")
    log("  6 = Uninterruptable")
    log("  7 = OutOfBounds")
    log("=" * 70)

if __name__ == "__main__":
    main()
