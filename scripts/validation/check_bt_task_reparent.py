# check_bt_task_reparent.py
# Check if Blueprint BT tasks are reparented to C++ classes

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/bt_task_reparent_check.txt"

BT_TASKS = [
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_ClearKey",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetCurrentLocation",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetRandomPoint",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetRandomPointNearStartPosition",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetStrafePointAroundTarget",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_PatrolPath",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SetKey",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SetMovementMode",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SimpleMoveTo",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SwitchState",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SwitchToPreviousState",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_ToggleFocus",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_TryExecuteAbility",
]

def log(msg):
    print(msg)
    with open(OUTPUT_FILE, "a") as f:
        f.write(msg + "\n")

def check_reparent():
    with open(OUTPUT_FILE, "w") as f:
        f.write("=== Blueprint BT Task Reparent Check ===\n\n")

    cpp_class_count = 0
    blueprint_base_count = 0

    for bp_path in BT_TASKS:
        try:
            bp = unreal.load_asset(bp_path)
            if not bp:
                log(f"{bp_path}: NOT FOUND")
                continue

            bp_name = bp.get_name()

            # Use SLFAutomationLibrary to get parent class
            parent_path = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
            if not parent_path:
                log(f"{bp_name}: No parent class found")
                continue

            # Check if parent is a C++ class (from /Script/ path) vs Blueprint
            is_cpp_parent = "/Script/" in parent_path

            status = "C++" if is_cpp_parent else "BLUEPRINT"

            if is_cpp_parent:
                cpp_class_count += 1
            else:
                blueprint_base_count += 1

            log(f"{bp_name}:")
            log(f"  Parent: {parent_path}")
            log(f"  Status: {status}")
            log("")

        except Exception as e:
            log(f"{bp_path}: Error - {e}")
            import traceback
            log(traceback.format_exc())

    log("=" * 60)
    log(f"SUMMARY:")
    log(f"  Reparented to C++: {cpp_class_count}")
    log(f"  Still Blueprint base: {blueprint_base_count}")
    log("")

    if cpp_class_count > 0 and cpp_class_count == len(BT_TASKS):
        log("ALL Blueprint BT tasks reparented to C++!")
        log("C++ ExecuteTask should be called - check for logging in those functions.")
    elif cpp_class_count > 0:
        log("PARTIAL: Some tasks reparented, some still Blueprint base.")
    else:
        log("WARNING: Blueprint tasks NOT reparented to C++!")
        log("Blueprint EventGraphs may have been cleared, leaving tasks empty!")

if __name__ == "__main__":
    check_reparent()
