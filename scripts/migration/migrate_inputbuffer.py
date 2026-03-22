"""
migrate_inputbuffer.py

Automated migration script for AC_InputBuffer.
Reparents the Blueprint to inherit from UInputBufferComponent.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_inputbuffer.py").read())

Prerequisites:
    1. C++ code compiled (UInputBufferComponent exists)
    2. Editor is open with project loaded
"""

import unreal


def check_cpp_class_exists():
    """Check if UInputBufferComponent C++ class is available."""
    try:
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.InputBufferComponent")
        if cpp_class:
            print("[OK] UInputBufferComponent C++ class found")
            return cpp_class
    except Exception as e:
        print(f"[ERROR] Exception loading C++ class: {e}")

    print("[ERROR] UInputBufferComponent C++ class NOT found")
    print("        Make sure you compiled the C++ code first!")
    return None


def load_blueprint():
    """Load AC_InputBuffer Blueprint asset."""
    bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer"

    bp = unreal.load_asset(bp_path)
    if bp:
        print(f"[OK] Loaded Blueprint: {bp_path}")
        return bp

    print(f"[ERROR] Could not load Blueprint: {bp_path}")
    return None


def delete_blueprint_functions(bp):
    """
    Delete the Blueprint function graphs that conflict with C++ functions.
    This MUST be done before reparenting.
    """
    functions_to_delete = [
        "QueueAction",
        "ConsumeInputBuffer",
        "ToggleBuffer",
        "ExecuteActionImmediately"
    ]

    print("[INFO] Deleting Blueprint functions that conflict with C++...")

    deleted_count = 0
    for func_name in functions_to_delete:
        try:
            # Try to remove the function
            result = unreal.BlueprintEditorLibrary.remove_function_from_blueprint(bp, func_name)
            if result:
                print(f"       - Deleted: {func_name}")
                deleted_count += 1
            else:
                print(f"       - Not found or couldn't delete: {func_name}")
        except Exception as e:
            print(f"       - Error deleting {func_name}: {e}")

    print(f"[INFO] Deleted {deleted_count}/{len(functions_to_delete)} functions")
    return deleted_count > 0


def delete_blueprint_variables(bp):
    """
    Delete Blueprint variables that conflict with C++ properties.
    """
    variables_to_delete = [
        "IncomingActionTag",
        "IgnoreNextOfActions",
        # "BufferOpen?" - this one has special character, might need different handling
        "OnInputBufferConsumed"  # This is an event dispatcher
    ]

    print("[INFO] Deleting Blueprint variables that conflict with C++...")

    deleted_count = 0
    for var_name in variables_to_delete:
        try:
            result = unreal.BlueprintEditorLibrary.remove_variable_from_blueprint(bp, var_name)
            if result:
                print(f"       - Deleted variable: {var_name}")
                deleted_count += 1
            else:
                print(f"       - Not found or couldn't delete: {var_name}")
        except Exception as e:
            print(f"       - Error deleting {var_name}: {e}")

    # Try the BufferOpen? variable with the special character
    try:
        result = unreal.BlueprintEditorLibrary.remove_variable_from_blueprint(bp, "BufferOpen?")
        if result:
            print(f"       - Deleted variable: BufferOpen?")
            deleted_count += 1
    except:
        pass

    print(f"[INFO] Deleted {deleted_count} variables")
    return True


def delete_event_dispatchers(bp):
    """
    Delete event dispatchers that conflict with C++ delegates.
    """
    print("[INFO] Attempting to delete event dispatchers...")

    try:
        # Event dispatchers are a special type - try removing as variable
        result = unreal.BlueprintEditorLibrary.remove_variable_from_blueprint(bp, "OnInputBufferConsumed")
        if result:
            print("       - Deleted event dispatcher: OnInputBufferConsumed")
            return True
    except Exception as e:
        print(f"       - Could not delete event dispatcher: {e}")

    return False


def reparent_blueprint(bp, new_parent_class):
    """Reparent the Blueprint to a new parent class."""
    try:
        success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent_class)

        if success:
            print("[OK] Blueprint reparented successfully")
            return True
        else:
            print("[ERROR] Reparent returned False")
            return False

    except Exception as e:
        print(f"[ERROR] Exception during reparent: {e}")
        return False


def compile_blueprint(bp):
    """Compile the Blueprint."""
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print("[OK] Blueprint compiled")
        return True
    except Exception as e:
        print(f"[WARN] Compile exception: {e}")
        return False


def save_blueprint(bp_path):
    """Save the Blueprint asset."""
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print("[OK] Blueprint saved")
        return True
    except Exception as e:
        print(f"[ERROR] Save exception: {e}")
        return False


def run_migration():
    """
    Main migration function.

    Steps:
    1. Check C++ class exists
    2. Load Blueprint
    3. Delete conflicting BP functions
    4. Delete conflicting BP variables
    5. Reparent to C++ class
    6. Compile
    7. Save
    """

    print("=" * 60)
    print("AC_InputBuffer Migration Script")
    print("=" * 60)
    print()

    # Step 1: Check C++ class
    print("[Step 1/7] Checking C++ class...")
    cpp_class = check_cpp_class_exists()
    if not cpp_class:
        print("\n[ABORT] C++ class not found. Please compile C++ first.")
        return False

    # Step 2: Load Blueprint
    print("\n[Step 2/7] Loading Blueprint...")
    bp = load_blueprint()
    if not bp:
        print("\n[ABORT] Could not load Blueprint.")
        return False

    # Step 3: Delete conflicting functions
    print("\n[Step 3/7] Deleting conflicting Blueprint functions...")
    delete_blueprint_functions(bp)

    # Step 4: Delete conflicting variables
    print("\n[Step 4/7] Deleting conflicting Blueprint variables...")
    delete_blueprint_variables(bp)
    delete_event_dispatchers(bp)

    # Step 5: Reparent
    print("\n[Step 5/7] Reparenting Blueprint...")
    if not reparent_blueprint(bp, cpp_class):
        print("\n[ABORT] Reparent failed.")
        print("\nTROUBLESHOOTING:")
        print("- Some BP functions/variables may still conflict")
        print("- Try opening AC_InputBuffer in editor and manually deleting:")
        print("  - Functions: QueueAction, ConsumeInputBuffer, ToggleBuffer, ExecuteActionImmediately")
        print("  - Variables: IncomingActionTag, IgnoreNextOfActions, BufferOpen?, OnInputBufferConsumed")
        print("- Then run this script again")
        return False

    # Step 6: Compile
    print("\n[Step 6/7] Compiling Blueprint...")
    compile_blueprint(bp)

    # Step 7: Save
    print("\n[Step 7/7] Saving Blueprint...")
    bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer"
    save_blueprint(bp_path)

    # Done!
    print("\n" + "=" * 60)
    print("[SUCCESS] Migration Complete!")
    print("=" * 60)
    print()
    print("AC_InputBuffer now inherits from UInputBufferComponent (C++)")
    print()
    print("The C++ class provides:")
    print("  - IncomingActionTag (FGameplayTag)")
    print("  - IgnoreNextOfActions (FGameplayTagContainer)")
    print("  - bBufferOpen (bool)")
    print("  - OnInputBufferConsumed (delegate)")
    print("  - QueueAction()")
    print("  - ConsumeInputBuffer()")
    print("  - ToggleBuffer()")
    print("  - ExecuteActionImmediately()")
    print()
    print("Test in Play mode to verify input buffering works!")
    print()

    return True


# Auto-run when script is executed
if __name__ == "__main__":
    run_migration()
else:
    run_migration()
