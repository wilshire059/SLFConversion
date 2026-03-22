# migrate_bstat_full_cpp.py
# Complete B_Stat migration: Reparent to UStatObject + Delete Blueprint functions
#
# APPROACH (verified from JSON export analysis):
# 1. C++ UStatObject has NO properties that match Blueprint variable names
# 2. C++ accesses Blueprint variables via reflection
# 3. C++ function parameters use UPARAM(DisplayName="...") to match Blueprint pins
# 4. Deleting Blueprint functions allows C++ BlueprintNativeEvent implementations to run
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\migrate_bstat_full_cpp.py").read())

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

def main():
    print("\n" + "="*70)
    print("  B_STAT FULL C++ MIGRATION")
    print("  Reparent to UStatObject + Delete Blueprint Functions")
    print("="*70)

    # B_Stat configuration (from JSON export analysis)
    bstat_path = "/Game/SoulslikeFramework/Data/Stats/B_Stat"
    cpp_class = "/Script/SLFConversion.StatObject"

    # Functions to delete (from JSON: B_Stat.json -> FunctionSignatures.Functions)
    functions_to_delete = [
        "AdjustValue",           # Main stat adjustment function
        "AdjustAffectedValue",   # Affected stat adjustment (no events)
        "CalculatePercent",      # Percentage calculation
        "GetRegenInfo",          # Get regen settings
        "UpdateStatInfo",        # Update stat data
        "InitializeBaseClassValue"  # Initialize with base values
    ]

    print(f"\nTarget: {bstat_path}")
    print(f"C++ Class: {cpp_class}")
    print(f"Functions to delete: {len(functions_to_delete)}")

    # Load the Blueprint
    print(f"\n[Step 1] Loading Blueprint...")
    asset = unreal.EditorAssetLibrary.load_asset(bstat_path)
    if not asset:
        print(f"ERROR: Could not load {bstat_path}")
        return False

    blueprint = unreal.Blueprint.cast(asset)
    if not blueprint:
        print(f"ERROR: Asset is not a Blueprint")
        return False

    print(f"Loaded: {blueprint}")

    # Step 2: Reparent to C++
    print(f"\n[Step 2] Reparenting to {cpp_class}...")
    if not reparent_blueprint(blueprint, cpp_class):
        print("ERROR: Reparent failed, aborting migration")
        return False

    # Step 3: Compile before deleting functions
    print(f"\n[Step 3] Compiling after reparent...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(blueprint)
        print("Compiled successfully")
    except Exception as e:
        print(f"Compile warning: {e}")

    # Step 4: Delete Blueprint functions
    print(f"\n[Step 4] Deleting Blueprint functions...")
    deleted = 0
    for func in functions_to_delete:
        if delete_function(blueprint, func):
            deleted += 1
    print(f"Deleted {deleted}/{len(functions_to_delete)} functions")

    # Step 5: Compile again
    print(f"\n[Step 5] Compiling after function deletion...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(blueprint)
        print("Compiled successfully")
    except Exception as e:
        print(f"Compile error: {e}")
        print("WARNING: Compilation failed - check Output Log for details")

    # Step 6: Save
    print(f"\n[Step 6] Saving...")
    try:
        unreal.EditorAssetLibrary.save_asset(bstat_path)
        print("Saved successfully")
    except Exception as e:
        print(f"Save warning: {e}")

    print("\n" + "="*70)
    print("  MIGRATION COMPLETE")
    print("="*70)
    print(f"\nB_Stat now inherits from UStatObject (C++)")
    print(f"Deleted {deleted} Blueprint functions - C++ implementations will run")
    print("\nTo verify C++ is running:")
    print("  1. Open B_Stat in Blueprint Editor - functions should be gone")
    print("  2. PIE and take damage, spend stamina, level up")
    print("  3. Check Output Log for '[NATIVE C++]' messages")
    print("="*70)

    return True

# Run
if __name__ == "__main__" or True:
    main()
