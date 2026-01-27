"""
Diagnose BT decorator IntValue configuration
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_DEC_DIAG] {msg}")

def main():
    log("=" * 70)
    log("BT DECORATOR DIAGNOSTIC")
    log("=" * 70)

    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat"
    bt = unreal.load_asset(bt_path)

    if not bt:
        log(f"[ERROR] Could not load: {bt_path}")
        return

    log(f"Loaded: {bt.get_name()}")
    log(f"Class: {bt.get_class().get_name()}")

    # Try to list all properties
    log("")
    log("Available properties:")
    try:
        for prop in dir(bt):
            if not prop.startswith('_') and not callable(getattr(bt, prop, None)):
                try:
                    val = bt.get_editor_property(prop)
                    log(f"  {prop}: {type(val).__name__}")
                except:
                    pass
    except Exception as e:
        log(f"Error listing props: {e}")

    # Try multiple property names for root decorators
    for prop_name in ["root_decorators", "RootDecorators", "decorators", "Decorators"]:
        try:
            decorators = bt.get_editor_property(prop_name)
            log(f"")
            log(f"Found '{prop_name}': {len(decorators) if decorators else 'None'}")
            if decorators:
                for i, dec in enumerate(decorators):
                    log(f"  [{i}] {dec.get_class().get_name() if dec else 'None'}")
                break
        except Exception as e:
            log(f"  {prop_name}: not found or error: {e}")

    # Try to get the root node
    try:
        root_node = bt.get_editor_property("root_node")
        log(f"")
        log(f"Root node: {root_node.get_class().get_name() if root_node else 'None'}")
    except Exception as e:
        log(f"Root node error: {e}")

    # Use C++ function to get detailed structure
    log("")
    log("Using C++ export function:")
    result = unreal.SLFAutomationLibrary.export_behavior_tree_structure(bt_path)
    for line in result.split('\n')[:40]:
        log(f"  {line}")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
