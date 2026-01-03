# delete_blueprint_functions.py
# Deletes Blueprint function implementations so C++ BlueprintNativeEvent implementations run instead
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\delete_blueprint_functions.py").read())

import unreal

def delete_function_from_blueprint(blueprint, function_name):
    """
    Delete a function from a Blueprint.
    Returns True if deleted, False if not found or error.
    """
    try:
        # Get all graphs from the blueprint
        # Function graphs are separate from the event graph
        result = unreal.BlueprintEditorLibrary.remove_function(blueprint, function_name)
        if result:
            print(f"    [DELETED] {function_name}")
            return True
        else:
            print(f"    [NOT FOUND] {function_name}")
            return False
    except Exception as e:
        print(f"    [ERROR] {function_name}: {e}")
        return False

def process_blueprint(blueprint_path, functions_to_delete):
    """
    Process a single Blueprint, deleting specified functions.
    """
    print(f"\n{'='*60}")
    print(f"Processing: {blueprint_path}")
    print(f"{'='*60}")

    # Load the Blueprint
    asset = unreal.EditorAssetLibrary.load_asset(blueprint_path)
    if not asset:
        print(f"ERROR: Could not load {blueprint_path}")
        return False

    blueprint = unreal.Blueprint.cast(asset)
    if not blueprint:
        print(f"ERROR: Asset is not a Blueprint")
        return False

    print(f"Loaded: {blueprint}")
    print(f"Functions to delete: {functions_to_delete}")

    deleted_count = 0
    for func_name in functions_to_delete:
        if delete_function_from_blueprint(blueprint, func_name):
            deleted_count += 1

    print(f"\nDeleted {deleted_count}/{len(functions_to_delete)} functions")

    # Compile the Blueprint
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(blueprint)
        print("Compiled successfully")
    except Exception as e:
        print(f"Compile warning: {e}")

    # Save the Blueprint
    try:
        unreal.EditorAssetLibrary.save_asset(blueprint_path)
        print("Saved successfully")
    except Exception as e:
        print(f"Save warning: {e}")

    return True

def main():
    print("\n" + "="*70)
    print("  DELETE BLUEPRINT FUNCTIONS - Enable C++ Implementations")
    print("="*70)
    print("\nThis script deletes Blueprint functions so C++ runs instead.")
    print("BlueprintNativeEvent functions in C++ will now be called.\n")

    # Define what to delete for each Blueprint
    blueprints_to_process = [
        {
            "path": "/Game/SoulslikeFramework/Data/Stats/B_Stat",
            "functions": [
                "AdjustValue",
                "AdjustAffectedValue",
                "CalculatePercent",
                "GetRegenInfo",
                "UpdateStatInfo",
                "InitializeBaseClassValue"
            ]
        },
        {
            "path": "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer",
            "functions": [
                "QueueAction",
                "ToggleBuffer",
                "ConsumeInputBuffer",
                "ExecuteActionImmediately"
            ]
        }
    ]

    # Note: AC_StatManager functions have different names in C++ (with "Native" suffix)
    # So we can't just delete them - callers need to call the new names
    # We'll handle AC_StatManager separately

    success_count = 0
    for bp_info in blueprints_to_process:
        if process_blueprint(bp_info["path"], bp_info["functions"]):
            success_count += 1

    print("\n" + "="*70)
    print(f"  COMPLETE: Processed {success_count}/{len(blueprints_to_process)} Blueprints")
    print("="*70)
    print("\nNext steps:")
    print("1. Test gameplay - functions should now run in C++")
    print("2. Check Output Log for '[NATIVE C++]' messages")
    print("3. If issues, restore from backup")
    print("\nNote: AC_StatManager has C++ functions with 'Native' suffix.")
    print("      Callers need to use 'Adjust Stat (C++)' instead of 'AdjustStat'.")
    print("="*70)

# Run
if __name__ == "__main__" or True:
    main()
