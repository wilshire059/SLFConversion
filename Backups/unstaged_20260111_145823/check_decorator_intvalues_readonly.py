"""
Read-only diagnostic of BTDecorator_Blackboard IntValues
Does NOT modify anything - just reports current state
"""
import unreal

def log(msg):
    unreal.log_warning(f"[DEC_CHECK] {msg}")

# Expected IntValue for each subtree's State decorator
EXPECTED_STATE_VALUES = {
    "BT_Combat": 4,      # Combat
    "BT_Idle": 0,        # Idle
    "BT_RandomRoam": 1,  # RandomRoam
    "BT_Investigating": 3,  # Investigating
    "BT_PatrolPath": 2,  # Patrolling
    "BT_PoiseBroken": 5, # PoiseBroken
    "BT_Uninterruptable": 6,  # Uninterruptable
    "BT_OutOfBounds": 7, # OutOfBounds
}

def main():
    log("=" * 70)
    log("CHECKING BT DECORATOR INTVALUES (READ-ONLY)")
    log("=" * 70)

    subtrees = [
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Investigating",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PatrolPath",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PoiseBroken",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Uninterruptable",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_OutOfBounds",
    ]

    issues = []

    for bt_path in subtrees:
        bt_name = bt_path.split('/')[-1]
        log("")
        log(f"=== {bt_name} ===")

        # Use export_behavior_tree_structure to get decorator info
        result = unreal.SLFAutomationLibrary.export_behavior_tree_structure(bt_path)
        if not result:
            log(f"  [ERROR] Could not export structure")
            continue

        # Parse the result for decorator info
        lines = result.split('\n')
        for line in lines:
            # Look for root decorator descriptions
            if "BTDecorator_Blackboard" in line:
                log(f"  {line.strip()}")
            elif "State Is Equal To" in line or "State is Is Set" in line or "State" in line:
                log(f"  {line.strip()}")
                # Check if this has the wrong value
                if bt_name in EXPECTED_STATE_VALUES:
                    expected = EXPECTED_STATE_VALUES[bt_name]
                    # The description should contain the state name
                    if "Combat" in line and expected != 4:
                        issues.append(f"{bt_name}: Shows 'Combat' but should be state {expected}")
                    elif "Idle" in line and expected != 0:
                        issues.append(f"{bt_name}: Shows 'Idle' but should be state {expected}")
            elif "BlackboardKey: State" in line:
                log(f"  {line.strip()}")

    log("")
    log("=" * 70)
    log("SUMMARY")
    log("=" * 70)

    if issues:
        log("ISSUES FOUND:")
        for issue in issues:
            log(f"  [WARN] {issue}")
    else:
        log("[OK] No obvious state mismatch issues detected")
        log("(Note: This only checks description text, not actual IntValue)")

    log("")
    log("Expected State values per subtree:")
    for name, val in EXPECTED_STATE_VALUES.items():
        log(f"  {name}: IntValue should be {val}")

if __name__ == "__main__":
    main()
