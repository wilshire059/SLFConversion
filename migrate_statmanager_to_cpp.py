"""
migrate_statmanager_to_cpp.py

Migrate AC_StatManager Blueprint to inherit from C++ UStatManagerComponent.

APPROACH: Properties Only
C++ provides primitive properties (bIsAiComponent, StatTable, SpeedAsset, Level, SelectedClassAsset).
Blueprint keeps its functions, event dispatchers, and complex TMap properties.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_statmanager_to_cpp.py").read())
"""

import unreal

print("=" * 60)
print("  AC_StatManager -> UStatManagerComponent Migration")
print("  Approach: Properties Only")
print("=" * 60)

# Paths
BP_PATH = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"
CPP_CLASS_PATH = "/Script/SLFConversion.StatManagerComponent"

# Step 1: Load the Blueprint
print("\n[Step 1] Loading AC_StatManager Blueprint...")
bp = unreal.load_asset(BP_PATH)
if not bp:
    print("[FAIL] Could not load AC_StatManager Blueprint")
    raise Exception("Blueprint load failed")
print(f"  [OK] Loaded: {bp.get_name()}")

# Get current parent
try:
    current_parent = bp.parent_class
    parent_name = current_parent.get_name() if current_parent else 'None'
    print(f"  Current parent: {parent_name}")
    if "StatManagerComponent" in parent_name:
        print("  [INFO] Already parented to StatManagerComponent!")
except:
    print("  Current parent: (unable to get)")

# Step 2: Load the C++ class
print("\n[Step 2] Loading C++ StatManagerComponent class...")
cpp_class = None

try:
    cpp_class = unreal.StatManagerComponent.static_class()
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
print("\n[Step 4] Reparenting AC_StatManager to UStatManagerComponent...")
try:
    success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
    if success:
        print("  [OK] Reparent succeeded")
    else:
        print("  [WARN] Reparent returned False, but may still have worked")
except Exception as e:
    print(f"  [ERROR] Reparent exception: {e}")
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
    if new_parent and "StatManagerComponent" in new_parent.get_name():
        print("  [OK] Parent class is now StatManagerComponent")
    else:
        print("  [WARN] Parent class may not be correct")
except Exception as e:
    print(f"  [ERROR] Could not verify parent: {e}")

# Step 6: Compile the Blueprint
print("\n[Step 6] Compiling Blueprint...")
try:
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    print("  [OK] Compilation completed")
except Exception as e:
    print(f"  [WARN] Compilation: {e}")

# Step 7: Save the Blueprint
print("\n[Step 7] Saving Blueprint...")
try:
    unreal.EditorAssetLibrary.save_asset(BP_PATH)
    print("  [OK] Saved")
except Exception as e:
    print(f"  [WARN] Save: {e}")

# Step 8: Post-migration diagnostics
print("\n[Step 8] Post-migration diagnostics...")
try:
    diag = unreal.BlueprintFixerLibrary.get_blueprint_diagnostics(bp)
    print(f"  Diagnostics:\n{diag}")
except:
    print("  (diagnostics not available)")

print("\n" + "=" * 60)
print("  MIGRATION COMPLETE")
print("=" * 60)
print("""
What was migrated:
- bIsAiComponent (bool)
- StatTable (UDataTable*)
- SpeedAsset (UDataAsset*)
- Level (int32)
- SelectedClassAsset (UDataAsset*)

What remains in Blueprint:
- All 12 functions (GetStat, AdjustStat, etc.)
- All 3 event dispatchers (OnStatsInitialized, OnLevelUpdated, OnSaveRequested)
- TMap properties (Stats, ActiveStats, StatOverrides)

Next steps:
1. Check the Blueprint for any compile errors
2. Test in Play mode - verify stats work
3. If working, proceed to plan full function migration with type alignment

For full function migration, we need to:
- Delete Blueprint UserDefinedStructs (FStatInfo, FStatOverride, FRegen)
- Create C++ versions with matching names
- Delete Blueprint E_ValueType enum
- Then C++ functions can match Blueprint signatures exactly
""")
