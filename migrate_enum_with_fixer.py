"""
migrate_enum_with_fixer.py

Migrate E_ValueType Blueprint UserDefinedEnum to C++ UENUM using BlueprintFixerLibrary.

HYBRID APPROACH:
- CustomEvents with UserDefinedPins: Update directly (no reconstruction needed)
- Delegate-bound events: Need RefreshAllNodes AFTER dispatcher is compiled

PASS 1: Update all pin types, definitions, and UserDefinedPins in ALL blueprints
PASS 2: Compile B_Stat FIRST (creates delegate with C++ enum signature)
PASS 3: RefreshAllNodes on blueprints with delegate-bound events
PASS 4: Compile remaining blueprints
PASS 5: Save ALL blueprints

PREREQUISITE: Compile C++ first! The BlueprintFixerLibrary must be compiled.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_enum_with_fixer.py").read())
"""

import unreal

print("=" * 70)
print("  E_ValueType Enum Migration via BlueprintFixerLibrary")
print("  HYBRID: Direct update + RefreshAllNodes for bound events")
print("=" * 70)

# Configuration
OLD_ENUM_PATH = "E_ValueType"  # Blueprint enum name (partial match works)
NEW_ENUM_PATH = "/Script/SLFConversion.E_ValueType"  # C++ enum full path
DRY_RUN = False  # Set to False to actually make changes

# Blueprint that owns dispatchers (must be compiled FIRST)
DISPATCHER_OWNER = "/Game/SoulslikeFramework/Data/Stats/B_Stat"

# Blueprints that have delegate-bound events (need RefreshAllNodes after dispatcher compiled)
BLUEPRINTS_WITH_BOUND_EVENTS = [
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
    "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
]

# List of Blueprints that use E_ValueType (from JSON analysis)
BLUEPRINTS_TO_MIGRATE = [
    # B_Stat FIRST - owns OnStatUpdated dispatcher
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",
    # Components that bind to B_Stat's dispatchers
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_Boss",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action",
    # Widgets
    "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
    "/Game/SoulslikeFramework/Widgets/Boss/W_Boss_Healthbar",
    "/Game/SoulslikeFramework/Widgets/LevelUp/W_LevelUp",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_LevelUp",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_Status",
]

# =============================================================================
# Step 1: Verify BlueprintFixerLibrary is available
# =============================================================================
print("\n[Step 1] Checking BlueprintFixerLibrary...")
try:
    bfl = unreal.BlueprintFixerLibrary

    required_funcs = ['find_enum_usage', 'migrate_enum_in_blueprint', 'reconstruct_all_nodes']
    for func in required_funcs:
        if hasattr(bfl, func):
            print(f"  [OK] {func}: Available")
        else:
            print(f"  [ERROR] {func}: NOT FOUND - Did you compile C++?")
            raise Exception(f"BlueprintFixerLibrary.{func} not found")

except AttributeError as e:
    print(f"  [ERROR] BlueprintFixerLibrary not found: {e}")
    print("  Make sure to compile C++ code first!")
    raise

# =============================================================================
# PASS 1: Update all definitions, pins, and UserDefinedPins (NO COMPILE)
# =============================================================================
print(f"\n[PASS 1] Updating all blueprints (DryRun={DRY_RUN})...")
print("         Updates: pins, UserDefinedPins, dispatcher signatures, switch nodes")

total_updated = 0
blueprints_loaded = {}  # path -> (blueprint, updated_count)
dispatcher_owner_bp = None

for bp_path in BLUEPRINTS_TO_MIGRATE:
    print(f"\n  Processing: {bp_path}")

    try:
        bp = unreal.load_asset(bp_path)
        if not bp:
            print(f"    [SKIP] Could not load Blueprint")
            continue

        # Track dispatcher owner
        if bp_path == DISPATCHER_OWNER:
            dispatcher_owner_bp = bp

        # Diagnostic: find enum usage
        usages = bfl.find_enum_usage(bp, OLD_ENUM_PATH)
        if usages and len(usages) > 0:
            print(f"    [INFO] Found {len(usages)} enum usages")

        # Migrate the enum references (update all types, NO reconstruction)
        updated = bfl.migrate_enum_in_blueprint(bp, OLD_ENUM_PATH, NEW_ENUM_PATH, DRY_RUN)
        total_updated += updated

        if updated > 0:
            print(f"    [{'WOULD UPDATE' if DRY_RUN else 'UPDATED'}] {updated} items")
        else:
            print(f"    [OK] Pins updated (no definition changes)")

        blueprints_loaded[bp_path] = (bp, updated)

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
    # PASS 2: Compile B_Stat FIRST (dispatcher owner)
    # =============================================================================
    print(f"\n[PASS 2] Compiling dispatcher owner: {DISPATCHER_OWNER}")
    print("         This creates the delegate with C++ enum signature")

    if dispatcher_owner_bp:
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(dispatcher_owner_bp)
            print(f"    [OK] B_Stat compiled - dispatcher now has C++ enum signature")
        except Exception as e:
            print(f"    [ERROR] Failed to compile B_Stat: {e}")
            raise
    else:
        print(f"    [WARN] B_Stat not loaded, skipping")

    # =============================================================================
    # PASS 3: RefreshAllNodes on blueprints with delegate-bound events
    # =============================================================================
    print(f"\n[PASS 3] Refreshing nodes in blueprints with delegate-bound events...")
    print("         Bound events will pick up B_Stat's new dispatcher signature")

    for bp_path in BLUEPRINTS_WITH_BOUND_EVENTS:
        if bp_path not in blueprints_loaded:
            continue

        bp, _ = blueprints_loaded[bp_path]
        print(f"\n  RefreshAllNodes: {bp_path}")
        try:
            bfl.reconstruct_all_nodes(bp)
            print(f"    [OK] All nodes refreshed")
        except Exception as e:
            print(f"    [ERROR] {e}")

    # =============================================================================
    # PASS 4: Compile all blueprints
    # =============================================================================
    print(f"\n[PASS 4] Compiling all blueprints...")

    compile_errors = []

    for bp_path, (bp, updated) in blueprints_loaded.items():
        if bp_path == DISPATCHER_OWNER:
            continue  # Already compiled

        print(f"\n  Compiling: {bp_path}")
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            print(f"    [OK] Compiled successfully")
        except Exception as e:
            print(f"    [ERROR] Compile failed: {e}")
            compile_errors.append((bp_path, str(e)))

    if compile_errors:
        print(f"\n  [WARN] {len(compile_errors)} blueprints had compile errors:")
        for bp_path, error in compile_errors:
            print(f"    - {bp_path}: {error}")

    # =============================================================================
    # PASS 5: Save ALL blueprints
    # =============================================================================
    print(f"\n[PASS 5] Saving all blueprints...")

    for bp_path, (bp, updated) in blueprints_loaded.items():
        print(f"\n  Saving: {bp_path}")
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print(f"    [OK] Saved")
        except Exception as e:
            print(f"    [ERROR] Save failed: {e}")

# =============================================================================
# Summary
# =============================================================================
print("\n" + "=" * 70)
print("  MIGRATION SUMMARY")
print("=" * 70)
print(f"""
Mode: {'DRY RUN' if DRY_RUN else 'ACTUAL MIGRATION'}
Approach: Hybrid (direct update + RefreshAllNodes for bound events)

Blueprints processed: {len(blueprints_loaded)}
Total items updated: {total_updated}
""")

if not DRY_RUN:
    print("""
Next steps:
1. Check for any compilation errors above
2. If no errors, delete the Blueprint E_ValueType enum:
   - In Content Browser, navigate to /Game/SoulslikeFramework/Enums/
   - Delete E_ValueType
3. Compile all Blueprints
4. Test in Play mode

If there are errors, restore and investigate:
    rm -rf "C:/scripts/SLFConversion/Content"
    cp -r "C:/scripts/SLFConversion_Migration/Backups/pre_enum_migration_20251229_132227/Content" "C:/scripts/SLFConversion/"
""")
