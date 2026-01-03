# migrate_bstat_to_cpp.py
# Reparents B_Stat Blueprint to inherit from UStatObject C++ class
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\migrate_bstat_to_cpp.py").read())

import unreal

def migrate_bstat():
    """Reparent B_Stat to UStatObject C++ base class."""

    print("=" * 60)
    print("B_Stat Migration: Reparenting to UStatObject")
    print("=" * 60)

    # Load the B_Stat Blueprint
    bstat_path = "/Game/SoulslikeFramework/Data/Stats/B_Stat"
    bstat_asset = unreal.EditorAssetLibrary.load_asset(bstat_path)

    if not bstat_asset:
        print(f"ERROR: Could not load B_Stat at {bstat_path}")
        return False

    print(f"Loaded B_Stat: {bstat_asset}")

    # Get the Blueprint object
    blueprint = unreal.Blueprint.cast(bstat_asset)
    if not blueprint:
        print("ERROR: Asset is not a Blueprint")
        return False

    # Get current parent class using generated_class
    try:
        gen_class = blueprint.get_editor_property('generated_class')
        if gen_class:
            current_parent = gen_class.get_super_class()
            print(f"Current parent class: {current_parent}")
    except Exception as e:
        print(f"Could not determine current parent class: {e}")

    # Load the C++ UStatObject class
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.StatObject")
    if not cpp_class:
        print("ERROR: Could not load UStatObject C++ class")
        print("Make sure the C++ code is compiled!")
        return False

    print(f"Target C++ class: {cpp_class}")

    # Reparent using BlueprintEditorLibrary
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(blueprint, cpp_class)
        if result:
            print("SUCCESS: Blueprint reparented!")
        else:
            # Check if already parented
            gen_class = blueprint.get_editor_property('generated_class')
            if gen_class:
                new_parent = gen_class.get_super_class()
                if new_parent and "StatObject" in str(new_parent):
                    print("INFO: Already parented to UStatObject")
                else:
                    print(f"WARNING: reparent returned False, current parent: {new_parent}")
    except Exception as e:
        print(f"ERROR during reparent: {e}")
        return False

    # Compile the Blueprint
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(blueprint)
        print("Compiled B_Stat")
    except Exception as e:
        print(f"WARNING: Compile issue: {e}")

    # Save the Blueprint
    try:
        unreal.EditorAssetLibrary.save_asset(bstat_path)
        print(f"Saved B_Stat")
    except Exception as e:
        print(f"WARNING: Could not save: {e}")

    # Verify the new parent
    try:
        gen_class = blueprint.get_editor_property('generated_class')
        if gen_class:
            new_parent = gen_class.get_super_class()
            print(f"Final parent class: {new_parent}")
    except:
        pass

    print("=" * 60)
    print("Migration complete!")
    print("")
    print("IMPORTANT: The Blueprint's AdjustValue function still exists.")
    print("To use NATIVE C++, you need to:")
    print("1. Delete B_Stat's AdjustValue function in Blueprint, OR")
    print("2. Call 'Adjust Stat (C++)' from AC_StatManager instead of 'AdjustStat'")
    print("")
    print("For now, test gameplay - existing Blueprint functions still work.")
    print("=" * 60)

    return True

# Run the migration
if __name__ == "__main__" or True:
    migrate_bstat()
