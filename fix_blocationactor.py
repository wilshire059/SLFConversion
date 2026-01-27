"""
Fix B_LocationActor compile error by reparenting to AB_LocationActor.

The Blueprint has a DefaultSceneRoot in its SCS, but was reparented to
SLFInteractableBase which also creates DefaultSceneRoot in C++. This causes
a property conflict.

Fix: Reparent to AB_LocationActor which is a simple AActor subclass.
"""

import unreal

def fix_location_actor():
    print("=" * 60)
    print("Fixing B_LocationActor - Reparent to AB_LocationActor")
    print("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_LocationActor"
    new_parent = "/Script/SLFConversion.AB_LocationActor"

    # Load the blueprint
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return False

    print(f"Loaded: {bp}")

    # Check the Blueprint's parent_class property
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
    fix_location_actor()
