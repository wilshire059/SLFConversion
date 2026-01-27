"""
Check BT decorator IntValues WITHOUT fixing them
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_CHECK] {msg}")

def main():
    log("=" * 70)
    log("CHECKING BT DECORATOR INTVALUES (READ-ONLY)")
    log("=" * 70)

    bt_paths = [
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat", "Combat", 4),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle", "Idle", 0),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Investigating", "Investigating", 3),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PatrolPath", "Patrolling", 2),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam", "RandomRoam", 1),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PoiseBroken", "PoiseBroken", 5),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Uninterruptable", "Uninterruptable", 6),
        ("/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_OutOfBounds", "OutOfBounds", 7),
    ]

    for bt_path, expected_state, expected_value in bt_paths:
        log("")
        log(f"Checking: {bt_path.split('/')[-1]}")
        result = unreal.SLFAutomationLibrary.export_behavior_tree_structure(bt_path)
        
        # Find IntValue in the result
        for line in result.split('\n'):
            if 'State' in line and 'Equal' in line:
                log(f"  {line.strip()}")
            if 'IntValue' in line:
                log(f"  {line.strip()}")
                # Check if it matches expected
                if f"IntValue: {expected_value}" in line:
                    log(f"  [OK] Matches expected {expected_state}={expected_value}")
                else:
                    log(f"  [WARN] Expected {expected_state}={expected_value}")

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
