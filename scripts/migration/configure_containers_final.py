# configure_containers_final.py
# Configure containers to use the migrated C++ DataTable for loot
# Run after migrate_chest_tier_datatable.py

import unreal

LEVEL_PATH = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
LOOT_TABLE_PATH = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleChestTier.DT_ExampleChestTier"


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CONFIGURING CONTAINERS WITH MIGRATED LOOT TABLE")
    unreal.log_warning("=" * 70)

    # Load level
    unreal.log_warning(f"Loading level: {LEVEL_PATH}")
    success = unreal.EditorLoadingAndSavingUtils.load_map(LEVEL_PATH)
    if not success:
        unreal.log_error("Failed to load level")
        return

    # Use C++ function to configure all containers
    result = unreal.SLFAutomationLibrary.configure_containers_to_use_loot_table()
    unreal.log_warning(f"Configured {result} containers")

    # Diagnose to verify
    unreal.log_warning("\n--- VERIFYING CONTAINER CONFIGURATION ---")
    diagnosis = unreal.SLFAutomationLibrary.diagnose_container_loot()
    unreal.log_warning(diagnosis)

    unreal.log_warning("=" * 70)
    unreal.log_warning("CONFIGURATION COMPLETE")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
