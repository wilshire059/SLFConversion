# export_all_bt_subtrees.py
# Export ALL BT subtree decorators

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/bt_subtree_export.txt"

SUBTREES = [
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle",
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Investigating",
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_OutOfBounds",
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PatrolPath",
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PoiseBroken",
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam",
    "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Uninterruptable",
]

def log(msg):
    print(msg)
    with open(OUTPUT_FILE, "a") as f:
        f.write(msg + "\n")

def export_subtrees():
    # Clear output file
    with open(OUTPUT_FILE, "w") as f:
        f.write("=== BT Subtree Decorator Export ===\n\n")

    for bt_path in SUBTREES:
        log(f"\n{'='*60}")
        log(f"Exporting: {bt_path}")
        log(f"{'='*60}")
        try:
            result = unreal.SLFAutomationLibrary.export_bt_all_decorator_int_values(bt_path)
            log(result)
        except Exception as e:
            log(f"Error: {e}")

    log("\n\nExport complete!")

if __name__ == "__main__":
    export_subtrees()
