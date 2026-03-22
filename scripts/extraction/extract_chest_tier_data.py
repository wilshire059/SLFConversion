# extract_chest_tier_data.py
# Extract DT_ExampleChestTier data table contents from bp_only
# Run on bp_only project

import unreal
import json

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("EXTRACTING DT_ExampleChestTier DATA")
    unreal.log_warning("=" * 70)

    # Load the data table
    dt_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleChestTier"
    dt = unreal.load_asset(dt_path)

    if not dt:
        unreal.log_error(f"Failed to load data table: {dt_path}")
        return

    unreal.log_warning(f"Data table: {dt.get_name()}")
    unreal.log_warning(f"Class: {dt.get_class().get_name()}")

    # Get row names
    row_names = unreal.DataTableFunctionLibrary.get_data_table_row_names(dt)
    unreal.log_warning(f"Row count: {len(row_names)}")

    loot_data = {
        "table_path": dt_path,
        "rows": []
    }

    for row_name in row_names:
        unreal.log_warning(f"\nRow: {row_name}")

        row_data = {
            "name": str(row_name),
            "item": None,
            "weight": 0.0,
            "min_amount": 1,
            "max_amount": 1
        }

        # Try to get row data using reflection
        # The row struct is FWeightedLoot with Item (FLootItem) and Weight (double)
        try:
            # Export the data table to see all row data
            exported = dt.export_text()
            if exported:
                # Log the full export for analysis
                unreal.log_warning(f"Export text (first 3000 chars):")
                unreal.log_warning(exported[:3000])
        except Exception as e:
            unreal.log_warning(f"Export error: {e}")

        loot_data["rows"].append(row_data)

    # Save to JSON
    output_path = "C:/scripts/SLFConversion/migration_cache/chest_tier_data.json"
    with open(output_path, "w") as f:
        json.dump(loot_data, f, indent=2)

    unreal.log_warning(f"\n" + "=" * 70)
    unreal.log_warning(f"Saved chest tier data to: {output_path}")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
