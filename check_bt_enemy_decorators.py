"""
Check BT_Enemy main tree decorators for State key comparison.
"""
import unreal

def main():
    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"
    bt = unreal.load_asset(bt_path)
    if not bt:
        print(f"ERROR: Could not load {bt_path}")
        return

    print(f"BehaviorTree: {bt.get_name()}")

    # Check root node
    root = bt.get_editor_property("root_node")
    if root:
        print(f"Root: {root.get_name()} - {root.get_class().get_name()}")

        # Check if root has decorators
        if hasattr(root, 'decorators'):
            decorators = root.get_editor_property("decorators")
            print(f"  Decorators: {len(decorators) if decorators else 0}")
            if decorators:
                for dec in decorators:
                    print(f"    - {dec.get_name()} ({dec.get_class().get_name()})")

        # Check children
        if hasattr(root, 'children'):
            children = root.get_editor_property("children")
            print(f"  Children: {len(children) if children else 0}")
            if children:
                for i, child in enumerate(children):
                    # Access child task or composite
                    child_task = child.get_editor_property("child_task") if hasattr(child, 'child_task') else None
                    child_composite = child.get_editor_property("child_composite") if hasattr(child, 'child_composite') else None

                    node = child_task or child_composite
                    if node:
                        print(f"    [{i}] {node.get_name()} ({node.get_class().get_name()})")

                        # Check decorators on this child
                        if hasattr(child, 'decorators'):
                            decs = child.get_editor_property("decorators")
                            if decs:
                                for dec in decs:
                                    dec_class = dec.get_class().get_name()
                                    print(f"        Decorator: {dec.get_name()} ({dec_class})")

                                    # If it's a blackboard decorator, show key
                                    if "Blackboard" in dec_class:
                                        key = dec.get_editor_property("blackboard_key")
                                        if key:
                                            key_name = key.get_editor_property("selected_key_name")
                                            print(f"          Key: {key_name}")

    # Also check root decorators
    root_decs = bt.get_editor_property("root_decorators")
    if root_decs:
        print(f"\nRoot Decorators: {len(root_decs)}")
        for dec in root_decs:
            print(f"  - {dec.get_name()} ({dec.get_class().get_name()})")

if __name__ == "__main__":
    main()
