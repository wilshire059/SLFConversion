"""
Fix ANS_Trail compile error by reparenting to C++ UANS_Trail.

The Blueprint has leftover logic trying to use 'self' as Actor,
but ANS_Trail inherits from UAnimNotifyState (a UObject, not AActor).
Reparenting to C++ class clears the conflicting Blueprint logic.
"""

import unreal

def fix_ans_trail():
    print("=" * 60)
    print("Fixing ANS_Trail - Reparent to C++ UANS_Trail")
    print("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_Trail"
    new_parent = "/Script/SLFConversion.ANS_Trail"  # UANS_Trail

    # Load the blueprint
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return False

    print(f"Loaded: {bp}")

    # Check the Blueprint's current parent class
    try:
        current_parent = bp.get_editor_property("parent_class")
        if current_parent:
            print(f"Current parent: {current_parent.get_name()}")
        else:
            print("Current parent: None")
    except:
        print("Could not get current parent")

    # Load the new parent class
    new_parent_class = unreal.load_class(None, new_parent)
    if not new_parent_class:
        print(f"ERROR: Could not load new parent class {new_parent}")
        return False

    print(f"New parent class: {new_parent_class.get_name()}")

    # Reparent
    try:
        success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent_class)
        if success:
            print("Reparenting successful!")

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print(f"Saved: {bp_path}")
            return True
        else:
            print("ERROR: Reparenting failed")
            return False
    except Exception as e:
        print(f"ERROR: {e}")
        return False

if __name__ == "__main__":
    fix_ans_trail()
