"""
Fix Showcase Demo Blueprint compile errors by clearing stale Blueprint logic.

These are demo helper actors with outdated Blueprint calls:
- B_ShowcaseEnemy_StatDisplay: Uses old UB_Stat API (GetStatMaxValue/GetStatCurrentValue)
- B_ShowcaseEnemy_ExitCombat: References AC_AI_BehaviorManager which doesn't exist

Solution: Clear their EventGraphs since they're not core gameplay functionality.
"""

import unreal

SHOWCASE_BLUEPRINTS = [
    {
        "name": "B_ShowcaseEnemy_StatDisplay",
        "path": "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_ShowcaseEnemy_StatDisplay",
        "cpp_parent": "/Script/SLFConversion.B_ShowcaseEnemy_StatDisplay"
    },
    {
        "name": "B_ShowcaseEnemy_ExitCombat",
        "path": "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_ShowcaseEnemy_ExitCombat",
        "cpp_parent": "/Script/SLFConversion.B_ShowcaseEnemy_ExitCombat"
    }
]

def fix_showcase_blueprint(bp_info):
    """Fix a single showcase Blueprint by reparenting to C++ class."""
    name = bp_info["name"]
    path = bp_info["path"]
    cpp_parent = bp_info["cpp_parent"]

    print(f"\n--- Fixing {name} ---")

    # Load the blueprint
    bp = unreal.EditorAssetLibrary.load_asset(path)
    if not bp:
        print(f"  ERROR: Could not load {path}")
        return False

    print(f"  Loaded: {bp}")

    # Load the C++ parent class
    new_parent_class = unreal.load_class(None, cpp_parent)
    if not new_parent_class:
        print(f"  ERROR: Could not load new parent class {cpp_parent}")
        return False

    print(f"  New parent class: {new_parent_class.get_name()}")

    # Reparent to clear Blueprint logic
    try:
        success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent_class)
        if success:
            print(f"  Reparenting successful!")

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(path)
            print(f"  Saved: {path}")
            return True
        else:
            print(f"  ERROR: Reparenting failed")
            return False
    except Exception as e:
        print(f"  ERROR: {e}")
        return False

def main():
    print("=" * 60)
    print("Fixing Showcase Demo Blueprints")
    print("=" * 60)

    success_count = 0
    fail_count = 0

    for bp_info in SHOWCASE_BLUEPRINTS:
        if fix_showcase_blueprint(bp_info):
            success_count += 1
        else:
            fail_count += 1

    print("\n" + "=" * 60)
    print(f"Complete: {success_count} succeeded, {fail_count} failed")
    print("=" * 60)

if __name__ == "__main__":
    main()
