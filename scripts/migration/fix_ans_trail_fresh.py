"""
Fix ANS_Trail by restoring from backup and reparenting fresh.

The Blueprint has corrupted state - restore from bp_only backup and reparent to C++ class.
"""

import unreal
import shutil
import os

def fix_ans_trail_fresh():
    print("=" * 60)
    print("Fixing ANS_Trail - Restore and Reparent Fresh")
    print("=" * 60)

    # Paths
    bp_only_path = "C:/scripts/bp_only/Content/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_Trail.uasset"
    target_path = "C:/scripts/SLFConversion/Content/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_Trail.uasset"
    unreal_path = "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_Trail"
    cpp_parent = "/Script/SLFConversion.ANS_Trail"

    # Step 1: Copy from backup (using Python file operations)
    print(f"Restoring from backup...")
    try:
        if os.path.exists(bp_only_path):
            shutil.copy2(bp_only_path, target_path)
            print(f"  Copied from {bp_only_path}")
        else:
            print(f"  ERROR: Backup not found at {bp_only_path}")
            return False
    except Exception as e:
        print(f"  ERROR copying: {e}")
        return False

    # Step 2: Load the restored Blueprint
    print(f"Loading restored Blueprint...")
    bp = unreal.EditorAssetLibrary.load_asset(unreal_path)
    if not bp:
        print(f"  ERROR: Could not load {unreal_path}")
        return False
    print(f"  Loaded: {bp}")

    # Step 3: Clear ALL logic using SLFAutomationLibrary
    print(f"Clearing all Blueprint logic...")
    try:
        result = unreal.SLFAutomationLibrary.clear_graphs_keep_variables(bp)
        print(f"  ClearGraphsKeepVariables: {result}")
    except Exception as e:
        print(f"  Clear failed: {e}")

    # Step 4: Reparent to C++ class
    print(f"Reparenting to C++ class...")
    try:
        new_parent_class = unreal.load_class(None, cpp_parent)
        if new_parent_class:
            success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent_class)
            print(f"  Reparent: {success}")
        else:
            print(f"  ERROR: Could not load parent class {cpp_parent}")
            return False
    except Exception as e:
        print(f"  Reparent failed: {e}")
        return False

    # Step 5: Compile and save
    print(f"Compiling and saving...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print(f"  Compiled!")
    except Exception as e:
        print(f"  Compile warning: {e}")

    try:
        unreal.EditorAssetLibrary.save_asset(unreal_path)
        print(f"  Saved!")
    except Exception as e:
        print(f"  Save failed: {e}")
        return False

    print("Done!")
    return True

if __name__ == "__main__":
    fix_ans_trail_fresh()
