# migrate_chest_tier_datatable.py
# Migrate DT_ExampleChestTier from Blueprint struct (FWeightedLoot) to C++ struct (FSLFWeightedLoot)
# Uses the C++ MigrateWeightedLootDataTable function for proper AAA-quality migration

import unreal

TABLE_PATH = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleChestTier"


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("DATATABLE MIGRATION: FWeightedLoot -> FSLFWeightedLoot")
    unreal.log_warning("=" * 70)

    # Call the C++ migration function
    result = unreal.SLFAutomationLibrary.migrate_weighted_loot_data_table(TABLE_PATH)

    unreal.log_warning(f"Migration result: {result}")

    unreal.log_warning("=" * 70)
    unreal.log_warning("MIGRATION COMPLETE")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
