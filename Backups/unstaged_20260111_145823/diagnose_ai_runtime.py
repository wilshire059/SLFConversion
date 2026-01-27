"""
Diagnose AI Runtime - Check behavior tree, blackboard, and decorator configuration
"""
import unreal

def log(msg):
    unreal.log_warning(f"[AI_DIAG] {msg}")

def main():
    log("=" * 70)
    log("AI RUNTIME DIAGNOSTIC")
    log("=" * 70)

    # Load blackboard
    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"
    bb = unreal.load_asset(bb_path)

    if bb:
        log(f"Blackboard: {bb.get_name()}")
        keys = bb.get_editor_property("Keys")
        log(f"Keys ({len(keys)}):")
        for i, key in enumerate(keys):
            name = key.get_editor_property("EntryName")
            key_type = key.get_editor_property("KeyType")
            type_name = key_type.get_class().get_name() if key_type else "None"

            # For enum keys, try to get the enum type
            if type_name == "BlackboardKeyType_Enum":
                try:
                    enum_type = key_type.get_editor_property("EnumType")
                    if enum_type:
                        log(f"  [{i}] {name}: {type_name} -> {enum_type.get_name()}")
                    else:
                        log(f"  [{i}] {name}: {type_name} -> (no enum type set)")
                except:
                    log(f"  [{i}] {name}: {type_name}")
            else:
                log(f"  [{i}] {name}: {type_name}")
    else:
        log(f"[ERROR] Could not load blackboard: {bb_path}")

    log("")

    # Load behavior tree
    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"
    bt = unreal.load_asset(bt_path)

    if bt:
        log(f"Behavior Tree: {bt.get_name()}")

        # Check blackboard asset
        bb_asset = bt.get_editor_property("BlackboardAsset")
        if bb_asset:
            log(f"  Uses Blackboard: {bb_asset.get_name()}")

        # Check root node
        root = bt.get_editor_property("RootNode")
        if root:
            log(f"  Root Node: {root.get_class().get_name()}")

            # Try to get child nodes
            try:
                child = root.get_editor_property("ChildNode")
                if child:
                    log(f"    Child: {child.get_class().get_name()}")

                    # Check for children array (if composite)
                    try:
                        children = child.get_editor_property("Children")
                        if children:
                            log(f"    Children ({len(children)}):")
                            for i, c in enumerate(children[:5]):  # First 5
                                child_node = c.get_editor_property("ChildComposite") if hasattr(c, "get_editor_property") else None
                                if not child_node:
                                    child_node = c.get_editor_property("ChildTask") if hasattr(c, "get_editor_property") else None
                                if child_node:
                                    log(f"      [{i}] {child_node.get_class().get_name()}")
                                else:
                                    log(f"      [{i}] {c}")
                    except Exception as e:
                        log(f"    Error getting children: {e}")
            except Exception as e:
                log(f"    Error getting child: {e}")
    else:
        log(f"[ERROR] Could not load behavior tree: {bt_path}")

    log("")

    # Load combat subtree
    bt_combat_path = "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat"
    bt_combat = unreal.load_asset(bt_combat_path)

    if bt_combat:
        log(f"Combat Subtree: {bt_combat.get_name()}")

        # Check root decorators
        try:
            root_decorators = bt_combat.get_editor_property("RootDecorators")
            if root_decorators:
                log(f"  Root Decorators ({len(root_decorators)}):")
                for i, dec in enumerate(root_decorators):
                    log(f"    [{i}] {dec.get_class().get_name()}")

                    # Try to get decorator key
                    try:
                        bb_key = dec.get_editor_property("BlackboardKey")
                        if bb_key:
                            key_name = bb_key.get_editor_property("SelectedKeyName")
                            log(f"        Key: {key_name}")
                    except Exception as e:
                        log(f"        Key error: {e}")

                    # Try to get condition type
                    try:
                        basic_op = dec.get_editor_property("BasicOperation")
                        log(f"        Operation: {basic_op}")
                    except:
                        pass

                    try:
                        int_val = dec.get_editor_property("IntValue")
                        log(f"        IntValue: {int_val}")
                    except:
                        pass
            else:
                log("  No root decorators")
        except Exception as e:
            log(f"  Error getting root decorators: {e}")
    else:
        log(f"[ERROR] Could not load combat subtree: {bt_combat_path}")

    log("")
    log("=" * 70)
    log("CHECKING ENUM VALUES")
    log("=" * 70)

    # Check ESLFAIStates enum values
    log("ESLFAIStates C++ enum values:")
    log("  Idle = 0")
    log("  RandomRoam = 1")
    log("  Patrolling = 2")
    log("  Investigating = 3")
    log("  Combat = 4")
    log("  PoiseBroken = 5")
    log("  Uninterruptable = 6")
    log("  OutOfBounds = 7")

if __name__ == "__main__":
    main()
