"""
run_fix_all.py - Complete Variable Migration Script v13
Two-Phase Migration: Layer 3 TMap Types with USLFStatObject

PREREQUISITES - MUST BE DONE BEFORE RUNNING THIS SCRIPT:
1. Run reparent_b_stat.py first to reparent B_Stat to USLFStatObject
2. Recompile C++ after updating SoulslikeStatComponent.h TMaps

This script handles:
- Phase 1: Rename IsAiComponent -> IsAiComponent_BP
- Phase 2: Reparent AC_StatManager to SoulslikeStatComponent
- Phase 3: Copy default values from BP vars to C++ properties
- Phase 4: Migrate ALL variable references (including TMap vars)
- Phase 5: Delete old BP variables

v13: Two-phase migration with USLFStatObject TMap types
v12: UObject types - failed (type incompatibility)
v11: USLFStatObject types without reparenting B_Stat first - failed
v9: Basic migration without TMap vars - worked
"""

import unreal


def verify_prerequisites():
    """Verify that B_Stat has been reparented to USLFStatObject."""
    print("=" * 60)
    print("PHASE 0: Verify Prerequisites")
    print("=" * 60)

    b_stat_path = "/Game/SoulslikeFramework/Data/Stats/B_Stat"
    b_stat = unreal.EditorAssetLibrary.load_asset(b_stat_path)

    if not b_stat:
        print(f"ERROR: Could not load B_Stat at {b_stat_path}")
        return False

    # Check parent class
    try:
        # Get the generated class and its parent
        gen_class = b_stat.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            parent_name = parent.get_name() if parent else "None"
            print(f"B_Stat parent class: {parent_name}")

            if "SLFStatObject" in parent_name:
                print("[OK] B_Stat inherits from USLFStatObject")
                return True
            else:
                print("[ERROR] B_Stat does NOT inherit from USLFStatObject")
                print("Please run reparent_b_stat.py first!")
                return False
    except Exception as e:
        print(f"Warning: Could not verify parent class: {e}")
        print("Proceeding anyway - will fail later if B_Stat is not reparented")
        return True  # Allow proceeding, migration will fail if types don't match


def run_full_migration():
    """
    Complete migration workflow for AC_StatManager and dependent assets.
    """

    automation_lib = unreal.SLFAutomationLibrary

    print("=" * 60)
    print("COMPLETE MIGRATION WORKFLOW v13")
    print("=" * 60)

    # =========================================================================
    # PHASE 0: Verify prerequisites
    # =========================================================================
    if not verify_prerequisites():
        print("\n" + "=" * 60)
        print("MIGRATION ABORTED - Prerequisites not met")
        print("=" * 60)
        return

    # =========================================================================
    # PHASE 1: Rename variables in AC_StatManager
    # =========================================================================
    print("\n" + "=" * 60)
    print("PHASE 1: Rename ONLY differently-named variables (preserve instance data)")
    print("=" * 60)

    stat_manager_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"
    stat_manager = unreal.EditorAssetLibrary.load_asset(stat_manager_path)

    if not stat_manager:
        print(f"ERROR: Could not load {stat_manager_path}")
        return

    renames = [
        ("IsAiComponent", "IsAiComponent_BP"),
    ]

    print(f"\n>>> Renaming variables in {stat_manager.get_name()}...")
    for old_name, new_name in renames:
        result = automation_lib.rename_blueprint_variable(stat_manager, old_name, new_name)
        if result:
            print(f"    [RENAMED] '{old_name}' -> '{new_name}'")
        else:
            print(f"    [SKIP] '{old_name}' not found or already renamed")

    unreal.EditorAssetLibrary.save_loaded_asset(stat_manager)
    print(f"    [SAVED] {stat_manager.get_name()}")

    # =========================================================================
    # PHASE 2: Reparent AC_StatManager
    # =========================================================================
    print("\n" + "=" * 60)
    print("PHASE 2: Reparent to SoulslikeStatComponent")
    print("=" * 60)

    print(f"\n>>> Reparenting {stat_manager.get_name()}...")
    new_parent = unreal.find_object(None, "/Script/SLFConversion.SoulslikeStatComponent")
    if not new_parent:
        new_parent = unreal.load_class(None, "/Script/SLFConversion.SoulslikeStatComponent")

    if new_parent:
        result = automation_lib.reparent_blueprint(stat_manager, new_parent)
        if result:
            print(f"    [REPARENTED] to SoulslikeStatComponent")
        else:
            print(f"    [FAILED] Reparent failed")
            return
    else:
        print("    [ERROR] Could not find SoulslikeStatComponent class")
        return

    # =========================================================================
    # PHASE 3: Copy default values
    # =========================================================================
    print("\n" + "=" * 60)
    print("PHASE 3: Copy default values to C++ properties")
    print("=" * 60)

    stat_manager = unreal.EditorAssetLibrary.load_asset(stat_manager_path)

    default_copies = [
        ("Level", "Level"),
        ("IsAiComponent_BP", "bIsAiComponent"),
        ("StatTable", "StatTable"),
        ("SpeedAsset", "SpeedAsset"),
        ("SelectedClassAsset", "SelectedClassAsset"),
        ("Stats", "Stats"),
        ("ActiveStats", "ActiveStats"),
        ("StatOverrides", "StatOverrides"),
    ]

    print(f"\n>>> Copying default values...")
    for bp_name, cpp_name in default_copies:
        result = automation_lib.copy_variable_default_value(stat_manager, bp_name, cpp_name)
        if result:
            print(f"    [COPIED] '{bp_name}' -> '{cpp_name}'")
        else:
            print(f"    [SKIP] Could not copy '{bp_name}'")

    unreal.EditorAssetLibrary.save_loaded_asset(stat_manager)
    print(f"    [SAVED] {stat_manager.get_name()}")

    # =========================================================================
    # PHASE 4: Migrate ALL variable references
    # =========================================================================
    print("\n" + "=" * 60)
    print("PHASE 4: Migrate ALL variable references (including TMap)")
    print("=" * 60)

    target_assets = [
        "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
        "/Game/SoulslikeFramework/Widgets/_Debug/W_DebugWindow",
        "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelCurrencyBlock_LevelUp",
        "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelEntry",
        "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp",
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_PlayerCard",
        "/Game/SoulslikeFramework/Widgets/Status/W_Status"
    ]

    # ALL mappings including TMap variables
    all_mappings = [
        ("Level", "Level"),
        ("IsAiComponent_BP", "bIsAiComponent"),
        ("StatTable", "StatTable"),
        ("SpeedAsset", "SpeedAsset"),
        ("SelectedClassAsset", "SelectedClassAsset"),
        ("Stats", "Stats"),
        ("ActiveStats", "ActiveStats"),
        ("StatOverrides", "StatOverrides"),
    ]

    total_assets_dirty = 0

    for asset_path in target_assets:
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            print(f"\nSKIPPING: Could not load {asset_path}")
            continue

        print(f"\n>>> Processing: {asset.get_name()}")
        modified_count = 0

        for old_name, new_name in all_mappings:
            count = automation_lib.advanced_force_replace(asset, old_name, new_name)
            if count > 0:
                print(f"    [REPLACED] '{old_name}' -> '{new_name}' ({count} nodes)")
                modified_count += count

        if modified_count > 0:
            unreal.EditorAssetLibrary.save_loaded_asset(asset)
            print(f"    [SAVED] {modified_count} nodes modified")
            total_assets_dirty += 1
        else:
            print(f"    [CLEAN] No matching variable references found")

    # =========================================================================
    # PHASE 5: Delete old BP variables
    # =========================================================================
    print("\n" + "=" * 60)
    print("PHASE 5: Delete old BP variables")
    print("=" * 60)

    stat_manager = unreal.EditorAssetLibrary.load_asset(stat_manager_path)

    vars_to_delete = [
        "Level", "IsAiComponent_BP", "StatTable", "SpeedAsset", "SelectedClassAsset",
        "Stats", "ActiveStats", "StatOverrides"
    ]

    print(f"\n>>> Deleting old variables from {stat_manager.get_name()}...")
    for var_name in vars_to_delete:
        result = automation_lib.delete_blueprint_variable(stat_manager, var_name)
        if result:
            print(f"    [DELETED] '{var_name}'")
        else:
            print(f"    [SKIP] '{var_name}' not found or still in use")

    unreal.EditorAssetLibrary.save_loaded_asset(stat_manager)
    print(f"    [SAVED] {stat_manager.get_name()}")

    # =========================================================================
    # SUMMARY
    # =========================================================================
    print("\n" + "=" * 60)
    print("MIGRATION COMPLETE")
    print("=" * 60)
    print(f"Updated {total_assets_dirty} assets")
    print("")
    print("What was migrated:")
    print("  - AC_StatManager now inherits from SoulslikeStatComponent")
    print("  - B_Stat now inherits from USLFStatObject")
    print("")
    print("C++ Properties (using USLFStatObject for TMap compatibility):")
    print("  - Level, bIsAiComponent, StatTable, SpeedAsset, SelectedClassAsset")
    print("  - Stats (TMap<TSubclassOf<USLFStatObject>, FGameplayTag>)")
    print("  - ActiveStats (TMap<FGameplayTag, USLFStatObject*>)")
    print("  - StatOverrides (TMap<FGameplayTag, FSLFStatOverride>)")
    print("")
    print("Verification:")
    print("1. Open AC_StatManager - should inherit from SoulslikeStatComponent")
    print("2. Compile all Blueprints")
    print("3. Play in Editor to test")


if __name__ == "__main__":
    run_full_migration()
