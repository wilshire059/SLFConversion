"""
Migrate BT decorators from Blueprint E_AI_States enum to C++ ESLFAIStates int values.

This script:
1. Loads each BT subtree
2. Finds decorators checking "State" blackboard key
3. Sets the correct IntValue based on subtree name
4. Saves the asset

C++ ESLFAIStates mapping:
  Idle = 0
  RandomRoam = 1
  Patrolling = 2
  Investigating = 3
  Combat = 4
  PoiseBroken = 5
  Uninterruptable = 6
  OutOfBounds = 7
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/bt_enum_migration.txt"

# Mapping: BT subtree name -> expected State IntValue
BT_STATE_MAP = {
    "BT_Idle": 0,
    "BT_RandomRoam": 1,
    "BT_PatrolPath": 2,  # Patrolling
    "BT_Investigating": 3,
    "BT_Combat": 4,
    "BT_PoiseBroken": 5,
    "BT_Uninterruptable": 6,
    "BT_OutOfBounds": 7,
}

output_lines = []

def log(msg):
    unreal.log_warning(f"[BT_ENUM_MIG] {msg}")
    output_lines.append(msg)

def fix_bt_decorator_intvalues(bt_path, expected_state):
    """
    Fix State decorator IntValue using C++ automation library.
    Returns True if fixes were made.
    """
    result = unreal.SLFAutomationLibrary.fix_bt_state_decorator_to_value(bt_path, expected_state)
    for line in result.split('\n'):
        if line.strip():
            log(f"  {line}")
    return "FIXED" in result or "OK" in result

def main():
    log("=" * 70)
    log("MIGRATING BT DECORATORS FROM BLUEPRINT ENUM TO C++ INT VALUES")
    log("=" * 70)
    log("")
    log("ESLFAIStates mapping:")
    log("  Idle=0, RandomRoam=1, Patrolling=2, Investigating=3")
    log("  Combat=4, PoiseBroken=5, Uninterruptable=6, OutOfBounds=7")
    log("")

    success_count = 0
    fail_count = 0

    for bt_name, expected_state in BT_STATE_MAP.items():
        bt_path = f"/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/{bt_name}"
        log(f"Processing: {bt_name} (expected State={expected_state})")

        try:
            if fix_bt_decorator_intvalues(bt_path, expected_state):
                success_count += 1
            else:
                fail_count += 1
        except Exception as e:
            log(f"  ERROR: {e}")
            fail_count += 1

        log("")

    log("=" * 70)
    log(f"SUMMARY: {success_count} succeeded, {fail_count} failed")
    log("=" * 70)

    # Write output
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(output_lines))
    log(f"Output written to: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
