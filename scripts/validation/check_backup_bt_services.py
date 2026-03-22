# check_backup_bt_services.py
# Check if backup BT_Combat has BTS_TryGetAbility service attached

import unreal

def log(msg):
    print(f"[BACKUP_BT] {msg}")

def check_services_recursive(node, depth=0):
    """Recursively check for services on BT nodes"""
    if not node:
        return

    indent = "  " * depth
    node_class = node.get_class().get_name()

    # Get display name
    display_name = ""
    try:
        display_name = node.get_editor_property('node_name')
    except:
        pass

    # Check services
    try:
        services = node.get_editor_property('services')
        if services and len(services) > 0:
            log(f"{indent}{display_name or node_class}: {len(services)} services")
            for svc in services:
                svc_class = svc.get_class().get_name()
                log(f"{indent}  SERVICE: {svc_class}")
    except:
        pass

    # Recurse children
    try:
        children = node.get_editor_property('children')
        if children:
            for child in children:
                try:
                    child_node = child.get_editor_property('child_node') if hasattr(child, 'get_editor_property') else child
                    check_services_recursive(child_node, depth + 1)
                except:
                    check_services_recursive(child, depth + 1)
    except:
        pass

def main():
    log("=" * 70)
    log("Checking BT_Combat Services")
    log("=" * 70)

    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat"

    bt = unreal.load_asset(bt_path)
    if not bt:
        log(f"ERROR: Could not load {bt_path}")
        return

    log(f"Loaded: {bt.get_name()}")

    root = bt.get_editor_property('root_node')
    if root:
        check_services_recursive(root)
    else:
        log("No root node")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
