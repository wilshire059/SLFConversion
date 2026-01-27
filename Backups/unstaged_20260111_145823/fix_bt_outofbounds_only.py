"""
Fix ONLY BT_OutOfBounds IntValue: 6 -> 7
This is the ONLY subtree that had an incorrect IntValue in the original backup.
All other subtrees are correct after restoring from backup.
"""
import unreal

def log(msg):
    unreal.log_warning(f"[FIX_OOB] {msg}")

def main():
    log("=" * 70)
    log("FIXING BT_OutOfBounds IntValue (6 -> 7)")
    log("=" * 70)

    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_OutOfBounds"

    bt = unreal.load_asset(bt_path)
    if not bt:
        log("[ERROR] Could not load BT_OutOfBounds")
        return

    log(f"Loaded: {bt.get_name()}")

    # Get root node
    root = bt.get_editor_property("root_node")
    if not root:
        log("[ERROR] No root node")
        return

    # Get decorators on root
    decorators = root.get_editor_property("decorators")
    if not decorators:
        log("[ERROR] No decorators on root")
        return

    fixed = False
    for dec in decorators:
        class_name = dec.get_class().get_name()
        if "Blackboard" in class_name:
            # Check if this is the State decorator
            try:
                key = dec.get_editor_property("blackboard_key")
                key_name = key.get_editor_property("selected_key_name")
                if str(key_name) == "State":
                    current_val = dec.get_editor_property("int_value")
                    log(f"Found State decorator, current IntValue={current_val}")

                    if current_val == 6:
                        dec.set_editor_property("int_value", 7)
                        log(f"[FIXED] Changed IntValue from 6 to 7")
                        fixed = True
                    elif current_val == 7:
                        log(f"[OK] IntValue already correct (7)")
                    else:
                        log(f"[WARN] Unexpected IntValue: {current_val}")
            except Exception as e:
                log(f"[ERROR] {e}")

    if fixed:
        # Save the asset
        unreal.EditorAssetLibrary.save_asset(bt_path)
        log("[OK] Saved BT_OutOfBounds")
    else:
        log("[INFO] No changes needed")

    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
