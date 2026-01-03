# migrate_fstatinfo.py
# Automated migration of FStatInfo from Blueprint struct to C++ struct
#
# PREREQUISITES:
# 1. FStatInfo is already defined in SLFStatTypes.h
# 2. BlueprintFixerLibrary is compiled and available
# 3. Run in Unreal Editor AFTER renaming FStatInfo to FStatInfo_DEPRECATED
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\migrate_fstatinfo.py").read())

import unreal

# ============================================================================
# CONFIGURATION - ALL 27 ASSETS FROM REFERENCE FINDER
# ============================================================================

# C++ struct path for FStatInfo
CPP_FSTATINFO_PATH = "/Script/SLFConversion.FStatInfo"

# All 27 assets that reference FStatInfo (from Unreal reference finder)
ASSETS_TO_FIX = [
    # Blueprints (16)
    "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
    "/Game/SoulslikeFramework/Blueprints/BFL_Helper",
    "/Game/SoulslikeFramework/Blueprints/BML_StructConversion",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_Boss",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Dodge",
    "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_DrinkFlask_HP",
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect",
    "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_ShowcaseEnemy_StatDisplay",

    # Widgets (11)
    "/Game/SoulslikeFramework/Widgets/_Debug/W_DebugWindow",
    "/Game/SoulslikeFramework/Widgets/Equipment/W_Equipment_Item_RequiredStats",
    "/Game/SoulslikeFramework/Widgets/Equipment/W_Equipment_Item_StatsGranted",
    "/Game/SoulslikeFramework/Widgets/HUD/W_Boss_Healthbar",
    "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
    "/Game/SoulslikeFramework/Widgets/HUD/W_Resources",
    "/Game/SoulslikeFramework/Widgets/MainMenu/W_CharacterSelectionCard",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_StatEntry_LevelUp",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry",
    "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_Status",
]


def check_prerequisites():
    """Check if BlueprintFixerLibrary is available."""
    print("\n" + "="*70)
    print("  CHECKING PREREQUISITES")
    print("="*70)

    try:
        fixer = unreal.BlueprintFixerLibrary

        required_funcs = [
            'fix_variable_struct_type',
            'reconstruct_all_nodes',
            'get_broken_node_guids',
            'get_variable_type_info',
        ]

        all_ok = True
        for func_name in required_funcs:
            if hasattr(fixer, func_name):
                print(f"  [OK] {func_name}")
            else:
                print(f"  [MISSING] {func_name}")
                all_ok = False

        if not all_ok:
            print("\n  ERROR: Some C++ functions not available!")
            print("  Please rebuild C++ and restart the editor.")
            return False

        return True

    except AttributeError:
        print("  ERROR: BlueprintFixerLibrary not found!")
        return False


def load_blueprint(bp_path):
    """Load a Blueprint or Widget Blueprint asset."""
    asset = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not asset:
        return None

    # Try Blueprint first, then WidgetBlueprint
    bp = unreal.Blueprint.cast(asset)
    if not bp:
        bp = unreal.WidgetBlueprint.cast(asset)
    return bp


def fix_asset(bp_path, fixer):
    """Fix FStatInfo references in a single asset."""
    asset_name = bp_path.split("/")[-1]
    print(f"\n  --- {asset_name} ---")

    bp = load_blueprint(bp_path)
    if not bp:
        print(f"    [SKIP] Could not load asset")
        return {"loaded": False}

    result = {
        "loaded": True,
        "variables_fixed": 0,
        "nodes_reconstructed": False,
        "broken_nodes": [],
    }

    # Step 1: Check for FStatInfo variables and fix them
    print(f"    Checking variables...")
    var_info = fixer.get_variable_type_info(bp)
    fstatinfo_vars = [v for v in var_info if "FStatInfo" in str(v) and "DEPRECATED" in str(v)]

    if fstatinfo_vars:
        print(f"    Found {len(fstatinfo_vars)} FStatInfo variable(s) to fix:")
        for v in fstatinfo_vars:
            print(f"      {v}")
            # Extract variable name from the info string
            var_name = str(v).split(":")[0].strip()
            # Remove "Name=" prefix if present
            if var_name.startswith("Name="):
                var_name = var_name[5:]
            if fixer.fix_variable_struct_type(bp, var_name, CPP_FSTATINFO_PATH):
                print(f"      -> Fixed!")
                result["variables_fixed"] += 1
            else:
                print(f"      -> Could not fix automatically")
    else:
        print(f"    No FStatInfo variables with DEPRECATED reference found")

    # Step 2: Reconstruct all nodes to refresh struct references
    print(f"    Reconstructing nodes...")
    if fixer.reconstruct_all_nodes(bp):
        result["nodes_reconstructed"] = True
        print(f"    -> Nodes reconstructed")

    # Step 3: Check for remaining broken nodes
    print(f"    Checking for broken nodes...")
    broken = fixer.get_broken_node_guids(bp)
    if broken:
        result["broken_nodes"] = list(broken)
        print(f"    Found {len(broken)} broken node(s):")
        for node_info in broken[:5]:
            print(f"      {node_info}")
        if len(broken) > 5:
            print(f"      ... and {len(broken) - 5} more")
    else:
        print(f"    No broken nodes!")

    # Step 4: Save the asset
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print(f"    Saved!")
    except Exception as e:
        print(f"    Save warning: {e}")

    return result


def main():
    print("\n" + "="*70)
    print("  FSTATINFO AUTOMATED MIGRATION - ALL 27 ASSETS")
    print("="*70)
    print(f"\n  Target: Migrate FStatInfo_DEPRECATED -> C++ FStatInfo")
    print(f"  C++ Path: {CPP_FSTATINFO_PATH}")
    print(f"  Assets to fix: {len(ASSETS_TO_FIX)}")

    # Check prerequisites
    if not check_prerequisites():
        return

    fixer = unreal.BlueprintFixerLibrary

    # Process each asset
    results = {}
    total_vars_fixed = 0
    total_broken = 0

    print("\n" + "="*70)
    print("  PROCESSING ASSETS")
    print("="*70)

    for bp_path in ASSETS_TO_FIX:
        result = fix_asset(bp_path, fixer)
        results[bp_path] = result
        total_vars_fixed += result.get("variables_fixed", 0)
        total_broken += len(result.get("broken_nodes", []))

    # Summary
    print("\n" + "="*70)
    print("  SUMMARY")
    print("="*70)

    loaded = sum(1 for r in results.values() if r.get("loaded"))
    print(f"\n  Assets processed: {loaded}/{len(ASSETS_TO_FIX)}")
    print(f"  Variables fixed: {total_vars_fixed}")
    print(f"  Broken nodes remaining: {total_broken}")

    # List assets with broken nodes
    if total_broken > 0:
        print("\n  Assets needing manual fixes:")
        for bp_path, result in results.items():
            if result.get("broken_nodes"):
                asset_name = bp_path.split("/")[-1]
                count = len(result["broken_nodes"])
                print(f"    [ ] {asset_name} - {count} broken node(s)")

    print("\n" + "="*70)


if __name__ == "__main__" or True:
    main()
