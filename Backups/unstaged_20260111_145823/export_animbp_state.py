# export_animbp_state.py
# Export AnimBP state to JSON for before/after diffing

import unreal
import os

# AnimBPs to export
ANIMBPS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Animation/ABP_SoulslikeCharacter",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Animation/ABP_SoulslikeNPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Animation/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Animation/ABP_SoulslikeBossNew",
]

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/animbp_states"

def log(msg):
    print(f"[AnimBP Export] {msg}")
    unreal.log(f"[AnimBP Export] {msg}")

def export_animbp_states():
    # Create output directory
    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)

    automation_lib = unreal.SLFAutomationLibrary

    for asset_path in ANIMBPS:
        asset_name = asset_path.split("/")[-1]
        log(f"Exporting: {asset_name}")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log(f"  ERROR: Could not load {asset_path}")
            continue

        output_file = os.path.join(OUTPUT_DIR, f"{asset_name}_state.json")

        # Call our C++ export function
        result = automation_lib.export_anim_graph_state(asset, output_file)

        if result and len(result) > 100:
            log(f"  OK: Exported to {output_file}")
        else:
            log(f"  WARNING: Export may have failed - {result[:100] if result else 'empty'}")

    log("Done!")

if __name__ == "__main__":
    export_animbp_states()
