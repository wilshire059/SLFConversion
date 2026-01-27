"""
Extract BlackboardKeySelector key names from backup BT task Blueprints
Run this on bp_only project to get original key configurations
"""
import unreal

def log(msg):
    unreal.log_warning(f"[EXTRACT_BT_KEYS] {msg}")

TASKS_TO_CHECK = [
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SimpleMoveTo",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_ToggleFocus",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SetKey",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_ClearKey",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetStrafePointAroundTarget",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetRandomPoint",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetRandomPointNearStartPosition",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetCurrentLocation",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_PatrolPath",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SwitchState",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SwitchToPreviousState",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SetMovementMode",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_TryExecuteAbility",
]

KEY_PROPERTIES = [
    "target_key",
    "radius_key",
    "key",
    "destination_key",
    "patrol_index_key",
    "new_state",
    "movement_mode",
]

def main():
    log("=" * 70)
    log("EXTRACTING BT TASK KEY CONFIGURATIONS FROM BACKUP")
    log("=" * 70)
    log("")

    results = {}

    for task_path in TASKS_TO_CHECK:
        task_name = task_path.split('/')[-1]
        log(f"Checking: {task_name}")

        bp = unreal.load_asset(task_path)
        if not bp:
            log(f"  [ERROR] Could not load")
            continue

        try:
            gen_class = bp.generated_class()
            if not gen_class:
                log(f"  [ERROR] No generated class")
                continue

            cdo = unreal.get_default_object(gen_class)
            if not cdo:
                log(f"  [ERROR] No CDO")
                continue

            task_keys = {}
            for prop_name in KEY_PROPERTIES:
                try:
                    val = cdo.get_editor_property(prop_name)
                    if val is not None:
                        # Check if it's a BlackboardKeySelector
                        try:
                            key_name = val.get_editor_property('selected_key_name')
                            if key_name and str(key_name) != "None" and str(key_name) != "":
                                task_keys[prop_name] = str(key_name)
                                log(f"  {prop_name} -> '{key_name}'")
                            elif str(val).find("BlackboardKeySelector") >= 0:
                                log(f"  {prop_name} -> [EMPTY]")
                        except:
                            # Not a BlackboardKeySelector, maybe enum or other value
                            log(f"  {prop_name} = {val}")
                            task_keys[prop_name] = str(val)
                except:
                    pass

            if task_keys:
                results[task_name] = task_keys
            else:
                log(f"  [No key properties found]")

        except Exception as e:
            log(f"  [ERROR] {e}")

        log("")

    log("=" * 70)
    log("SUMMARY - Copy this to fix_bt_task_keys.py:")
    log("=" * 70)
    log("")
    log("TASK_KEY_FIXES = {")
    for task_name, keys in results.items():
        task_path = f"/Game/SoulslikeFramework/Blueprints/_AI/Tasks/{task_name}"
        log(f'    "{task_path}": {{')
        for prop, val in keys.items():
            log(f'        "{prop}": "{val}",')
        log("    },")
    log("}")
    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
