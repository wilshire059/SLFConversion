import unreal

def run_global_migration():
    """
    Migrates 'Level_BP' and 'IsAiComponent_BP' to C++ equivalents across ALL referencing assets.
    """

    # 1. THE LIST OF ZOMBIE ASSETS
    # These are all the files referencing the old variables.
    target_assets = [
        "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
        "/Game/SoulslikeFramework/Widgets/_Debug/W_DebugWindow",
        "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelCurrencyBlock_LevelUp",
        "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelEntry",
        "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp",
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_PlayerCard",
        "/Game/SoulslikeFramework/Widgets/Status/W_Status"
    ]

    # 2. THE SWAPS
    mappings = [
        ("Level_BP", "Level"),
        ("IsAiComponent_BP", "bIsAiComponent"),
    ]

    automation_lib = unreal.SLFAutomationLibrary
    total_assets_dirty = 0

    print("--- STARTING GLOBAL MIGRATION ---")

    for asset_path in target_assets:
        # Load the asset
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            print(f"SKIPPING: Could not load {asset_path}")
            continue

        print(f"Processing: {asset.get_name()}...")

        modified_count = 0

        # Run swaps on this asset
        for old_name, new_name in mappings:
            # The C++ tool is smart enough to find external references (e.g. Target->Level_BP)
            count = automation_lib.advanced_force_replace(asset, old_name, new_name)
            if count > 0:
                print(f"  - Swapped '{old_name}' -> '{new_name}' ({count} nodes)")
                modified_count += count

        if modified_count > 0:
            # Save immediately to lock it in
            unreal.EditorAssetLibrary.save_loaded_asset(asset)
            print(f"  [SAVED] {asset.get_name()}")
            total_assets_dirty += 1
        else:
            print(f"  [CLEAN] No references found.")

    print("--- MIGRATION COMPLETE ---")
    print(f"Updated {total_assets_dirty} assets.")
    print("Now try searching for references or deleting the variables in AC_StatManager.")

if __name__ == "__main__":
    run_global_migration()
