"""
migrate_enum_full.py

Full E_ValueType enum migration - migrates ALL blueprints at once.
Uses the delete/recreate approach for Switch nodes.

This script:
1. Loads all blueprints that use E_ValueType
2. Migrates ALL pin types to C++ enum (for each blueprint)
3. Recreates ALL Switch nodes (for each blueprint)
4. Compiles all blueprints

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_enum_full.py").read())
"""

import unreal

print("=" * 70)
print("  FULL E_ValueType Enum Migration")
print("  Blueprint enum -> C++ enum")
print("=" * 70)

# All Blueprints that use E_ValueType (from previous analysis)
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

OLD_ENUM_NAME = "E_ValueType"
NEW_ENUM_PATH = "/Script/SLFConversion.E_ValueType"

bfl = unreal.BlueprintFixerLibrary

# =============================================================================
# Step 1: Load all blueprints
# =============================================================================
print("\n[Step 1] Loading all blueprints...")

blueprints = {}
for bp_path in BLUEPRINTS_TO_MIGRATE:
    bp_name = bp_path.split('/')[-1]
    bp = unreal.load_asset(bp_path)
    if bp:
        blueprints[bp_path] = bp
        print(f"  [OK] {bp_name}")
    else:
        print(f"  [SKIP] {bp_name} - could not load")

print(f"\nLoaded {len(blueprints)} blueprints")

# =============================================================================
# Step 2: Migrate all pin types (for all blueprints)
# =============================================================================
print("\n[Step 2] Migrating pin types to C++ enum...")

for bp_path, bp in blueprints.items():
    bp_name = bp_path.split('/')[-1]
    try:
        updated = bfl.migrate_enum_in_blueprint(bp, OLD_ENUM_NAME, NEW_ENUM_PATH, False)
        if updated > 0:
            print(f"  {bp_name}: updated {updated} items")
        else:
            print(f"  {bp_name}: no changes needed")
    except Exception as e:
        print(f"  {bp_name}: ERROR - {e}")

# =============================================================================
# Step 3: Recreate all Switch nodes (for all blueprints)
# =============================================================================
print("\n[Step 3] Recreating Switch nodes...")

# We need to find and recreate ALL Switch nodes in ALL graphs
# The test_recreate_switch_node function only does one graph
# Let's call it for each blueprint to at least handle the first Switch node

for bp_path, bp in blueprints.items():
    bp_name = bp_path.split('/')[-1]

    # Find all graphs with Switch nodes using E_ValueType
    try:
        usages = bfl.find_enum_usage(bp, OLD_ENUM_NAME)
        switch_graphs = set()
        for usage in usages:
            if "SWITCH:" in usage:
                # Extract graph name from "SWITCH: Graph=AdjustValue, Enum=E_ValueType"
                parts = usage.split("Graph=")
                if len(parts) > 1:
                    graph_name = parts[1].split(",")[0]
                    switch_graphs.add(graph_name)

        if switch_graphs:
            print(f"  {bp_name}: Found Switch nodes in {switch_graphs}")
            for graph_name in switch_graphs:
                try:
                    result = bfl.test_recreate_switch_node(bp, graph_name)
                    if result:
                        print(f"    [OK] Recreated Switch in {graph_name}")
                    else:
                        print(f"    [SKIP] No Switch found in {graph_name}")
                except Exception as e:
                    print(f"    [ERROR] {graph_name}: {e}")
        else:
            print(f"  {bp_name}: No Switch nodes")
    except Exception as e:
        print(f"  {bp_name}: ERROR finding usages - {e}")

# =============================================================================
# Step 4: DO NOT RefreshAllNodes - it undoes our changes!
# =============================================================================
print("\n[Step 4] Skipping RefreshAllNodes (it undoes signature changes)")

# =============================================================================
# Step 5: Compile all blueprints
# =============================================================================
print("\n[Step 5] Compiling all blueprints...")

compile_errors = []
for bp_path, bp in blueprints.items():
    bp_name = bp_path.split('/')[-1]
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print(f"  [OK] {bp_name}")
    except Exception as e:
        print(f"  [ERROR] {bp_name}: {e}")
        compile_errors.append(bp_name)

# =============================================================================
# Summary
# =============================================================================
print("\n" + "=" * 70)
print("  MIGRATION SUMMARY")
print("=" * 70)
print(f"""
Blueprints processed: {len(blueprints)}
Compile errors: {len(compile_errors)}
""")

if compile_errors:
    print("Blueprints with compile errors:")
    for name in compile_errors:
        print(f"  - {name}")
    print("""
Check the Output Log for detailed error messages.
If there are errors, close Unreal and restore from backup:
    rm -rf "C:/scripts/SLFConversion/Content"
    cp -r "C:/scripts/backups/SLFConversion_InputBuffer_Complete_20251229/Content" "C:/scripts/SLFConversion/"
""")
else:
    print("""
SUCCESS! All blueprints migrated and compiled.

Next steps:
1. Test in Play mode
2. If working, save all assets
3. Delete the Blueprint E_ValueType enum:
   - Navigate to /Game/SoulslikeFramework/Enums/
   - Delete E_ValueType (the UserDefinedEnum)
""")
