"""
fix_ans_inputbuffer.py

Fix the ToggleBuffer nodes in ANS_InputBuffer after C++ migration.
The parameter name changed from "BufferOpen?" to "BufferOpen".

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\fix_ans_inputbuffer.py").read())
"""

import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_InputBuffer"

print("=" * 60)
print("  FIXING ANS_InputBuffer")
print("=" * 60)

bp = unreal.load_asset(BP_PATH)
if not bp:
    print(f"[FAIL] Could not load {BP_PATH}")
else:
    print(f"[OK] Loaded {bp.get_name()}")

    # Reconstruct all nodes - this will refresh pins to match C++ signatures
    print("\n  Reconstructing all nodes...")
    unreal.BlueprintFixerLibrary.reconstruct_all_nodes(bp)
    print("  [OK] Nodes reconstructed")

    # Compile
    print("\n  Compiling...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print("  [OK] Compiled")
    except Exception as e:
        print(f"  [WARN] Compile: {e}")

    # Save
    print("\n  Saving...")
    try:
        unreal.EditorAssetLibrary.save_asset(BP_PATH)
        print("  [OK] Saved")
    except Exception as e:
        print(f"  [WARN] Save: {e}")

    print("\n" + "=" * 60)
    print("  DONE - Test jump/roll/attack in Play mode!")
    print("=" * 60)
