"""
migrate_inputbuffer_continue.py

Continue migration from Step 3 (after cleanup already succeeded).

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_inputbuffer_continue.py").read())
"""

import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer"
CPP_CLASS_PATH = "/Script/SLFConversion.InputBufferComponent"

DEPENDENT_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Animations/Notifies/AN_TryGuard",
    "/Game/SoulslikeFramework/Animations/Notifies/ANS_InputBuffer",
    "/Game/SoulslikeFramework/Blueprints/B_Soulslike_Character",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_InventoryManager",
]


def log_section(title):
    print()
    print("=" * 70)
    print(f"  {title}")
    print("=" * 70)


def log_step(step_num, total, description):
    print()
    print(f"[Step {step_num}/{total}] {description}")
    print("-" * 50)


print("=" * 70)
print("  CONTINUING MIGRATION FROM STEP 3")
print("=" * 70)

# Load Blueprint
bp = unreal.load_asset(BP_PATH)
if not bp:
    print(f"[FAIL] Could not load {BP_PATH}")
else:
    # Step 3: Reparent
    log_step(3, 7, "REPARENTING TO C++")

    cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
    if cpp_class:
        print(f"  New parent: {cpp_class.get_name()}")

        success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if success:
            print("[OK] Reparent successful!")

            # Compile and save
            print("  Compiling and saving...")
            try:
                unreal.BlueprintEditorLibrary.compile_blueprint(bp)
                print("  [OK] Compiled")
            except Exception as e:
                print(f"  [WARN] Compile: {e}")

            try:
                unreal.EditorAssetLibrary.save_asset(BP_PATH)
                print("  [OK] Saved")
            except Exception as e:
                print(f"  [WARN] Save: {e}")

            # Step 4: Post-migration diagnostics
            log_step(4, 7, "POST-MIGRATION DIAGNOSTICS")
            bp = unreal.load_asset(BP_PATH)
            diag = unreal.BlueprintFixerLibrary.get_blueprint_diagnostics(bp)
            print(diag)

            # Step 5: Fix dependent Blueprints
            log_step(5, 7, "FIXING DEPENDENT BLUEPRINTS")
            for bp_path in DEPENDENT_BLUEPRINTS:
                bp_name = bp_path.split('/')[-1]
                print(f"\n--- {bp_name} ---")

                dep_bp = unreal.load_asset(bp_path)
                if not dep_bp:
                    print("  [WARN] Could not load")
                    continue

                # Reconstruct all nodes
                print("  Reconstructing nodes...")
                unreal.BlueprintFixerLibrary.reconstruct_all_nodes(dep_bp)

                # Fix function calls
                print("  Fixing function calls...")
                fixed = unreal.BlueprintFixerLibrary.refresh_function_calls_for_class(
                    dep_bp, "AC_InputBuffer_C", "InputBufferComponent"
                )
                print(f"  Fixed {fixed} function call nodes")

                # Fix event bindings
                print("  Fixing event bindings...")
                events = unreal.BlueprintFixerLibrary.fix_event_dispatcher_bindings(
                    dep_bp, "AC_InputBuffer"
                )
                print(f"  Fixed {events} event bindings")

                # Compile and save
                try:
                    unreal.BlueprintEditorLibrary.compile_blueprint(dep_bp)
                    unreal.EditorAssetLibrary.save_asset(bp_path)
                    print("  [OK] Compiled and saved")
                except Exception as e:
                    print(f"  [WARN] {e}")

            # Step 6: Verify
            log_step(6, 7, "VERIFICATION")
            bp = unreal.load_asset(BP_PATH)
            try:
                parent = bp.parent_class
                parent_name = parent.get_name() if parent else "None"
            except:
                parent_name = "Unknown"

            print(f"  Parent class: {parent_name}")

            if "InputBufferComponent" in parent_name:
                print("[OK] Migration verified!")
            else:
                print(f"[WARN] Expected InputBufferComponent")

            # Summary
            log_step(7, 7, "SUMMARY")
            print("[DONE] Migration complete!")
            print()
            print("Test in Play mode to verify input buffering works.")

        else:
            print("[FAIL] Reparent failed!")
            print("Check Output Log for LogBlueprintFixer messages")
    else:
        print("[FAIL] Could not load C++ class")
