# diagnose_bt_combat.py
# Inspect BT_Combat subtree structure to find the attack task

import unreal

def log(msg):
    unreal.log_warning(f"[BT_DIAG] {msg}")

def inspect_bt_node(node, indent=0):
    """Recursively inspect a BT node and its children"""
    prefix = "  " * indent

    if not node:
        log(f"{prefix}(null node)")
        return

    node_name = node.get_name()
    node_class = node.get_class().get_name()
    log(f"{prefix}Node: {node_name} ({node_class})")

    # Check for decorators
    if hasattr(node, 'get_decorators_num'):
        num_decorators = node.get_decorators_num()
        if num_decorators > 0:
            log(f"{prefix}  Decorators: {num_decorators}")

    # Check for services
    if hasattr(node, 'get_services_num'):
        num_services = node.get_services_num()
        if num_services > 0:
            log(f"{prefix}  Services: {num_services}")

    # Check for children (composite nodes)
    if hasattr(node, 'get_children_num'):
        num_children = node.get_children_num()
        if num_children > 0:
            log(f"{prefix}  Children: {num_children}")
            for i in range(num_children):
                child = node.get_child_node(i)
                inspect_bt_node(child, indent + 2)

def main():
    log("=" * 70)
    log("BT_Combat Structure Diagnosis")
    log("=" * 70)

    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat"

    bt = unreal.load_asset(bt_path)
    if not bt:
        log(f"ERROR: Could not load {bt_path}")
        return

    log(f"Loaded: {bt.get_name()}")

    # Get root node
    root_node = bt.get_editor_property("root_node")
    if root_node:
        log(f"Root node exists: {root_node.get_name()}")
        inspect_bt_node(root_node)
    else:
        log("ERROR: No root node!")

    log("")
    log("=" * 70)
    log("Looking for BTT_TryExecuteAbility references...")
    log("=" * 70)

    # Also check main BT_Enemy
    bt_enemy_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"
    bt_enemy = unreal.load_asset(bt_enemy_path)
    if bt_enemy:
        log(f"Loaded: {bt_enemy.get_name()}")
        root = bt_enemy.get_editor_property("root_node")
        if root:
            inspect_bt_node(root)

    log("=" * 70)
    log("Done!")
    log("=" * 70)

if __name__ == "__main__":
    main()
