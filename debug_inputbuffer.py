"""
debug_inputbuffer.py

Debug the InputBuffer system to find why jump/roll/attack don't work.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\debug_inputbuffer.py").read())
"""

import unreal

print("=" * 60)
print("  INPUT BUFFER DEBUG")
print("=" * 60)

# Check AC_InputBuffer
print("\n--- AC_InputBuffer ---")
bp = unreal.load_asset("/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer")
if bp:
    print(f"[OK] Loaded: {bp.get_name()}")

    # Check parent class
    try:
        parent = bp.parent_class
        print(f"  Parent: {parent.get_name() if parent else 'None'}")
    except:
        print("  Parent: (error getting)")

    # Check diagnostics
    diag = unreal.BlueprintFixerLibrary.get_blueprint_diagnostics(bp)
    print(f"  Diagnostics:\n{diag}")
else:
    print("[FAIL] Could not load AC_InputBuffer")

# Check B_Soulslike_Character
print("\n--- B_Soulslike_Character ---")
char_bp = unreal.load_asset("/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character")
if char_bp:
    print(f"[OK] Loaded: {char_bp.get_name()}")

    # Check for InputBuffer-related function calls
    print("\n  InputBuffer-related nodes:")
    info = unreal.BlueprintFixerLibrary.get_function_call_info(char_bp)
    for i in info:
        if "InputBuffer" in i or "QueueAction" in i or "OnInputBuffer" in i:
            print(f"    {i}")
else:
    print("[FAIL] Could not load B_Soulslike_Character")

# Check ANS_InputBuffer
print("\n--- ANS_InputBuffer ---")
ans_bp = unreal.load_asset("/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_InputBuffer")
if ans_bp:
    print(f"[OK] Loaded: {ans_bp.get_name()}")

    # Check function calls
    print("\n  Function calls:")
    info = unreal.BlueprintFixerLibrary.get_function_call_info(ans_bp)
    for i in info:
        print(f"    {i}")
else:
    print("[FAIL] Could not load ANS_InputBuffer")

# Check AN_TryGuard
print("\n--- AN_TryGuard ---")
guard_bp = unreal.load_asset("/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_TryGuard")
if guard_bp:
    print(f"[OK] Loaded: {guard_bp.get_name()}")

    # Check function calls
    print("\n  Function calls:")
    info = unreal.BlueprintFixerLibrary.get_function_call_info(guard_bp)
    for i in info:
        if "InputBuffer" in i or "Consume" in i:
            print(f"    {i}")
else:
    print("[FAIL] Could not load AN_TryGuard")

print("\n" + "=" * 60)
print("  END DEBUG")
print("=" * 60)
