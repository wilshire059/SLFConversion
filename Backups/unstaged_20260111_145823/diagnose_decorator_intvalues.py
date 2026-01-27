"""
Diagnose BTDecorator_Blackboard IntValues to ensure enum comparisons work correctly
"""
import unreal

def log(msg):
    unreal.log_warning(f"[DEC_INT] {msg}")

def main():
    log("=" * 70)
    log("DIAGNOSING BT DECORATOR INTVALUES")
    log("=" * 70)

    # Check all subtrees
    subtrees = [
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat", "Combat", 4),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle", "Idle", 0),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam", "RandomRoam", 1),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Investigating", "Investigating", 3),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PatrolPath", "Patrolling", 2),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PoiseBroken", "PoiseBroken", 5),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Uninterruptable", "Uninterruptable", 6),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_OutOfBounds", "OutOfBounds", 7),
    ]

    for bt_path, expected_state, expected_int in subtrees:
        log("")
        log(f"=== {bt_path.split('/')[-1]} (expects State={expected_int} {expected_state}) ===")

        result = unreal.SLFAutomationLibrary.diagnose_and_fix_bt_decorator_int_values(bt_path)
        if result:
            for line in result.split('\n')[:30]:  # Limit output
                if line.strip():
                    log(line)

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
