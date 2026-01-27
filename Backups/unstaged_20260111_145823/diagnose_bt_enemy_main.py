"""
Diagnose BT_Enemy main tree structure to see how it calls subtrees
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_ENEMY] {msg}")

def dump_node(node, indent=0):
    """Recursively dump node structure"""
    prefix = "  " * indent
    class_name = node.get_class().get_name()
    node_name = node.get_editor_property("node_name") if hasattr(node, "node_name") else ""

    log(f"{prefix}[{class_name}] {node_name}")

    # Check for BTTask_RunBehavior
    if "RunBehavior" in class_name or "RunBehaviorDynamic" in class_name:
        try:
            subtree = node.get_editor_property("behavior_asset")
            if subtree:
                log(f"{prefix}  -> Subtree: {subtree.get_name()}")
            else:
                log(f"{prefix}  -> Subtree: NULL")
        except:
            pass
        try:
            # For dynamic version
            key = node.get_editor_property("behavior_tree_key")
            if key:
                key_name = key.get_editor_property("selected_key_name")
                log(f"{prefix}  -> DynamicKey: {key_name}")
        except:
            pass

    # Check decorators
    try:
        decorators = node.get_editor_property("decorators")
        if decorators:
            for dec in decorators:
                dec_class = dec.get_class().get_name()
                dec_name = dec.get_editor_property("node_name") if hasattr(dec, "node_name") else ""
                log(f"{prefix}  [Decorator] {dec_class} - {dec_name}")

                # If blackboard decorator, show key info
                if "Blackboard" in dec_class:
                    try:
                        key = dec.get_editor_property("blackboard_key")
                        key_name = key.get_editor_property("selected_key_name")
                        int_val = dec.get_editor_property("int_value")
                        log(f"{prefix}    Key: {key_name}, IntValue: {int_val}")
                    except Exception as e:
                        pass
    except:
        pass

    # Check children (for composite nodes)
    try:
        children = node.get_editor_property("children")
        if children:
            for i, child in enumerate(children):
                child_node = child.get_editor_property("child_task") if hasattr(child, "child_task") else None
                if not child_node:
                    child_node = child.get_editor_property("child_composite") if hasattr(child, "child_composite") else None
                if child_node:
                    dump_node(child_node, indent + 1)
                else:
                    # Try to get wrapped node
                    log(f"{prefix}  [Child {i}] (wrapped)")
                    dump_node(child, indent + 1)
    except Exception as e:
        pass

def main():
    log("=" * 70)
    log("DIAGNOSING BT_ENEMY MAIN TREE")
    log("=" * 70)

    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"

    bt = unreal.load_asset(bt_path)
    if not bt:
        log("[ERROR] Could not load BT_Enemy")
        return

    log(f"Loaded: {bt.get_name()}")

    # Get blackboard
    bb = bt.get_editor_property("blackboard_asset")
    if bb:
        log(f"BlackboardAsset: {bb.get_name()}")

    # Get root
    root = bt.get_editor_property("root_node")
    if not root:
        log("[ERROR] No root node")
        return

    log("")
    log("=== ROOT NODE ===")

    # Check root decorators
    try:
        decorators = root.get_editor_property("decorators")
        if decorators:
            log(f"Root Decorators: {len(decorators)}")
            for dec in decorators:
                log(f"  - {dec.get_class().get_name()}")
    except:
        pass

    # Dump child structure
    try:
        child = root.get_editor_property("child")
        if child:
            log("")
            log("=== TREE STRUCTURE ===")
            dump_node(child)
    except Exception as e:
        log(f"[ERROR] {e}")

    # Use export function for full detail
    log("")
    log("=== USING EXPORT FUNCTION ===")
    result = unreal.SLFAutomationLibrary.export_behavior_tree_structure(bt_path)
    if result:
        for line in result.split('\n'):
            log(line)

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
