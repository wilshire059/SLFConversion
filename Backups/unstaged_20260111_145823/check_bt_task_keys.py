"""
Check BT Task blackboard key configurations
Verify that TargetKey, RadiusKey etc. are properly set after migration
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_TASK] {msg}")

def main():
    log("=" * 70)
    log("CHECKING BT TASK BLACKBOARD KEY CONFIGURATIONS")
    log("=" * 70)

    # Load BTT_SimpleMoveTo Blueprint to check its default key values
    btt_path = "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SimpleMoveTo"

    log(f"Loading: {btt_path}")

    bp = unreal.load_asset(btt_path)
    if not bp:
        log(f"[ERROR] Could not load Blueprint")
        return

    log(f"Blueprint class: {bp.get_class().get_name()}")

    # Get the generated class
    try:
        gen_class = bp.generated_class()
        if gen_class:
            log(f"Generated class: {gen_class.get_name()}")

            # Get CDO
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                log(f"CDO class: {cdo.get_class().get_name()}")

                # Try to get TargetKey property
                try:
                    target_key = cdo.get_editor_property('target_key')
                    log(f"TargetKey: {target_key}")
                    if target_key:
                        try:
                            selected_key_name = target_key.get_editor_property('selected_key_name')
                            log(f"  SelectedKeyName: {selected_key_name}")
                        except Exception as e:
                            log(f"  SelectedKeyName error: {e}")
                except Exception as e:
                    log(f"TargetKey error: {e}")

                # Try to get RadiusKey property
                try:
                    radius_key = cdo.get_editor_property('radius_key')
                    log(f"RadiusKey: {radius_key}")
                    if radius_key:
                        try:
                            selected_key_name = radius_key.get_editor_property('selected_key_name')
                            log(f"  SelectedKeyName: {selected_key_name}")
                        except Exception as e:
                            log(f"  SelectedKeyName error: {e}")
                except Exception as e:
                    log(f"RadiusKey error: {e}")

    except Exception as e:
        log(f"Generated class error: {e}")

    # Also check a few other key tasks
    log("")
    log("=" * 70)
    log("Checking other tasks...")
    log("=" * 70)

    other_tasks = [
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_ToggleFocus",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SetKey",
    ]

    for task_path in other_tasks:
        log("")
        log(f"Checking: {task_path.split('/')[-1]}")

        bp = unreal.load_asset(task_path)
        if not bp:
            log(f"  [ERROR] Could not load")
            continue

        try:
            gen_class = bp.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    # List all blackboard key selector properties
                    for prop in ['target_key', 'key', 'Key', 'BlackboardKey', 'blackboard_key']:
                        try:
                            val = cdo.get_editor_property(prop)
                            if val:
                                log(f"  {prop}: {val}")
                                try:
                                    key_name = val.get_editor_property('selected_key_name')
                                    log(f"    SelectedKeyName: {key_name}")
                                except:
                                    pass
                        except:
                            pass
        except Exception as e:
            log(f"  Error: {e}")

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
