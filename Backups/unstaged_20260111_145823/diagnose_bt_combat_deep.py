"""
Deep diagnosis of BT_Combat structure to find why tasks aren't executing
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_COMBAT] {msg}")

def main():
    log("=" * 70)
    log("DEEP DIAGNOSIS OF BT_COMBAT STRUCTURE")
    log("=" * 70)

    # Get detailed structure of BT_Combat
    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat"

    result = unreal.SLFAutomationLibrary.export_behavior_tree_structure(bt_path)
    if result:
        for line in result.split('\n'):
            log(line)

    log("")
    log("=" * 70)
    log("CHECKING COMPOSITE NODE CHILDREN")
    log("=" * 70)

    # Use diagnostic function that shows more detail
    result2 = unreal.SLFAutomationLibrary.diagnose_bt_task_key_selectors(bt_path)
    if result2:
        for line in result2.split('\n'):
            log(line)

    log("")
    log("=" * 70)
    log("EXPORTING ALL SUBTREE STRUCTURES")
    log("=" * 70)

    # Export all subtrees to see full picture
    result3 = unreal.SLFAutomationLibrary.export_all_bt_subtrees()
    log("Exported to migration_cache/bt_structure_export.txt")

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
