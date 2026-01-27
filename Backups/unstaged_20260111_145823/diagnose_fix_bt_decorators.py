"""
Diagnose and fix BT decorator IntValues
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_DEC_FIX] {msg}")

def main():
    log("=" * 70)
    log("DIAGNOSING AND FIXING BT DECORATOR INTVALUES")
    log("=" * 70)

    bt_paths = [
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Investigating",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PatrolPath",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PoiseBroken",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Uninterruptable",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_OutOfBounds",
    ]

    for bt_path in bt_paths:
        log("")
        log(f"Processing: {bt_path}")
        result = unreal.SLFAutomationLibrary.diagnose_and_fix_bt_decorator_int_values(bt_path)
        for line in result.split('\n'):
            log(f"  {line}")

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
