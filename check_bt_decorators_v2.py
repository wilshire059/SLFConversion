"""
Check BT_Enemy decorator configuration by iterating nodes
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_DEC] {msg}")

def check_decorators(node, depth=0):
    if not node:
        return
    
    indent = "  " * depth
    node_name = node.get_name()
    node_class = node.get_class().get_name()
    
    log(f"{indent}Node: {node_name} ({node_class})")
    
    # Check if this node has decorators
    # BTCompositeNode has Decorators property
    try:
        if hasattr(node, 'get_editor_property'):
            # Try to get Children for composites
            try:
                children = node.get_editor_property('children')
                if children:
                    log(f"{indent}  Children count: {len(children)}")
                    for i, child in enumerate(children):
                        # Each child is FBTCompositeChild which has child_composite/child_task
                        try:
                            child_node = child.get_editor_property('child_composite')
                            if not child_node:
                                child_node = child.get_editor_property('child_task')
                            if child_node:
                                check_decorators(child_node, depth + 1)
                        except:
                            pass
                            
                        # Also check decorators on this branch
                        try:
                            decorators = child.get_editor_property('decorators')
                            if decorators:
                                for dec in decorators:
                                    dec_name = dec.get_name()
                                    dec_class = dec.get_class().get_name()
                                    log(f"{indent}    Decorator: {dec_name} ({dec_class})")
                                    
                                    # If BTDecorator_Blackboard, get key and comparison info
                                    if 'Blackboard' in dec_class:
                                        try:
                                            key = dec.get_editor_property('blackboard_key')
                                            log(f"{indent}      Key: {key.selected_key_name if key else 'None'}")
                                        except Exception as e:
                                            log(f"{indent}      Key error: {e}")
                                        
                                        try:
                                            int_val = dec.get_editor_property('int_value')
                                            log(f"{indent}      IntValue: {int_val}")
                                        except:
                                            pass
                                        
                                        try:
                                            basic_op = dec.get_editor_property('basic_operation')
                                            log(f"{indent}      BasicOp: {basic_op}")
                                        except:
                                            pass
                        except:
                            pass
            except:
                pass
    except Exception as e:
        log(f"{indent}  Error: {e}")

def main():
    log("=" * 70)
    log("CHECKING BT_ENEMY DECORATORS")
    log("=" * 70)
    
    bt = unreal.load_asset("/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy")
    if not bt:
        log("ERROR: Could not load BT_Enemy")
        return
    
    log(f"Loaded: {bt.get_name()}")
    
    # Get root node
    try:
        # Use BlackboardAsset to get info
        bb = bt.get_editor_property('blackboard_asset')
        if bb:
            log(f"Blackboard: {bb.get_name()}")
    except:
        pass
    
    # Try to access RootNode
    try:
        # BehaviorTree has RootNode property
        root = bt.root_node
        if root:
            log(f"Root: {root.get_name()}")
            check_decorators(root, 0)
    except Exception as e:
        log(f"Root access error: {e}")
    
    log("=" * 70)

if __name__ == "__main__":
    main()
