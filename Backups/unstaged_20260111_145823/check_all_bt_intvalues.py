"""
Check IntValue for ALL behavior tree decorators that use State key
Show actual IntValue compared to expected C++ enum values
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/bt_intvalues.txt"
output_lines = []

def log(msg):
    unreal.log_warning(f"[BT_INT] {msg}")
    output_lines.append(msg)

# C++ ESLFAIStates enum values
EXPECTED_VALUES = {
    "BT_Idle": 0,           # Idle
    "BT_RandomRoam": 1,     # RandomRoam
    "BT_PatrolPath": 2,     # Patrolling
    "BT_Investigating": 3,  # Investigating
    "BT_Combat": 4,         # Combat
    "BT_PoiseBroken": 5,    # PoiseBroken
    "BT_Uninterruptable": 6,# Uninterruptable
    "BT_OutOfBounds": 7,    # OutOfBounds
}

def check_decorator_intvalue(dec, bt_name):
    try:
        class_name = dec.get_class().get_name()
        if "Blackboard" not in class_name:
            return None

        key = dec.get_editor_property("blackboard_key")
        key_name = str(key.get_editor_property("selected_key_name"))

        if key_name != "State":
            return None

        int_val = dec.get_editor_property("int_value")
        expected = EXPECTED_VALUES.get(bt_name, -1)

        status = "OK" if int_val == expected else "MISMATCH"
        return {
            "key": key_name,
            "int_value": int_val,
            "expected": expected,
            "status": status
        }
    except Exception as e:
        return {"error": str(e)}

def main():
    log("=" * 70)
    log("CHECKING ALL BT STATE DECORATOR INTVALUES")
    log("=" * 70)
    log("")
    log("C++ ESLFAIStates enum values:")
    for name, val in EXPECTED_VALUES.items():
        log(f"  {name}: {val}")
    log("")

    subtree_paths = [
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

    for bt_path in subtree_paths:
        bt_name = bt_path.split('/')[-1]
        log(f"=== {bt_name} ===")

        bt = unreal.load_asset(bt_path)
        if not bt:
            log(f"  [ERROR] Could not load")
            continue

        root = bt.get_editor_property("root_node")
        if not root:
            log(f"  [ERROR] No root node")
            continue

        decorators = root.get_editor_property("decorators")
        if not decorators:
            log(f"  [WARN] No root decorators")
            continue

        for i, dec in enumerate(decorators):
            result = check_decorator_intvalue(dec, bt_name)
            if result:
                if "error" in result:
                    log(f"  [{i}] Error: {result['error']}")
                else:
                    status = result["status"]
                    int_val = result["int_value"]
                    expected = result["expected"]
                    log(f"  [{i}] State decorator: IntValue={int_val}, Expected={expected} [{status}]")

                    if status == "MISMATCH":
                        issues.append(f"{bt_name}: IntValue={int_val}, should be {expected}")

    log("")
    log("=" * 70)
    log("SUMMARY")
    log("=" * 70)

    if issues:
        log(f"[ISSUES FOUND] {len(issues)} decorators have wrong IntValue:")
        for issue in issues:
            log(f"  - {issue}")
    else:
        log("[OK] All decorators have correct IntValue")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(output_lines))
    unreal.log_warning(f"[BT_INT] Output written to {OUTPUT_FILE}")
