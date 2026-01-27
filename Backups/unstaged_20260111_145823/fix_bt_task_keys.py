"""
Fix BT Task BlackboardKeySelector properties that are empty after reparenting
Sets the correct SelectedKeyName for each task type
"""
import unreal

def log(msg):
    unreal.log_warning(f"[FIX_BT_KEYS] {msg}")

# Task -> Key property -> Blackboard key name mapping
TASK_KEY_FIXES = {
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SimpleMoveTo": {
        "target_key": "Target",
        "radius_key": "AcceptableRadius",  # May need adjustment
    },
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_ToggleFocus": {
        "target_key": "Target",
    },
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetStrafePointAroundTarget": {
        "target_key": "Target",
        "destination_key": "Destination",
    },
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetRandomPoint": {
        "destination_key": "Destination",
    },
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetRandomPointNearStartPosition": {
        "destination_key": "Destination",
    },
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetCurrentLocation": {
        "key": "StartLocation",
    },
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_PatrolPath": {
        "destination_key": "Destination",
        "patrol_index_key": "PatrolIndex",
    },
    # BTT_SetKey and BTT_ClearKey use per-instance keys set in BT, not defaults
}

def fix_blackboard_key_selector(cdo, prop_name, key_name):
    """Try to fix a BlackboardKeySelector property"""
    try:
        key_selector = cdo.get_editor_property(prop_name)
        if key_selector is None:
            log(f"    [WARN] {prop_name} is None")
            return False

        # Try to set selected_key_name
        try:
            key_selector.set_editor_property('selected_key_name', key_name)
            log(f"    [OK] Set {prop_name}.selected_key_name = '{key_name}'")
            return True
        except Exception as e:
            log(f"    [ERROR] Could not set selected_key_name: {e}")
            return False

    except Exception as e:
        log(f"    [ERROR] Could not get {prop_name}: {e}")
        return False

def main():
    log("=" * 70)
    log("FIXING BT TASK BLACKBOARD KEY SELECTORS")
    log("=" * 70)

    fixed_count = 0
    failed_count = 0

    for task_path, key_fixes in TASK_KEY_FIXES.items():
        log("")
        task_name = task_path.split('/')[-1]
        log(f"Fixing: {task_name}")

        bp = unreal.load_asset(task_path)
        if not bp:
            log(f"  [ERROR] Could not load Blueprint")
            failed_count += 1
            continue

        try:
            gen_class = bp.generated_class()
            if not gen_class:
                log(f"  [ERROR] No generated class")
                failed_count += 1
                continue

            cdo = unreal.get_default_object(gen_class)
            if not cdo:
                log(f"  [ERROR] No CDO")
                failed_count += 1
                continue

            log(f"  CDO class: {cdo.get_class().get_name()}")

            task_fixed = True
            for prop_name, key_name in key_fixes.items():
                if not fix_blackboard_key_selector(cdo, prop_name, key_name):
                    task_fixed = False

            if task_fixed:
                # Save the asset
                try:
                    unreal.EditorAssetLibrary.save_asset(task_path, only_if_is_dirty=False)
                    log(f"  [SAVED] {task_name}")
                    fixed_count += 1
                except Exception as e:
                    log(f"  [ERROR] Could not save: {e}")
                    failed_count += 1
            else:
                failed_count += 1

        except Exception as e:
            log(f"  [ERROR] {e}")
            failed_count += 1

    log("")
    log("=" * 70)
    log(f"SUMMARY: Fixed {fixed_count}, Failed {failed_count}")
    log("=" * 70)

    # Verify the fixes
    log("")
    log("=== VERIFICATION ===")
    for task_path, key_fixes in TASK_KEY_FIXES.items():
        task_name = task_path.split('/')[-1]
        bp = unreal.load_asset(task_path)
        if bp:
            try:
                gen_class = bp.generated_class()
                if gen_class:
                    cdo = unreal.get_default_object(gen_class)
                    if cdo:
                        log(f"{task_name}:")
                        for prop_name, expected_key in key_fixes.items():
                            try:
                                key_selector = cdo.get_editor_property(prop_name)
                                if key_selector:
                                    actual_key = key_selector.get_editor_property('selected_key_name')
                                    status = "[OK]" if str(actual_key) == expected_key else "[FAIL]"
                                    log(f"  {status} {prop_name}: {actual_key}")
                            except Exception as e:
                                log(f"  [ERROR] {prop_name}: {e}")
            except:
                pass

if __name__ == "__main__":
    main()
