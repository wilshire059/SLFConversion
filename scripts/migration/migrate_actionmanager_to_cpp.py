"""
migrate_actionmanager_to_cpp.py

Migrate AC_ActionManager Blueprint to inherit from C++ UActionManagerComponent.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_actionmanager_to_cpp.py").read())
"""

import unreal

print("=" * 60)
print("  AC_ActionManager -> UActionManagerComponent Migration")
print("=" * 60)

# Paths
BP_PATH = "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager"
CPP_CLASS_PATH = "/Script/SLFConversion.ActionManagerComponent"

# Step 1: Load the Blueprint
print("\n[Step 1] Loading AC_ActionManager Blueprint...")
bp = unreal.load_asset(BP_PATH)
if not bp:
    print("[FAIL] Could not load AC_ActionManager Blueprint")
    raise Exception("Blueprint load failed")
print(f"  [OK] Loaded: {bp.get_name()}")

# Get current parent
try:
    current_parent = bp.parent_class
    print(f"  Current parent: {current_parent.get_name() if current_parent else 'None'}")
except:
    print("  Current parent: (unable to get)")

# Step 2: Load the C++ class
print("\n[Step 2] Loading C++ ActionManagerComponent class...")
cpp_class = None

# Try different methods to get the C++ class
try:
    cpp_class = unreal.ActionManagerComponent.static_class()
    print(f"  [OK] Found via static_class(): {cpp_class.get_name()}")
except:
    try:
        cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
        print(f"  [OK] Found via load_class(): {cpp_class.get_name()}")
    except Exception as e:
        print(f"  [FAIL] Could not load C++ class: {e}")
        raise

if not cpp_class:
    print("[FAIL] C++ class is None")
    raise Exception("C++ class load failed")

# Step 3: Get current Blueprint diagnostics
print("\n[Step 3] Pre-migration diagnostics...")
try:
    diag = unreal.BlueprintFixerLibrary.get_blueprint_diagnostics(bp)
    print(f"  Diagnostics:\n{diag}")
except:
    print("  (diagnostics not available)")

# Step 4: Reparent the Blueprint
print("\n[Step 4] Reparenting AC_ActionManager to UActionManagerComponent...")
try:
    # Use FBlueprintEditorUtils equivalent
    success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
    if success:
        print("  [OK] Reparent succeeded")
    else:
        print("  [WARN] Reparent returned False, but may still have worked")
except Exception as e:
    print(f"  [ERROR] Reparent exception: {e}")
    # Try alternative method
    try:
        bp.parent_class = cpp_class
        print("  [OK] Direct parent assignment succeeded")
    except Exception as e2:
        print(f"  [FAIL] Direct assignment also failed: {e2}")
        raise

# Step 5: Verify new parent
print("\n[Step 5] Verifying new parent class...")
try:
    new_parent = bp.parent_class
    print(f"  New parent: {new_parent.get_name() if new_parent else 'None'}")
    if new_parent and "ActionManagerComponent" in new_parent.get_name():
        print("  [OK] Parent class is now ActionManagerComponent")
    else:
        print("  [WARN] Parent class may not be correct")
except Exception as e:
    print(f"  [ERROR] Could not verify parent: {e}")

# Step 6: Reconstruct all nodes
print("\n[Step 6] Reconstructing all Blueprint nodes...")
try:
    unreal.BlueprintFixerLibrary.reconstruct_all_nodes(bp)
    print("  [OK] Nodes reconstructed")
except Exception as e:
    print(f"  [WARN] Node reconstruction: {e}")

# Step 7: Compile the Blueprint
print("\n[Step 7] Compiling Blueprint...")
try:
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    print("  [OK] Compilation completed")
except Exception as e:
    print(f"  [WARN] Compilation: {e}")

# Step 8: Save the Blueprint
print("\n[Step 8] Saving Blueprint...")
try:
    unreal.EditorAssetLibrary.save_asset(BP_PATH)
    print("  [OK] Saved")
except Exception as e:
    print(f"  [WARN] Save: {e}")

# Step 9: Post-migration diagnostics
print("\n[Step 9] Post-migration diagnostics...")
try:
    diag = unreal.BlueprintFixerLibrary.get_blueprint_diagnostics(bp)
    print(f"  Diagnostics:\n{diag}")
except:
    print("  (diagnostics not available)")

# Step 10: Fix dependent Blueprints
print("\n[Step 10] Fixing dependent Blueprints...")
dependent_bps = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Dodge",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_StartSprinting",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_StopSprinting",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_InteractionManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_LadderManager",
]

for dep_path in dependent_bps:
    dep_bp = unreal.load_asset(dep_path)
    if dep_bp:
        print(f"  Processing: {dep_bp.get_name()}")
        try:
            unreal.BlueprintFixerLibrary.reconstruct_all_nodes(dep_bp)
            unreal.BlueprintEditorLibrary.compile_blueprint(dep_bp)
            unreal.EditorAssetLibrary.save_asset(dep_path)
            print(f"    [OK] Fixed and saved")
        except Exception as e:
            print(f"    [WARN] {e}")
    else:
        print(f"  [SKIP] Not found: {dep_path}")

print("\n" + "=" * 60)
print("  MIGRATION COMPLETE")
print("=" * 60)
print("""
Next steps:
1. Open AC_ActionManager in Blueprint Editor
2. Check for any remaining errors
3. Test in Play mode - verify actions work (attack, dodge, jump)
4. Check logs for ActionManager: messages

If issues occur:
- Check that PerformAction event is properly bound
- Verify AvailableActions map is populated on BeginPlay
- Ensure B_Action.ExecuteAction is being called
""")
