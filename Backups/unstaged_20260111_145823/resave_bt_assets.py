"""
Re-save behavior tree assets to refresh enum decorator configuration
after EnumRedirect from E_AI_States to ESLFAIStates
"""
import unreal

def log(msg):
    unreal.log_warning(f"[RESAVE_BT] {msg}")

def main():
    log("=" * 70)
    log("RE-SAVING BEHAVIOR TREE ASSETS")
    log("=" * 70)

    bt_paths = [
        "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Investigating",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PatrolPath",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PoiseBroken",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Uninterruptable",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_OutOfBounds",
    ]

    # Also re-save the blackboard asset
    bb_paths = [
        "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard",
    ]

    all_paths = bb_paths + bt_paths

    for path in all_paths:
        asset = unreal.load_asset(path)
        if asset:
            # Mark dirty and save
            asset.modify()
            saved = unreal.EditorAssetLibrary.save_asset(path)
            if saved:
                log(f"[OK] Saved: {path}")
            else:
                log(f"[WARN] Failed to save: {path}")
        else:
            log(f"[ERROR] Could not load: {path}")

    log("")
    log("Done! Behavior trees should now use updated enum type.")

if __name__ == "__main__":
    main()
