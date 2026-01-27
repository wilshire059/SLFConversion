# update_bt_decorators.py
# Updates BT subtrees to use C++ BTD_StateEquals decorator
# This is the AAA approach: single State enum drives all BT selection

import unreal

# Map of BT subtree to required ESLFAIStates value
# From SLFEnums.h:
#   Idle = 0
#   RandomRoam = 1
#   Patrolling = 2
#   Investigating = 3
#   Combat = 4
#   PoiseBroken = 5
#   Uninterruptable = 6
#   OutOfBounds = 7

BT_STATE_MAP = {
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle": 0,           # Idle
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam": 1,     # RandomRoam
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PatrolPath": 2,     # Patrolling
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Investigating": 3,  # Investigating
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat": 4,         # Combat
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PoiseBroken": 5,    # PoiseBroken
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Uninterruptable": 6, # Uninterruptable
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_OutOfBounds": 7,    # OutOfBounds
}

def get_bt_info(bt_path):
    """Get information about a behavior tree's structure"""
    bt = unreal.load_asset(bt_path)
    if not bt:
        return None

    info = {
        "name": bt.get_name(),
        "path": bt_path,
        "root_node": None,
        "decorators": []
    }

    # Get root node
    root = bt.get_editor_property("root_node")
    if root:
        info["root_node"] = root.get_class().get_name()

        # Try to get decorators on root branches
        try:
            children = root.get_editor_property("children")
            if children:
                for i, child in enumerate(children):
                    child_decorators = child.get_editor_property("decorators")
                    if child_decorators:
                        for dec in child_decorators:
                            info["decorators"].append({
                                "branch": i,
                                "class": dec.get_class().get_name(),
                                "name": dec.get_editor_property("node_name") if hasattr(dec, "node_name") else "Unknown"
                            })
        except Exception as e:
            info["decorator_error"] = str(e)

    return info


def diagnose_all_bts():
    """Diagnose all BT subtrees to understand current structure"""
    print("=" * 60)
    print("BT SUBTREE DIAGNOSIS")
    print("=" * 60)

    for bt_path, state_value in BT_STATE_MAP.items():
        info = get_bt_info(bt_path)
        if info:
            print(f"\n{info['name']} (State={state_value})")
            print(f"  Root: {info['root_node']}")
            print(f"  Decorators: {len(info['decorators'])}")
            for dec in info["decorators"]:
                print(f"    [{dec['branch']}] {dec['class']}: {dec['name']}")
            if "decorator_error" in info:
                print(f"  Error: {info['decorator_error']}")
        else:
            print(f"\nFAILED to load: {bt_path}")

    print("\n" + "=" * 60)


def check_cpp_decorator_class():
    """Check if BTD_StateEquals C++ class is available"""
    try:
        # Try to load the C++ decorator class
        decorator_class = unreal.load_class(None, "/Script/SLFConversion.BTD_StateEquals")
        if decorator_class:
            print("BTD_StateEquals C++ class found!")
            return decorator_class
        else:
            print("BTD_StateEquals class not found in /Script/SLFConversion")
            return None
    except Exception as e:
        print(f"Error loading BTD_StateEquals: {e}")
        return None


def create_state_decorator(state_value, decorator_class):
    """Create a new BTD_StateEquals decorator with the given state value"""
    if not decorator_class:
        print("No decorator class provided")
        return None

    try:
        # Create new decorator instance
        decorator = unreal.new_object(decorator_class)

        # Set the RequiredState property
        # ESLFAIStates is a uint8 enum
        decorator.set_editor_property("required_state", state_value)
        decorator.set_editor_property("state_key_name", "State")
        decorator.set_editor_property("b_enable_logging", True)

        print(f"Created BTD_StateEquals decorator with RequiredState={state_value}")
        return decorator
    except Exception as e:
        print(f"Error creating decorator: {e}")
        return None


def add_decorator_to_bt(bt_path, state_value, decorator_class):
    """Add BTD_StateEquals decorator to a behavior tree's root"""
    bt = unreal.load_asset(bt_path)
    if not bt:
        print(f"FAILED to load: {bt_path}")
        return False

    root = bt.get_editor_property("root_node")
    if not root:
        print(f"No root node in {bt_path}")
        return False

    # Get children (branches)
    try:
        children = root.get_editor_property("children")
        if not children or len(children) == 0:
            print(f"No children in root of {bt_path}")
            return False

        # Get first child's decorators
        first_child = children[0]
        existing_decorators = list(first_child.get_editor_property("decorators") or [])

        # Check if BTD_StateEquals already exists
        for dec in existing_decorators:
            if "BTD_StateEquals" in dec.get_class().get_name():
                print(f"BTD_StateEquals already exists in {bt_path}")
                return True

        # Create new decorator
        new_decorator = create_state_decorator(state_value, decorator_class)
        if not new_decorator:
            return False

        # Add to decorators list
        existing_decorators.insert(0, new_decorator)  # Insert at beginning
        first_child.set_editor_property("decorators", existing_decorators)

        # Save the asset
        unreal.EditorAssetLibrary.save_asset(bt_path)
        print(f"SUCCESS: Added BTD_StateEquals to {bt_path}")
        return True

    except Exception as e:
        print(f"Error modifying {bt_path}: {e}")
        return False


def main():
    print("=" * 60)
    print("UPDATE BT DECORATORS - AAA Approach")
    print("Using C++ BTD_StateEquals decorator")
    print("=" * 60)

    # First, diagnose current state
    diagnose_all_bts()

    # Check if C++ decorator class is available
    decorator_class = check_cpp_decorator_class()
    if not decorator_class:
        print("\nERROR: BTD_StateEquals C++ class not available!")
        print("Make sure the project is compiled with BTD_StateEquals.h/cpp")
        return

    print("\n" + "-" * 60)
    print("UPDATING BT SUBTREES")
    print("-" * 60)

    success_count = 0
    fail_count = 0

    for bt_path, state_value in BT_STATE_MAP.items():
        if add_decorator_to_bt(bt_path, state_value, decorator_class):
            success_count += 1
        else:
            fail_count += 1

    print("\n" + "=" * 60)
    print(f"RESULTS: {success_count} success, {fail_count} failed")
    print("=" * 60)


if __name__ == "__main__":
    main()
