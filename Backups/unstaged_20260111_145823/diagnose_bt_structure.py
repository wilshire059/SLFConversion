"""
Comprehensive BT structure diagnostic - shows ALL nodes, decorators, and their configuration
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_DIAG] {msg}")

def main():
    log("=" * 80)
    log("COMPREHENSIVE BEHAVIOR TREE STRUCTURE DIAGNOSIS")
    log("=" * 80)

    # Export BT_Enemy structure
    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"
    log(f"")
    log(f"=== BT_ENEMY FULL STRUCTURE ===")
    result = unreal.SLFAutomationLibrary.export_behavior_tree_structure(bt_path)
    for line in result.split('\n'):
        log(line)

    # Export each subtree
    subtrees = [
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Investigating",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PoiseBroken",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Uninterruptable",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_OutOfBounds",
    ]

    for subtree_path in subtrees:
        log(f"")
        log(f"=== {subtree_path.split('/')[-1]} STRUCTURE ===")
        result = unreal.SLFAutomationLibrary.export_behavior_tree_structure(subtree_path)
        for line in result.split('\n')[:50]:
            log(line)

    log("")
    log("=" * 80)
    log("DONE")
    log("=" * 80)

if __name__ == "__main__":
    main()
