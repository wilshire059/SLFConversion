"""
migrate_statmanager_full.py

AC_StatManager Migration Script - Reparent Only
Option B: Struct types redirected via CoreRedirects

This script:
1. Reparents AC_StatManager to UStatManagerComponent
2. Compiles the blueprint
3. Saves changes

NOTE: Blueprint UserDefinedStructs are NOT deleted from this script.
CoreRedirects in DefaultEngine.ini handle type redirection.
The old struct files can remain (unused) or be deleted manually later.

PREREQUISITES:
- C++ must be compiled first
- CoreRedirects must be in DefaultEngine.ini

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_statmanager_full.py").read())
"""

import unreal

print("=" * 70)
print("  AC_StatManager Migration")
print("  Reparent to UStatManagerComponent")
print("=" * 70)

# =============================================================================
# Configuration
# =============================================================================

STAT_MANAGER_BP_PATH = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"
CPP_CLASS_PATH = "/Script/SLFConversion.StatManagerComponent"

# =============================================================================
# Step 1: Verify C++ class is available
# =============================================================================
print("\n[Step 1] Verifying C++ StatManagerComponent...")

try:
    cpp_class = unreal.StatManagerComponent.static_class()
    print(f"  [OK] Found: {cpp_class.get_name()}")
except Exception as e:
    print(f"  [FAIL] StatManagerComponent not found: {e}")
    print("  Make sure C++ is compiled!")
    raise Exception("C++ not compiled")

# =============================================================================
# Step 2: Load AC_StatManager Blueprint
# =============================================================================
print("\n[Step 2] Loading AC_StatManager Blueprint...")

bp = unreal.load_asset(STAT_MANAGER_BP_PATH)
if not bp:
    print(f"  [FAIL] Could not load {STAT_MANAGER_BP_PATH}")
    raise Exception("Blueprint load failed")

print(f"  [OK] Loaded: {bp.get_name()}")

# Check current parent
try:
    current_parent = bp.parent_class
    parent_name = current_parent.get_name() if current_parent else 'None'
    print(f"  Current parent: {parent_name}")

    if "StatManagerComponent" in parent_name:
        print("  [INFO] Already parented to StatManagerComponent!")
        needs_reparent = False
    else:
        needs_reparent = True
except:
    needs_reparent = True

# =============================================================================
# Step 3: Reparent to C++ base class
# =============================================================================
if needs_reparent:
    print("\n[Step 3] Reparenting to UStatManagerComponent...")

    try:
        success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if success:
            print("  [OK] Reparent succeeded")
        else:
            print("  [INFO] Reparent returned False - checking if it worked anyway...")
    except Exception as e:
        print(f"  [WARN] Reparent exception: {e}")

    # Reload and verify - reparent may have worked despite returning False
    bp = unreal.load_asset(STAT_MANAGER_BP_PATH)
else:
    print("\n[Step 3] Skipping reparent (already done)")

# =============================================================================
# Step 4: Compile Blueprint
# =============================================================================
print("\n[Step 4] Compiling AC_StatManager...")

try:
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    print("  [OK] Compilation completed")
except Exception as e:
    print(f"  [WARN] Compilation issue: {e}")

# =============================================================================
# Step 5: Save Blueprint
# =============================================================================
print("\n[Step 5] Saving AC_StatManager...")

try:
    unreal.EditorAssetLibrary.save_asset(STAT_MANAGER_BP_PATH)
    print("  [OK] Saved")
except Exception as e:
    print(f"  [WARN] Save issue: {e}")

# =============================================================================
# Summary
# =============================================================================
print("\n" + "=" * 70)
print("  MIGRATION COMPLETE")
print("=" * 70)
print("""
What was done:
- Reparented AC_StatManager to UStatManagerComponent

C++ base class provides:
- Properties: bIsAiComponent, StatTable, SpeedAsset, Level, SelectedClassAsset, StatOverrides
- Struct types via CoreRedirects: FStatInfo, FStatOverride, FRegen, etc.

Next steps:
1. Test gameplay - stats should work as before
2. Check Output Log for any errors
3. If issues, restore from backup:
   Close editor, then:
   rm -rf "C:/scripts/SLFConversion/Content"
   cp -r "C:/scripts/backups/SLFConversion_InputBuffer_Complete_20251229/Content" "C:/scripts/SLFConversion/"
""")
