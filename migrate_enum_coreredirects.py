"""
migrate_enum_coreredirects.py

Migrate E_ValueType Blueprint UserDefinedEnum to C++ UENUM using CoreRedirects.

APPROACH:
CoreRedirects in DefaultEngine.ini redirect the type automatically.
This script just needs to:
1. Load each Blueprint
2. RefreshAllNodes (picks up the redirected type)
3. Compile
4. Save (serializes the new C++ enum reference)

PREREQUISITE:
1. C++ enum must be compiled (E_ValueType in SLFStatTypes.h)
2. CoreRedirect must be in DefaultEngine.ini:
   +EnumRedirects=(OldName="/Game/SoulslikeFramework/Enums/E_ValueType.E_ValueType",NewName="/Script/SLFConversion.E_ValueType")

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_enum_coreredirects.py").read())
"""

import unreal

print("=" * 70)
print("  E_ValueType Enum Migration via CoreRedirects")
print("=" * 70)

# All Blueprints that use E_ValueType
BLUEPRINTS_TO_MIGRATE = [
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_Boss",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action",
    "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
    "/Game/SoulslikeFramework/Widgets/Boss/W_Boss_Healthbar",
    "/Game/SoulslikeFramework/Widgets/LevelUp/W_LevelUp",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_LevelUp",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_Status",
]

# =============================================================================
# Step 1: Verify prerequisites
# =============================================================================
print("\n[Step 1] Verifying prerequisites...")

# Check C++ enum exists
try:
    cpp_enum = unreal.load_object(None, "/Script/SLFConversion.E_ValueType")
    if cpp_enum:
        print(f"  [OK] C++ E_ValueType found: {cpp_enum}")
    else:
        print("  [ERROR] C++ E_ValueType not found!")
        print("  Make sure to compile C++ first!")
        raise Exception("C++ enum not found")
except Exception as e:
    print(f"  [ERROR] {e}")
    raise

# Check BlueprintFixerLibrary for RefreshAllNodes
try:
    bfl = unreal.BlueprintFixerLibrary
    if hasattr(bfl, 'reconstruct_all_nodes'):
        print("  [OK] BlueprintFixerLibrary.reconstruct_all_nodes available")
    else:
        raise Exception("reconstruct_all_nodes not found")
except AttributeError as e:
    print(f"  [ERROR] BlueprintFixerLibrary not found: {e}")
    raise

# =============================================================================
# Step 2: Process each Blueprint
# =============================================================================
print("\n[Step 2] Processing Blueprints...")

successful = []
errors = []

for bp_path in BLUEPRINTS_TO_MIGRATE:
    print(f"\n  Processing: {bp_path}")

    try:
        # Load the Blueprint
        bp = unreal.load_asset(bp_path)
        if not bp:
            print(f"    [SKIP] Could not load Blueprint")
            errors.append((bp_path, "Could not load"))
            continue

        # RefreshAllNodes - this will pick up the CoreRedirect
        print(f"    Refreshing nodes...")
        bfl.reconstruct_all_nodes(bp)

        # Compile
        print(f"    Compiling...")
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)

        # Save
        print(f"    Saving...")
        unreal.EditorAssetLibrary.save_asset(bp_path)

        print(f"    [OK] Migrated successfully")
        successful.append(bp_path)

    except Exception as e:
        print(f"    [ERROR] {e}")
        errors.append((bp_path, str(e)))
        import traceback
        traceback.print_exc()

# =============================================================================
# Summary
# =============================================================================
print("\n" + "=" * 70)
print("  MIGRATION SUMMARY")
print("=" * 70)
print(f"""
Successful: {len(successful)}
Errors: {len(errors)}
""")

if errors:
    print("Blueprints with errors:")
    for bp_path, error in errors:
        print(f"  - {bp_path}: {error}")

if len(successful) == len(BLUEPRINTS_TO_MIGRATE):
    print("""
SUCCESS! All Blueprints migrated.

Next steps:
1. Verify all Blueprints compile without errors
2. Test in Play mode
3. Delete the Blueprint E_ValueType enum:
   - Navigate to /Game/SoulslikeFramework/Enums/
   - Delete E_ValueType
4. Optionally remove the CoreRedirect from DefaultEngine.ini
   (after deleting the Blueprint enum, it's no longer needed)
""")
else:
    print("""
Some Blueprints had errors. Check the errors above.

If needed, restore from backup:
    Close Unreal Editor, then:
    rm -rf "C:/scripts/SLFConversion/Content"
    cp -r "C:/scripts/SLFConversion_Migration/Backups/pre_enum_migration_20251229_132227/Content" "C:/scripts/SLFConversion/"
""")
