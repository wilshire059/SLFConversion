"""
Diagnose BT task BlackboardKeySelector configurations
Uses C++ DiagnoseBTTaskKeySelectors function to get actual key values from task instances
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_KEYS] {msg}")

def main():
    log("=" * 70)
    log("DIAGNOSING BT TASK KEY SELECTORS")
    log("=" * 70)

    subtrees = [
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PatrolPath",
    ]

    for bt_path in subtrees:
        log("")
        log("=" * 70)
        log(f"SUBTREE: {bt_path.split('/')[-1]}")
        log("=" * 70)

        result = unreal.SLFAutomationLibrary.diagnose_bt_task_key_selectors(bt_path)
        if result:
            for line in result.split('\n'):
                log(line)
        else:
            log(f"[ERROR] No result for {bt_path}")

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
