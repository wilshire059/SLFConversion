"""
migrate_enum_direct.py

Migrate E_ValueType Blueprint UserDefinedEnum to C++ UENUM.
Direct approach - updates pin types AND converts DefaultValues from string to byte.

NO CoreRedirects needed - the C++ migration function handles:
1. Pin type updates (PinSubCategoryObject)
2. DefaultValue conversion ("NewEnumerator0" -> "0")
3. Switch node reconstruction

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_enum_direct.py").read())
"""

import unreal

print("=" * 70)
print("  E_ValueType Enum Migration - Direct Approach")
print("  Converts pin types AND DefaultValues")
print("=" * 70)

# Configuration
OLD_ENUM_NAME = "E_ValueType"
NEW_ENUM_PATH = "/Script/SLFConversion.E_ValueType"
DRY_RUN = False  # Set to True to preview changes

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
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_Status",
]

# =============================================================================
# Step 1: Verify prerequisites
# =============================================================================
print("\n[Step 1] Verifying prerequisites...")

bfl = unreal.BlueprintFixerLibrary
if not hasattr(bfl, 'migrate_enum_in_blueprint'):
    print("  [ERROR] BlueprintFixerLibrary.migrate_enum_in_blueprint not found!")
    print("  Make sure to compile C++ first!")
    raise Exception("C++ not compiled")

print("  [OK] BlueprintFixerLibrary available")

# =============================================================================
# Step 2: Migrate all blueprints (update types and values, NO compile yet)
# =============================================================================
print(f"\n[Step 2] Migrating blueprints (DryRun={DRY_RUN})...")

blueprints_loaded = {}
total_updated = 0

for bp_path in BLUEPRINTS_TO_MIGRATE:
    print(f"\n  Processing: {bp_path}")

    try:
        bp = unreal.load_asset(bp_path)
        if not bp:
            print(f"    [SKIP] Could not load")
            continue

        # Migrate enum (updates types AND converts values)
        updated = bfl.migrate_enum_in_blueprint(bp, OLD_ENUM_NAME, NEW_ENUM_PATH, DRY_RUN)
        total_updated += updated

        if updated > 0:
            print(f"    [{'WOULD UPDATE' if DRY_RUN else 'UPDATED'}] {updated} items")
        else:
            print(f"    [OK] No changes needed")

        blueprints_loaded[bp_path] = bp

    except Exception as e:
        print(f"    [ERROR] {e}")
        import traceback
        traceback.print_exc()

if DRY_RUN:
    print("\n" + "=" * 70)
    print("  DRY RUN COMPLETE")
    print("=" * 70)
    print(f"\nBlueprints analyzed: {len(blueprints_loaded)}")
    print(f"Total items would be updated: {total_updated}")
    print("\nTo perform actual migration, set DRY_RUN = False and run again.")
else:
    # =============================================================================
    # Step 3: Compile all blueprints
    # =============================================================================
    print(f"\n[Step 3] Compiling all blueprints...")

    compile_errors = []

    for bp_path, bp in blueprints_loaded.items():
        print(f"\n  Compiling: {bp_path.split('/')[-1]}")
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            print(f"    [OK] Compiled")
        except Exception as e:
            print(f"    [ERROR] {e}")
            compile_errors.append((bp_path, str(e)))

    # =============================================================================
    # Step 4: Save all blueprints
    # =============================================================================
    print(f"\n[Step 4] Saving all blueprints...")

    for bp_path in blueprints_loaded.keys():
        print(f"  Saving: {bp_path.split('/')[-1]}")
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path)
        except Exception as e:
            print(f"    [ERROR] {e}")

    # =============================================================================
    # Summary
    # =============================================================================
    print("\n" + "=" * 70)
    print("  MIGRATION SUMMARY")
    print("=" * 70)
    print(f"""
Blueprints processed: {len(blueprints_loaded)}
Total items updated: {total_updated}
Compile errors: {len(compile_errors)}
""")

    if compile_errors:
        print("Blueprints with compile errors:")
        for bp_path, error in compile_errors:
            print(f"  - {bp_path.split('/')[-1]}")

        print("""
If there are errors, restore from backup:
    Close Unreal Editor, then run:
    rm -rf "C:/scripts/SLFConversion/Content"
    cp -r "C:/scripts/SLFConversion_Migration/Backups/pre_enum_migration_clean_20251229/Content" "C:/scripts/SLFConversion/"
""")
    else:
        print("""
SUCCESS! All blueprints migrated and compiled.

Next steps:
1. Test in Play mode
2. If working, delete the Blueprint E_ValueType enum:
   - Navigate to /Game/SoulslikeFramework/Enums/
   - Delete E_ValueType
3. Compile all Blueprints again
""")
