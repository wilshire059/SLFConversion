"""
reparent_inputbuffer.py

Simple reparent script - run AFTER manually deleting BP functions/variables.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\reparent_inputbuffer.py").read())

Prerequisites:
    1. C++ code compiled (UInputBufferComponent exists)
    2. Manually deleted from AC_InputBuffer:
       - Functions: QueueAction, ConsumeInputBuffer, ToggleBuffer, ExecuteActionImmediately
       - Variables: IncomingActionTag, IgnoreNextOfActions, BufferOpen?, OnInputBufferConsumed
"""

import unreal


def run_reparent():
    print("=" * 60)
    print("AC_InputBuffer Reparent Script")
    print("=" * 60)
    print()

    # Load C++ class
    print("[1/4] Loading C++ class...")
    try:
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.InputBufferComponent")
        if not cpp_class:
            print("[ERROR] UInputBufferComponent not found. Compile C++ first!")
            return False
        print("       Found UInputBufferComponent")
    except Exception as e:
        print(f"[ERROR] {e}")
        return False

    # Load Blueprint
    print("[2/4] Loading Blueprint...")
    bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer"
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"[ERROR] Could not load: {bp_path}")
        return False
    print("       Loaded AC_InputBuffer")

    # Reparent
    print("[3/4] Reparenting to UInputBufferComponent...")
    try:
        success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if not success:
            print("[ERROR] Reparent failed!")
            print()
            print("Did you delete these from AC_InputBuffer first?")
            print("  Functions: QueueAction, ConsumeInputBuffer, ToggleBuffer, ExecuteActionImmediately")
            print("  Variables: IncomingActionTag, IgnoreNextOfActions, BufferOpen?, OnInputBufferConsumed")
            return False
        print("       Reparent successful!")
    except Exception as e:
        print(f"[ERROR] {e}")
        return False

    # Compile and save
    print("[4/4] Compiling and saving...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print("       Compiled and saved!")
    except Exception as e:
        print(f"[WARN] {e}")

    print()
    print("=" * 60)
    print("[SUCCESS] AC_InputBuffer now inherits from UInputBufferComponent!")
    print("=" * 60)
    print()
    print("C++ now provides:")
    print("  - IncomingActionTag (FGameplayTag)")
    print("  - IgnoreNextOfActions (FGameplayTagContainer)")
    print("  - bBufferOpen (bool)")
    print("  - OnInputBufferConsumed (delegate)")
    print("  - QueueAction(), ConsumeInputBuffer(), ToggleBuffer(), ExecuteActionImmediately()")
    print()
    print("Test in Play mode to verify input buffering works!")
    return True


# Run
run_reparent()
