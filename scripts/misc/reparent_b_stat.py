"""
reparent_b_stat.py - Phase A: Reparent B_Stat to USLFStatObject

This must be done BEFORE updating C++ TMap types to use USLFStatObject.
After running this script, B_Stat will inherit from USLFStatObject,
making it compatible with TMap<TSubclassOf<USLFStatObject>, ...> types.

Prerequisites:
- USLFStatObject C++ class exists and is compiled
- B_Stat Blueprint exists at expected path

After running this script:
1. Verify B_Stat now inherits from USLFStatObject in the editor
2. Update C++ TMaps to use USLFStatObject types
3. Recompile C++
4. Run the main migration script
"""

import unreal


def reparent_b_stat():
    """Reparent B_Stat Blueprint to USLFStatObject C++ class."""

    automation_lib = unreal.SLFAutomationLibrary

    print("=" * 60)
    print("PHASE A: Reparent B_Stat to USLFStatObject")
    print("=" * 60)

    # Load B_Stat Blueprint
    b_stat_path = "/Game/SoulslikeFramework/Data/Stats/B_Stat"
    b_stat = unreal.EditorAssetLibrary.load_asset(b_stat_path)

    if not b_stat:
        print(f"ERROR: Could not load B_Stat at {b_stat_path}")
        return False

    print(f"\nLoaded: {b_stat.get_name()}")

    # Get current parent
    try:
        current_parent = b_stat.get_class().get_super_class()
        print(f"Current parent: {current_parent.get_name() if current_parent else 'None'}")
    except:
        print("Could not determine current parent")

    # Find USLFStatObject class - try multiple methods
    new_parent = None

    # Method 1: Try to get it from the SLFConversion module
    try:
        new_parent = unreal.SLFStatObject.static_class()
        print(f"Found via unreal.SLFStatObject: {new_parent}")
    except:
        pass

    # Method 2: Try load_class with full path
    if not new_parent:
        try:
            new_parent = unreal.load_class(None, "/Script/SLFConversion.SLFStatObject")
            print(f"Found via load_class (SLFStatObject): {new_parent}")
        except:
            pass

    # Method 3: Try find_object
    if not new_parent:
        try:
            new_parent = unreal.find_object(None, "/Script/SLFConversion.SLFStatObject")
            print(f"Found via find_object: {new_parent}")
        except:
            pass

    # Method 4: Try with U prefix in path
    if not new_parent:
        try:
            new_parent = unreal.load_class(None, "/Script/SLFConversion.USLFStatObject")
            print(f"Found via load_class (USLFStatObject): {new_parent}")
        except:
            pass

    if not new_parent:
        print("ERROR: Could not find USLFStatObject class")
        print("Make sure the C++ module is compiled and loaded")
        print("Try running: print(dir(unreal)) to check available classes")
        return False

    print(f"Target parent: {new_parent.get_name()}")

    # Reparent B_Stat
    print(f"\n>>> Reparenting {b_stat.get_name()} to USLFStatObject...")
    result = automation_lib.reparent_blueprint(b_stat, new_parent)

    if result:
        print("[SUCCESS] B_Stat reparented to USLFStatObject")

        # Save the asset
        unreal.EditorAssetLibrary.save_loaded_asset(b_stat)
        print(f"[SAVED] {b_stat.get_name()}")

        print("\n" + "=" * 60)
        print("PHASE A COMPLETE")
        print("=" * 60)
        print("\nNext steps:")
        print("1. Verify B_Stat inherits from USLFStatObject in the editor")
        print("2. Update SoulslikeStatComponent.h TMaps to use USLFStatObject")
        print("3. Recompile C++")
        print("4. Run the main migration script (run_fix_all.py)")

        return True
    else:
        print("[FAILED] Reparent operation failed")
        return False


if __name__ == "__main__":
    reparent_b_stat()
