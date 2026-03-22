"""
Check BT_Enemy structure and decorator setup
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_CHECK] {msg}")

def check_node_recursive(node, depth=0):
    if not node:
        return
    
    indent = "  " * depth
    node_name = node.get_name()
    node_class = node.get_class().get_name()
    log(f"{indent}{node_name} ({node_class})")
    
    # Check for decorators
    try:
        decorators = node.get_editor_property("decorators")
        if decorators:
            for dec in decorators:
                dec_name = dec.get_name()
                dec_class = dec.get_class().get_name()
                log(f"{indent}  [DEC] {dec_name} ({dec_class})")
                
                # If it's a blackboard decorator, check its config
                if "Blackboard" in dec_class:
                    try:
                        key_selector = dec.get_editor_property("blackboard_key")
                        if key_selector:
                            key_name = key_selector.selected_key_name
                            log(f"{indent}    Key: {key_name}")
                    except:
                        pass
                    try:
                        int_value = dec.get_editor_property("int_value")
                        log(f"{indent}    IntValue: {int_value}")
                    except:
                        pass
    except:
        pass
    
    # Check for services
    try:
        services = node.get_editor_property("services")
        if services:
            for svc in services:
                svc_name = svc.get_name()
                svc_class = svc.get_class().get_name()
                log(f"{indent}  [SVC] {svc_name} ({svc_class})")
    except:
        pass
    
    # Check children
    try:
        children = node.get_editor_property("children")
        if children:
            for child in children:
                child_node = child.get_editor_property("child_composite") or child.get_editor_property("child_task")
                check_node_recursive(child_node, depth + 1)
    except:
        pass

def main():
    log("=" * 70)
    log("CHECKING BT_ENEMY STRUCTURE")
    log("=" * 70)
    
    bt = unreal.load_asset("/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy")
    if not bt:
        log("ERROR: Could not load BT_Enemy")
        return
    
    log(f"Loaded: {bt.get_name()}")
    log(f"Class: {bt.get_class().get_name()}")
    
    # Get root node
    try:
        root = bt.get_editor_property("root_node")
        if root:
            log(f"Root: {root.get_name()} ({root.get_class().get_name()})")
            check_node_recursive(root)
    except Exception as e:
        log(f"Error: {e}")
    
    log("=" * 70)

if __name__ == "__main__":
    main()
