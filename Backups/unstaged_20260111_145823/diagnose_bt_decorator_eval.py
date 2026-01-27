"""
Diagnose why BT_Combat decorator is failing despite State=4 and Target set
Check if the decorator IntValue and blackboard enum match
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/dec_eval_output.txt"
output_lines = []

def log(msg):
    unreal.log_warning(f"[DEC_EVAL] {msg}")
    output_lines.append(msg)

def main():
    log("=" * 70)
    log("DIAGNOSING BT_COMBAT DECORATOR EVALUATION")
    log("=" * 70)

    # Load BT_Combat
    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat"
    bt = unreal.load_asset(bt_path)
    if not bt:
        log("[ERROR] Could not load BT_Combat")
        return

    log(f"Loaded: {bt.get_name()}")

    # Get blackboard
    bb_asset = bt.get_editor_property("blackboard_asset")
    if bb_asset:
        log(f"BlackboardAsset: {bb_asset.get_name()}")

        # Get blackboard keys
        log("")
        log("=== BLACKBOARD KEYS ===")
        try:
            keys = bb_asset.get_editor_property("keys")
            for key in keys:
                key_name = key.get_editor_property("entry_name")
                key_type = key.get_class().get_name()
                log(f"  {key_name}: {key_type}")

                # If this is the State key, check its settings
                if str(key_name) == "State":
                    log(f"    (Checking State key details)")
                    try:
                        # Check if it's enum type
                        if hasattr(key, "enum_type"):
                            enum_type = key.get_editor_property("enum_type")
                            log(f"    EnumType: {enum_type}")
                        if hasattr(key, "enum_name"):
                            enum_name = key.get_editor_property("enum_name")
                            log(f"    EnumName: {enum_name}")
                    except Exception as e:
                        log(f"    Error getting enum info: {e}")
        except Exception as e:
            log(f"[ERROR] {e}")

    # Check root decorators
    log("")
    log("=== BT_COMBAT ROOT DECORATORS ===")
    root = bt.get_editor_property("root_node")
    if not root:
        log("[ERROR] No root node")
        return

    decorators = root.get_editor_property("decorators")
    if decorators:
        for i, dec in enumerate(decorators):
            class_name = dec.get_class().get_name()
            node_name = dec.get_editor_property("node_name")
            log(f"  [{i}] {class_name} - {node_name}")

            if "Blackboard" in class_name:
                try:
                    # Get key info
                    key = dec.get_editor_property("blackboard_key")
                    key_name = key.get_editor_property("selected_key_name")
                    log(f"      Key: {key_name}")

                    # Get comparison value
                    int_val = dec.get_editor_property("int_value")
                    log(f"      IntValue: {int_val}")

                    # Get notify observer (how decorator evaluates)
                    try:
                        notify = dec.get_editor_property("notify_observer")
                        log(f"      NotifyObserver: {notify}")
                    except:
                        pass

                    # Get flow abort mode
                    try:
                        flow = dec.get_editor_property("flow_abort_mode")
                        log(f"      FlowAbortMode: {flow}")
                    except:
                        pass

                    # Get key type
                    try:
                        key_type = dec.get_editor_property("key_type")
                        log(f"      KeyType: {key_type}")
                    except:
                        pass

                    # Get basic operation
                    try:
                        basic_op = dec.get_editor_property("basic_operation")
                        log(f"      BasicOperation: {basic_op}")
                    except:
                        pass

                    # Get operation
                    try:
                        operation = dec.get_editor_property("operation_mode")
                        log(f"      OperationMode: {operation}")
                    except:
                        pass

                except Exception as e:
                    log(f"      [ERROR] {e}")

    # Check BTTask_RunBehavior in BT_Enemy that calls BT_Combat
    log("")
    log("=== CHECKING BT_ENEMY SUBTREE CALL TO BT_COMBAT ===")

    bt_enemy_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"
    bt_enemy = unreal.load_asset(bt_enemy_path)
    if not bt_enemy:
        log("[ERROR] Could not load BT_Enemy")
        return

    root_enemy = bt_enemy.get_editor_property("root_node")
    if root_enemy:
        child = root_enemy.get_editor_property("child")
        if child:
            # This should be the Selector with RunBehavior tasks
            children = child.get_editor_property("children")
            if children:
                for i, child_data in enumerate(children):
                    # Get the actual task/composite
                    task = None
                    try:
                        task = child_data.get_editor_property("child_task")
                    except:
                        pass
                    if not task:
                        try:
                            task = child_data.get_editor_property("child_composite")
                        except:
                            pass

                    if task:
                        task_class = task.get_class().get_name()
                        task_name = task.get_editor_property("node_name")

                        if "Combat" in str(task_name) or i == 3:  # Combat is 4th child (index 3)
                            log(f"  [{i}] {task_class} - {task_name}")

                            # Check if it's RunBehavior
                            if "RunBehavior" in task_class:
                                try:
                                    subtree = task.get_editor_property("behavior_asset")
                                    log(f"      SubtreeAsset: {subtree.get_name() if subtree else 'NULL'}")
                                except:
                                    pass

                            # Check decorators on this node
                            try:
                                task_decs = task.get_editor_property("decorators")
                                if task_decs:
                                    log(f"      Decorators on RunBehavior node: {len(task_decs)}")
                                    for dec in task_decs:
                                        log(f"        - {dec.get_class().get_name()}")
                                else:
                                    log(f"      No decorators on RunBehavior node")
                            except:
                                log(f"      (no decorator property)")

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
    # Write output to file
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(output_lines))
    unreal.log_warning(f"[DEC_EVAL] Output written to {OUTPUT_FILE}")
