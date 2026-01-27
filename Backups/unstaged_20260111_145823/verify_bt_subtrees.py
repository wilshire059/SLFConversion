"""
Verify all BT subtrees are loadable and have valid structure
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_VERIFY] {msg}")

def main():
    log("=" * 70)
    log("VERIFYING BT SUBTREES")
    log("=" * 70)

    subtrees = [
        "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Investigating",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PatrolPath",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PoiseBroken",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Uninterruptable",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_OutOfBounds",
    ]

    all_valid = True
    results = []

    for bt_path in subtrees:
        log("")
        log(f"Checking: {bt_path.split('/')[-1]}")

        bt = unreal.load_asset(bt_path)
        if not bt:
            log(f"  [ERROR] Could not load!")
            all_valid = False
            results.append((bt_path.split('/')[-1], "LOAD_ERROR", None, None))
            continue

        # Check if it's a valid BehaviorTree
        class_name = bt.get_class().get_name()
        log(f"  Class: {class_name}")

        if class_name != "BehaviorTree":
            log(f"  [ERROR] Not a BehaviorTree!")
            all_valid = False
            results.append((bt_path.split('/')[-1], "CLASS_ERROR", class_name, None))
            continue

        # Check blackboard asset
        try:
            bb = bt.get_editor_property('blackboard_asset')
            bb_name = bb.get_name() if bb else "None"
            log(f"  BlackboardAsset: {bb_name}")
        except Exception as e:
            bb_name = f"ERROR: {e}"
            log(f"  BlackboardAsset: {bb_name}")

        # Check root node
        try:
            root = bt.get_editor_property('root_node')
            root_name = root.get_class().get_name() if root else "None"
            log(f"  RootNode: {root_name}")
        except Exception as e:
            root_name = f"ERROR: {e}"
            log(f"  RootNode: {root_name}")

        # Try to export structure
        try:
            structure = unreal.SLFAutomationLibrary.export_behavior_tree_structure(bt_path)
            if structure and len(structure) > 50:
                log(f"  Structure: Valid ({len(structure)} chars)")
                results.append((bt_path.split('/')[-1], "OK", bb_name, root_name))
            else:
                log(f"  Structure: Empty or too short")
                results.append((bt_path.split('/')[-1], "STRUCTURE_ERROR", bb_name, root_name))
                all_valid = False
        except Exception as e:
            log(f"  Structure error: {e}")
            results.append((bt_path.split('/')[-1], "EXPORT_ERROR", bb_name, root_name))
            all_valid = False

    log("")
    log("=" * 70)
    log("SUMMARY:")
    log("-" * 70)
    for name, status, bb, root in results:
        log(f"  {name}: {status}")
    log("=" * 70)

    if all_valid:
        log("[OK] All BT subtrees are valid!")
    else:
        log("[WARN] Some BT subtrees have issues!")

    log("=" * 70)

if __name__ == "__main__":
    main()
