# migrate_fregen_fstatoverride.py
# Automated migration of FRegen and FStatOverride from Blueprint structs to C++ structs
#
# PREREQUISITES:
# 1. FRegen and FStatOverride are already defined in SLFStatTypes.h
# 2. BlueprintFixerLibrary is compiled and available
# 3. Run in Unreal Editor AFTER renaming:
#    - FRegen to FRegen_DEPRECATED
#    - FStatOverride to FStatOverride_DEPRECATED
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\migrate_fregen_fstatoverride.py").read())

import unreal

# ============================================================================
# CONFIGURATION - EXACT REFERENCES FROM USER
# ============================================================================

# C++ struct paths
CPP_FREGEN_PATH = "/Script/SLFConversion.FRegen"
CPP_FSTATOVERRIDE_PATH = "/Script/SLFConversion.FStatOverride"

# ============================================================================
# FREGEN REFERENCES (from Unreal reference finder):
# ============================================================================
# 1. /Script/Engine.Blueprint'/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager.AC_StatManager'
# 2. /Script/Engine.Blueprint'/Game/SoulslikeFramework/Data/Stats/B_Stat.B_Stat'
# 3. /Script/CoreUObject.UserDefinedStruct'/Game/SoulslikeFramework/Structures/Stats/FStatInfo_DEPRECATED.FStatInfo_DEPRECATED'
#    ^ STRUCT DEPENDENCY - FStatInfo contains FRegen, will be fixed when we migrate FStatInfo
# 4. /Script/CoreUObject.UserDefinedStruct'/Game/SoulslikeFramework/Structures/Stats/FStatOverride.FStatOverride'
#    ^ STRUCT DEPENDENCY - FStatOverride contains FRegen, will be fixed when FStatOverride is deprecated
# 5. /Script/UMGEditor.WidgetBlueprint'/Game/SoulslikeFramework/Widgets/_Debug/W_DebugWindow.W_DebugWindow'
# 6. /Script/UMGEditor.WidgetBlueprint'/Game/SoulslikeFramework/Widgets/Equipment/W_Equipment_Item_StatsGranted.W_Equipment_Item_StatsGranted'

FREGEN_ASSETS = [
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",      # Blueprint
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",                         # Blueprint
    "/Game/SoulslikeFramework/Widgets/_Debug/W_DebugWindow",              # WidgetBlueprint
    "/Game/SoulslikeFramework/Widgets/Equipment/W_Equipment_Item_StatsGranted",  # WidgetBlueprint
]
# Note: FStatInfo_DEPRECATED and FStatOverride struct references are handled separately

# ============================================================================
# FSTATOVERRIDE REFERENCES (from Unreal reference finder):
# ============================================================================
# 1. /Script/Engine.Blueprint'/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager.AC_StatManager'
# 2. /Script/UMGEditor.WidgetBlueprint'/Game/SoulslikeFramework/Widgets/_Debug/W_DebugWindow.W_DebugWindow'

FSTATOVERRIDE_ASSETS = [
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",      # Blueprint
    "/Game/SoulslikeFramework/Widgets/_Debug/W_DebugWindow",              # WidgetBlueprint
]

# Combined unique assets (4 total)
ALL_ASSETS = [
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",      # FRegen + FStatOverride
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",                         # FRegen only
    "/Game/SoulslikeFramework/Widgets/_Debug/W_DebugWindow",              # FRegen + FStatOverride
    "/Game/SoulslikeFramework/Widgets/Equipment/W_Equipment_Item_StatsGranted",  # FRegen only
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

    bp = unreal.Blueprint.cast(asset)
    if not bp:
        bp = unreal.WidgetBlueprint.cast(asset)
    return bp


def fix_asset(bp_path, fixer):
    """Fix FRegen and FStatOverride references in a single asset."""
    asset_name = bp_path.split("/")[-1]
    print(f"\n  --- {asset_name} ---")

    bp = load_blueprint(bp_path)
    if not bp:
        print(f"    [SKIP] Could not load asset")
        return {"loaded": False}

    result = {
        "loaded": True,
        "fregen_vars_fixed": 0,
        "fstatoverride_vars_fixed": 0,
        "nodes_reconstructed": False,
        "broken_nodes": [],
    }

    # Step 1: Check for FRegen variables and fix them
    print(f"    Checking variables for FRegen...")
    var_info = fixer.get_variable_type_info(bp)

    fregen_vars = [v for v in var_info if "FRegen" in str(v) and "DEPRECATED" in str(v)]
    if fregen_vars:
        print(f"    Found {len(fregen_vars)} FRegen variable(s) to fix:")
        for v in fregen_vars:
            print(f"      {v}")
            var_name = str(v).split(",")[0].replace("Name=", "").strip()
            if fixer.fix_variable_struct_type(bp, var_name, CPP_FREGEN_PATH):
                print(f"      -> Fixed!")
                result["fregen_vars_fixed"] += 1
            else:
                print(f"      -> Could not fix automatically")
    else:
        print(f"    No FRegen_DEPRECATED variables found")

    # Step 2: Check for FStatOverride variables and fix them
    print(f"    Checking variables for FStatOverride...")
    fstatoverride_vars = [v for v in var_info if "FStatOverride" in str(v) and "DEPRECATED" in str(v)]
    if fstatoverride_vars:
        print(f"    Found {len(fstatoverride_vars)} FStatOverride variable(s) to fix:")
        for v in fstatoverride_vars:
            print(f"      {v}")
            var_name = str(v).split(",")[0].replace("Name=", "").strip()
            if fixer.fix_variable_struct_type(bp, var_name, CPP_FSTATOVERRIDE_PATH):
                print(f"      -> Fixed!")
                result["fstatoverride_vars_fixed"] += 1
            else:
                print(f"      -> Could not fix automatically")
    else:
        print(f"    No FStatOverride_DEPRECATED variables found")

    # Step 3: Reconstruct all nodes to refresh struct references
    print(f"    Reconstructing nodes...")
    if fixer.reconstruct_all_nodes(bp):
        result["nodes_reconstructed"] = True
        print(f"    -> Nodes reconstructed")

    # Step 4: Check for remaining broken nodes
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

    # Step 5: Save the asset
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print(f"    Saved!")
    except Exception as e:
        print(f"    Save warning: {e}")

    return result


def main():
    print("\n" + "="*70)
    print("  FREGEN + FSTATOVERRIDE MIGRATION")
    print("="*70)
    print(f"\n  Migrating:")
    print(f"    - FRegen_DEPRECATED -> C++ FRegen")
    print(f"    - FStatOverride_DEPRECATED -> C++ FStatOverride")
    print(f"\n  Assets to process: {len(ALL_ASSETS)}")
    for a in ALL_ASSETS:
        print(f"    - {a.split('/')[-1]}")

    # Check prerequisites
    if not check_prerequisites():
        return

    fixer = unreal.BlueprintFixerLibrary

    # Process each asset
    results = {}
    total_fregen_fixed = 0
    total_fstatoverride_fixed = 0
    total_broken = 0

    print("\n" + "="*70)
    print("  PROCESSING ASSETS")
    print("="*70)

    for bp_path in ALL_ASSETS:
        result = fix_asset(bp_path, fixer)
        results[bp_path] = result
        total_fregen_fixed += result.get("fregen_vars_fixed", 0)
        total_fstatoverride_fixed += result.get("fstatoverride_vars_fixed", 0)
        total_broken += len(result.get("broken_nodes", []))

    # Summary
    print("\n" + "="*70)
    print("  SUMMARY")
    print("="*70)

    loaded = sum(1 for r in results.values() if r.get("loaded"))
    print(f"\n  Assets processed: {loaded}/{len(ALL_ASSETS)}")
    print(f"  FRegen variables fixed: {total_fregen_fixed}")
    print(f"  FStatOverride variables fixed: {total_fstatoverride_fixed}")
    print(f"  Broken nodes remaining: {total_broken}")

    if total_broken > 0:
        print("\n  Assets needing manual fixes:")
        for bp_path, result in results.items():
            if result.get("broken_nodes"):
                asset_name = bp_path.split("/")[-1]
                count = len(result["broken_nodes"])
                print(f"    [ ] {asset_name} - {count} broken node(s)")

    print("\n" + "="*70)
    print("  DETAILED VALIDATION CHECKLIST")
    print("="*70)
    print("""
  =========================================================================
  ASSET 1: AC_StatManager (FRegen + FStatOverride)
  =========================================================================

  FREGEN LOCATIONS TO CHECK:
  - Variables: Look for any variable of type FRegen (should now be C++ FRegen)
  - Search graph: "Break FRegen" - verify output pins match C++ struct
  - Search graph: "Make FRegen" - verify input pins match C++ struct
  - Look in: Any function that deals with stat regeneration

  FSTATOVERRIDE LOCATIONS TO CHECK:
  - Variables: "StatOverrides" TMap<FGameplayTag, FStatOverride>
  - Search graph: "Break FStatOverride" - verify output pins (especially RegenOverride pin)
  - Search graph: "Make FStatOverride" - verify input pins
  - Look in: AdjustStat, GetStat, stat modification functions

  KEY TEST: Break FStatOverride -> RegenOverride output should connect to
            Make FStatInfo -> RegenInfo input (both C++ FRegen type)

  =========================================================================
  ASSET 2: B_Stat (FRegen ONLY)
  =========================================================================

  FREGEN LOCATIONS TO CHECK:
  - Look for "RegenInfo" property/variable
  - Search graph: "Break FRegen"
  - Search graph: "Make FRegen"
  - Look in: Tick function, regen logic, any timer-based stat recovery

  SPECIFIC NODES:
  - RegenInfo struct pins on Break/Make FStatInfo nodes
  - Any Set/Get for regen properties (bCanRegenerate, RegenPercent, RegenInterval)

  =========================================================================
  ASSET 3: W_DebugWindow (FRegen + FStatOverride)
  =========================================================================

  FREGEN LOCATIONS TO CHECK:
  - Search: "FRegen", "Regen"
  - Debug display showing regen info
  - Any text formatting of regen values

  FSTATOVERRIDE LOCATIONS TO CHECK:
  - Search: "FStatOverride", "Override"
  - Debug display showing stat overrides
  - Any text formatting of override values

  LOOK FOR: Debug text nodes that display struct member values

  =========================================================================
  ASSET 4: W_Equipment_Item_StatsGranted (FRegen ONLY)
  =========================================================================

  FREGEN LOCATIONS TO CHECK:
  - Search: "FRegen", "Regen"
  - Equipment stat display for regen bonuses
  - Item tooltip showing regen values

  LOOK FOR: Text blocks/formatting showing regen percentage or interval

  =========================================================================
  STRUCT DEPENDENCIES (NOT FIXED BY THIS SCRIPT):
  =========================================================================

  - FStatInfo_DEPRECATED contains FRegen -> Will fix when migrating FStatInfo
  - FStatOverride contains FRegen -> Already renamed to FStatOverride_DEPRECATED

  After this migration succeeds, run migrate_fstatinfo.py for the 27 assets.
""")
    print("="*70 + "\n")


if __name__ == "__main__" or True:
    main()
