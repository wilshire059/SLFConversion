# export_animbp_state_backup.py
# Export AnimBP state from BACKUP project (ground truth)
# Run this on C:\scripts\bp_only project

import unreal
import os

# Correct paths for bp_only project
ANIMBPS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
]

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/animbp_states"

def log(msg):
    print(f"[Backup Export] {msg}")
    unreal.log(f"[Backup Export] {msg}")

def main():
    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)

    automation_lib = unreal.SLFAutomationLibrary

    for asset_path in ANIMBPS:
        asset_name = asset_path.split("/")[-1]
        log(f"Exporting backup: {asset_name}")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log(f"  ERROR: Could not load {asset_path}")
            continue

        output_file = os.path.join(OUTPUT_DIR, f"{asset_name}_backup.json")
        result = automation_lib.export_anim_graph_state(asset, output_file)

        if result and len(result) > 100:
            log(f"  OK: {len(result)} bytes -> {output_file}")
        else:
            log(f"  ERROR: Export failed")

    log("Backup export complete!")

if __name__ == "__main__":
    main()
