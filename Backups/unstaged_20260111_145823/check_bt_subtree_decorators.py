"""
Check BT subtree decorator internal configuration via export_behavior_tree_structure
This includes full tree structure with IntValue details
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_DEC] {msg}")

def main():
    log("=" * 70)
    log("CHECKING BT SUBTREE DECORATOR CONFIGURATION")
    log("=" * 70)

    subtrees = [
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat", "Combat", 4),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle", "Idle", 0),
    ]

    for bt_path, expected_state, expected_value in subtrees:
        log("")
        log("=" * 60)
        log(f"Subtree: {bt_path.split('/')[-1]}")
        log(f"Expected: State == {expected_state} (IntValue={expected_value})")
        log("=" * 60)

        # Use export_behavior_tree_structure which gives full details
        result = unreal.SLFAutomationLibrary.export_behavior_tree_structure(bt_path)

        if result:
            for line in result.split('\n'):
                log(line)
        else:
            log(f"[ERROR] Could not export structure for {bt_path}")

    # Also check main BT_Enemy to understand BTTask_RunBehavior execution
    log("")
    log("=" * 60)
    log("MAIN TREE: BT_Enemy")
    log("=" * 60)

    result = unreal.SLFAutomationLibrary.export_behavior_tree_structure("/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy")
    if result:
        for line in result.split('\n'):
            log(line)

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
