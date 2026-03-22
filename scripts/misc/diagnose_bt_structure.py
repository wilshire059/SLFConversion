# diagnose_bt_structure.py
# Diagnose the behavior tree structure to find what services are attached

import unreal

def log(msg):
    print(f"[BT_DIAG] {msg}")

def diagnose_bt_node(node, depth=0):
    """Recursively diagnose BT nodes"""
    indent = "  " * depth

    if not node:
        return

    node_name = node.get_class().get_name()
    log(f"{indent}Node: {node_name}")

    # Check for services
    if hasattr(node, 'services'):
        try:
            services = node.get_editor_property('services')
            if services:
                for svc in services:
                    svc_name = svc.get_class().get_name()
                    log(f"{indent}  SERVICE: {svc_name}")
                    # Check interval
                    if hasattr(svc, 'interval'):
                        try:
                            interval = svc.get_editor_property('interval')
                            log(f"{indent}    Interval: {interval}")
                        except:
                            pass
        except Exception as e:
            pass

    # Check for decorators
    if hasattr(node, 'decorators'):
        try:
            decorators = node.get_editor_property('decorators')
            if decorators:
                for dec in decorators:
                    dec_name = dec.get_class().get_name()
                    log(f"{indent}  DECORATOR: {dec_name}")
        except:
            pass

    # Check for children (composite nodes)
    if hasattr(node, 'children'):
        try:
            children = node.get_editor_property('children')
            if children:
                for child in children:
                    diagnose_bt_node(child, depth + 1)
        except:
            pass

    # Check for child_nodes (array)
    if hasattr(node, 'child_nodes'):
        try:
            children = node.get_editor_property('child_nodes')
            if children:
                for child in children:
                    diagnose_bt_node(child, depth + 1)
        except:
            pass

def main():
    log("=" * 70)
    log("Behavior Tree Diagnostic")
    log("=" * 70)

    # Check BT_Combat subtree first (where attacks should happen)
    bt_paths = [
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
        "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy",
    ]

    for bt_path in bt_paths:
        log("")
        log(f"=== {bt_path.split('/')[-1]} ===")

        bt = unreal.load_asset(bt_path)
        if not bt:
            log(f"ERROR: Could not load {bt_path}")
            continue

        log(f"Loaded: {bt.get_name()}")

        # Get root node
        if hasattr(bt, 'root_node'):
            try:
                root = bt.get_editor_property('root_node')
                log(f"Root node: {root.get_class().get_name() if root else 'None'}")
                diagnose_bt_node(root)
            except Exception as e:
                log(f"Error getting root: {e}")

        # Try alternative method
        if hasattr(unreal, 'SLFAutomationLibrary'):
            try:
                result = unreal.SLFAutomationLibrary.export_bt_all_decorator_int_values(bt_path)
                if result:
                    log("")
                    log("Decorator IntValues:")
                    for line in result.split('\n'):
                        if line.strip():
                            log(f"  {line}")
            except:
                pass

    log("")
    log("=" * 70)
    log("Done")

if __name__ == "__main__":
    main()
