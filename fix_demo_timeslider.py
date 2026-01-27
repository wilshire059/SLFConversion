"""
Fix B_Demo_TimeSlider compile error by reparenting to C++ class.

The Blueprint has OnInteract function with different parameter name than C++ parent.
Reparenting clears the Blueprint logic and uses C++ implementation.
"""

import unreal

def fix_demo_timeslider():
    print("=" * 60)
    print("Fixing B_Demo_TimeSlider - Reparent to C++ class")
    print("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_Demo_TimeSlider"
    new_parent = "/Script/SLFConversion.B_Demo_TimeSlider"

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
    fix_demo_timeslider()
