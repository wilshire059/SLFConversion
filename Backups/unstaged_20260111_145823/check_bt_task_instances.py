"""
Check BT task instances in behavior trees to see their key configurations
The key selectors are set per-instance in the BT, not in task defaults
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_INSTANCES] {msg}")

def main():
    log("=" * 70)
    log("CHECKING BT TASK INSTANCES IN BEHAVIOR TREES")
    log("=" * 70)

    # Export BT_Combat structure to see task instances
    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat"

    log(f"Exporting: {bt_path}")

    result = unreal.SLFAutomationLibrary.export_behavior_tree_structure(bt_path)
    if result:
        # Print first 5000 chars to see task configurations
        for i, line in enumerate(result.split('\n')[:200]):
            log(line)
    else:
        log("[ERROR] No result from export_behavior_tree_structure")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
