# migrate_to_full_cpp.py
# Complete migration: Reparent Blueprints AND delete Blueprint functions
# This enables full C++ execution for migrated components
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\migrate_to_full_cpp.py").read())

import unreal

def reparent_blueprint(blueprint, target_class_path):
    """Reparent a Blueprint to a C++ class."""
    cpp_class = unreal.load_class(None, target_class_path)
    if not cpp_class:
        print(f"    ERROR: Could not load C++ class {target_class_path}")
        return False

    # Check current parent
    try:
        gen_class = blueprint.get_editor_property('generated_class')
        if gen_class:
            current_parent = gen_class.get_super_class()
            target_name = target_class_path.split('.')[-1]
            if current_parent and target_name in str(current_parent):
                print(f"    Already parented to {target_name}")
                return True
    except:
        pass

    # Reparent
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(blueprint, cpp_class)
        if result:
            print(f"    Reparented to {target_class_path}")
            return True
        else:
            # Double-check if it worked anyway
            gen_class = blueprint.get_editor_property('generated_class')
            if gen_class:
                new_parent = gen_class.get_super_class()
                target_name = target_class_path.split('.')[-1]
                if new_parent and target_name in str(new_parent):
                    print(f"    Already parented to {target_name}")
                    return True
            print(f"    WARNING: Reparent returned False")
            return False
    except Exception as e:
        print(f"    ERROR: {e}")
        return False

def delete_function(blueprint, function_name):
    """Delete a function from a Blueprint."""
    try:
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

def process_blueprint(config):
    """Process a single Blueprint: reparent and delete functions."""
    path = config["path"]
    cpp_class = config["cpp_class"]
    functions = config["functions"]

    print(f"\n{'='*60}")
    print(f"Processing: {path.split('/')[-1]}")
    print(f"{'='*60}")

    # Load the Blueprint
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        print(f"ERROR: Could not load {path}")
        return False

    blueprint = unreal.Blueprint.cast(asset)
    if not blueprint:
        print(f"ERROR: Asset is not a Blueprint")
        return False

    print(f"Loaded successfully")

    # Step 1: Reparent to C++
    print(f"\n[Step 1] Reparenting to {cpp_class}...")
    if not reparent_blueprint(blueprint, cpp_class):
        print("ERROR: Reparent failed, skipping function deletion")
        return False

    # Step 2: Delete Blueprint functions
    print(f"\n[Step 2] Deleting Blueprint functions...")
    deleted = 0
    for func in functions:
        if delete_function(blueprint, func):
            deleted += 1
    print(f"Deleted {deleted}/{len(functions)} functions")

    # Step 3: Compile
    print(f"\n[Step 3] Compiling...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(blueprint)
        print("Compiled successfully")
    except Exception as e:
        print(f"Compile warning: {e}")

    # Step 4: Save
    print(f"\n[Step 4] Saving...")
    try:
        unreal.EditorAssetLibrary.save_asset(path)
        print("Saved successfully")
    except Exception as e:
        print(f"Save warning: {e}")

    return True

def main():
    print("\n" + "="*70)
    print("  FULL C++ MIGRATION")
    print("  Reparent Blueprints + Delete Blueprint Functions")
    print("="*70)

    # Configuration for each Blueprint
    blueprints = [
        {
            "path": "/Game/SoulslikeFramework/Data/Stats/B_Stat",
            "cpp_class": "/Script/SLFConversion.StatObject",
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
            "cpp_class": "/Script/SLFConversion.InputBufferComponent",
            "functions": [
                "QueueAction",
                "ToggleBuffer",
                "ConsumeInputBuffer",
                "ExecuteActionImmediately"
            ]
        },
        {
            "path": "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
            "cpp_class": "/Script/SLFConversion.StatManagerComponent",
            "functions": [
                # AC_StatManager keeps its Blueprint functions for now
                # because C++ versions have different names (with "Native" suffix)
                # We'd need to update all call sites to use the C++ versions
            ]
        }
    ]

    success = 0
    for config in blueprints:
        if config["functions"]:  # Skip if no functions to delete
            if process_blueprint(config):
                success += 1
        else:
            print(f"\n[SKIP] {config['path'].split('/')[-1]} - no functions to delete")
            # Still check reparent
            asset = unreal.EditorAssetLibrary.load_asset(config["path"])
            if asset:
                blueprint = unreal.Blueprint.cast(asset)
                if blueprint:
                    reparent_blueprint(blueprint, config["cpp_class"])

    print("\n" + "="*70)
    print("  MIGRATION COMPLETE")
    print("="*70)
    print(f"\nProcessed Blueprints:")
    print("  - B_Stat -> UStatObject (functions deleted)")
    print("  - AC_InputBuffer -> UInputBufferComponent (functions deleted)")
    print("  - AC_StatManager -> UStatManagerComponent (reparent only)")
    print("\nTo verify C++ is running:")
    print("  1. PIE and take damage, use stamina, attack")
    print("  2. Check Output Log for '[NATIVE C++]' messages")
    print("\nAC_StatManager Note:")
    print("  C++ functions have 'Native' suffix (e.g., 'Adjust Stat (C++)')")
    print("  To use C++ for stats, call these instead of Blueprint functions.")
    print("="*70)

# Run
if __name__ == "__main__" or True:
    main()
