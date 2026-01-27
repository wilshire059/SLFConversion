"""
Resave AI assets to fix serialized enum references via redirect
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/resave_ai_output.txt"
output_lines = []

def log(msg):
    unreal.log_warning(f"[RESAVE_AI] {msg}")
    output_lines.append(msg)

def main():
    log("=" * 70)
    log("RESAVING AI ASSETS TO FIX ENUM REFERENCES")
    log("=" * 70)

    ai_assets = [
        "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy",
        "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Investigating",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PatrolPath",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PoiseBroken",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Uninterruptable",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_OutOfBounds",
    ]

    success_count = 0
    fail_count = 0

    for asset_path in ai_assets:
        try:
            log(f"Loading: {asset_path}")
            asset = unreal.load_asset(asset_path)
            if asset:
                log(f"  Loaded OK: {asset.get_name()}")
                # Try to save it
                if unreal.EditorAssetLibrary.save_asset(asset_path):
                    log(f"  [OK] Saved")
                    success_count += 1
                else:
                    log(f"  [FAIL] Could not save")
                    fail_count += 1
            else:
                log(f"  [FAIL] Could not load")
                fail_count += 1
        except Exception as e:
            log(f"  [ERROR] {e}")
            fail_count += 1

    log("")
    log("=" * 70)
    log(f"SUMMARY: {success_count} saved, {fail_count} failed")
    log("=" * 70)

if __name__ == "__main__":
    main()
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(output_lines))
    unreal.log_warning(f"[RESAVE_AI] Output written to {OUTPUT_FILE}")
