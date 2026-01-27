"""
Check BT_Enemy main tree structure
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_ENEMY] {msg}")

def main():
    log("=" * 70)
    log("CHECKING BT_Enemy MAIN TREE")
    log("=" * 70)

    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"
    result = unreal.SLFAutomationLibrary.export_behavior_tree_structure(bt_path)

    if result:
        log("")
        for line in result.split('\n'):
            log(line)
    else:
        log("[ERROR] Could not export BT_Enemy structure")

    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
