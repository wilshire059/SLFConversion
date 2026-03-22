# export_bt_decorators.py
# Export ALL BT_Enemy decorators and their IntValue settings

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/bt_decorator_export.txt"

def log(msg):
    print(msg)
    with open(OUTPUT_FILE, "a") as f:
        f.write(msg + "\n")

def export_bt_decorators():
    # Clear output file
    with open(OUTPUT_FILE, "w") as f:
        f.write("=== BT_Enemy Decorator Export ===\n\n")

    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"

    # Use SLFAutomationLibrary to export all decorator IntValues
    log("Calling SLFAutomationLibrary.ExportBTAllDecoratorIntValues...")

    try:
        result = unreal.SLFAutomationLibrary.export_bt_all_decorator_int_values(bt_path)
        log(result)
    except AttributeError as e:
        log(f"Function not found: {e}")
        log("\nTrying FixBTStateDecoratorToValue for diagnosis...")
        try:
            # Use this to at least see root decorators
            result = unreal.SLFAutomationLibrary.fix_bt_state_decorator_to_value(bt_path, 999)  # Use 999 as dummy to just see values
            log(result)
        except Exception as e2:
            log(f"Error: {e2}")

    # Also try DiagnoseBlackboardStateKey for comparison
    log("\n\n=== Blackboard State Key ===")
    try:
        bb_result = unreal.SLFAutomationLibrary.diagnose_blackboard_state_key("/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard")
        log(bb_result)
    except Exception as e:
        log(f"Error: {e}")

if __name__ == "__main__":
    export_bt_decorators()
