# fix_container_loot.py
# Configure containers to use LootTable (random items) instead of OverrideItem (fixed items)
# This matches bp_only behavior where containers have random loot from DT_ExampleChestTier

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("FIX CONTAINER LOOT CONFIGURATION")
    unreal.log_warning("=" * 70)

    # Load the demo level first
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    unreal.log_warning(f"Loading level: {level_path}")

    success = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
    if not success:
        unreal.log_error(f"Failed to load level: {level_path}")
        return

    unreal.log_warning("Level loaded successfully")

    # First diagnose current state
    unreal.log_warning("\n--- BEFORE FIX ---")
    diagnosis = unreal.SLFAutomationLibrary.diagnose_container_loot()
    unreal.log_warning(diagnosis)

    # Configure all containers to use LootTable
    unreal.log_warning("\n--- APPLYING FIX ---")
    fixed_count = unreal.SLFAutomationLibrary.configure_containers_to_use_loot_table()

    # Diagnose after fix
    unreal.log_warning("\n--- AFTER FIX ---")
    diagnosis = unreal.SLFAutomationLibrary.diagnose_container_loot()
    unreal.log_warning(diagnosis)

    unreal.log_warning("=" * 70)
    unreal.log_warning(f"Fixed {fixed_count} containers to use LootTable")
    unreal.log_warning("Containers will now drop random items from DT_ExampleChestTier")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
