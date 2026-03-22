# check_bt_task_parents.py
# Check which C++ class the Blueprint BT tasks are parented to

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/bt_task_parents.txt"

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

def check_parents():
    with open(OUTPUT_FILE, "w") as f:
        f.write("=== Blueprint BT Task Parents ===\n\n")

    for bp_path in BT_TASKS:
        try:
            bp = unreal.load_asset(bp_path)
            if not bp:
                log(f"{bp_path}: NOT FOUND")
                continue

            bp_name = bp.get_name()

            # Try to get parent class via BlueprintEditorLibrary
            try:
                parent_class = unreal.BlueprintEditorLibrary.get_blueprint_parent_class(bp)
                parent_name = parent_class.get_name() if parent_class else "None"
            except:
                parent_name = "Could not get"

            log(f"{bp_name}: Parent = {parent_name}")

        except Exception as e:
            log(f"{bp_path}: Error - {e}")

    log("\nDone!")

if __name__ == "__main__":
    check_parents()
