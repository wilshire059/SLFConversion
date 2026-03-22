"""
Fix PIE Blueprint compile errors by clearing ALL Blueprint logic and reparenting.

Errors to fix:
1. ANS_Trail - Target connection issue (clear ALL logic, reparent to UANS_Trail)
2. B_ShowcaseEnemy_StatDisplay - Type incompatibility (clear ALL logic)
3. B_ShowcaseEnemy_ExitCombat - Missing BehaviorManager (clear ALL logic)
4. B_Demo_TimeSlider - OnInteract parameter order (clear ALL logic)
5. B_LocationActor - DefaultSceneRoot conflict (reparent to AB_LocationActor)
"""

import unreal

BLUEPRINTS_TO_FIX = [
    {
        "name": "ANS_Trail",
        "path": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_Trail",
        "cpp_parent": "/Script/SLFConversion.ANS_Trail"
    },
    {
        "name": "B_ShowcaseEnemy_StatDisplay",
        "path": "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_ShowcaseEnemy_StatDisplay",
        "cpp_parent": "/Script/SLFConversion.B_ShowcaseEnemy_StatDisplay"
    },
    {
        "name": "B_ShowcaseEnemy_ExitCombat",
        "path": "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_ShowcaseEnemy_ExitCombat",
        "cpp_parent": "/Script/SLFConversion.B_ShowcaseEnemy_ExitCombat"
    },
    {
        "name": "B_Demo_TimeSlider",
        "path": "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_Demo_TimeSlider",
        "cpp_parent": "/Script/SLFConversion.B_Demo_TimeSlider"
    },
    {
        "name": "B_LocationActor",
        "path": "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_LocationActor",
        "cpp_parent": "/Script/SLFConversion.AB_LocationActor"
    }
]

def fix_blueprint(bp_info):
    """Fix a single Blueprint using SLFAutomationLibrary."""
    name = bp_info["name"]
    path = bp_info["path"]
    cpp_parent = bp_info["cpp_parent"]

    print(f"\n--- Fixing {name} ---")

    # Load Blueprint first
    bp = unreal.EditorAssetLibrary.load_asset(path)
    if not bp:
        print(f"  ERROR: Could not load {path}")
        return False

    print(f"  Loaded: {bp}")

    # Use ClearAllBlueprintLogic to clear BOTH EventGraphs AND FunctionGraphs
    try:
        result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
        print(f"  ClearAllBlueprintLogic: {result}")
    except Exception as e:
        print(f"  ClearAllBlueprintLogic failed: {e}")

    # Also clear EventGraph nodes specifically
    try:
        result = unreal.SLFAutomationLibrary.clear_all_event_graph_nodes(path, True)
        print(f"  ClearAllEventGraphNodes result: {result}")
    except Exception as e:
        print(f"  ClearAllEventGraphNodes failed: {e}")

    # Also try to clear function graphs
    try:
        result = unreal.SLFAutomationLibrary.clear_function_graphs(bp)
        print(f"  ClearFunctionGraphs: {result}")
    except Exception as e:
        print(f"  ClearFunctionGraphs failed (may not exist): {e}")

    # Now reparent to C++ class
    try:
        new_parent_class = unreal.load_class(None, cpp_parent)
        if new_parent_class:
            success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent_class)
            print(f"  Reparent to {cpp_parent}: {success}")
        else:
            print(f"  ERROR: Could not load parent class {cpp_parent}")
    except Exception as e:
        print(f"  Reparent failed: {e}")

    # Compile and save
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print(f"  Compiled!")
    except Exception as e:
        print(f"  Compile warning (expected): {e}")

    try:
        unreal.EditorAssetLibrary.save_asset(path)
        print(f"  Saved: {path}")
    except Exception as e:
        print(f"  Save failed: {e}")

    return True

def main():
    print("=" * 60)
    print("Fixing PIE Blueprint Compile Errors - Full Logic Clear")
    print("=" * 60)

    success_count = 0
    fail_count = 0

    for bp_info in BLUEPRINTS_TO_FIX:
        try:
            if fix_blueprint(bp_info):
                success_count += 1
            else:
                fail_count += 1
        except Exception as e:
            print(f"  ERROR fixing {bp_info['name']}: {e}")
            fail_count += 1

    print("\n" + "=" * 60)
    print(f"Complete: {success_count} succeeded, {fail_count} failed")
    print("=" * 60)

if __name__ == "__main__":
    main()
