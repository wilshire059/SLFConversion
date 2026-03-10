"""Extract dialog data from bp_only DataTables."""
import unreal
import json

dialog_tables = [
    "/Game/SoulslikeFramework/Data/Dialog/DT_GenericDefaultDialog",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DT_ShowcaseVendorNpc_Generic",
]

output_data = {}

unreal.log_warning("=" * 70)
unreal.log_warning("EXTRACTING DIALOG DATA FROM BP_ONLY")
unreal.log_warning("=" * 70)

for dt_path in dialog_tables:
    table_name = dt_path.split('/')[-1]
    try:
        dt = unreal.EditorAssetLibrary.load_asset(dt_path)
        if not dt:
            unreal.log_warning(f"{table_name}: FAILED TO LOAD")
            continue

        row_names = dt.get_row_names()
        unreal.log_warning(f"")
        unreal.log_warning(f"{table_name}: {len(row_names)} rows")

        table_data = {"rows": []}

        for row_name in row_names:
            # Use export_text to get all row data
            row_export = unreal.DataTableFunctionLibrary.get_data_table_row_from_name(dt, row_name)

            # Try to get the Entry text
            entry_text = ""
            gameplay_events = []

            # Export the entire table as text to parse
            export_path = f"C:/scripts/SLFConversion/migration_cache/dialog_export_{table_name}.txt"

            row_data = {
                "row_name": str(row_name),
            }
            table_data["rows"].append(row_data)
            unreal.log_warning(f"  Row: {row_name}")

        output_data[table_name] = table_data

        # Export the full table as text
        exported = dt.export_text()
        if exported:
            export_file = f"C:/scripts/SLFConversion/migration_cache/dialog_export_{table_name}.txt"
            with open(export_file, 'w', encoding='utf-8') as f:
                f.write(exported)
            unreal.log_warning(f"  Exported to: {export_file}")

    except Exception as e:
        unreal.log_warning(f"{table_name}: ERROR - {e}")

# Save summary
with open("C:/scripts/SLFConversion/migration_cache/dialog_data_summary.json", 'w') as f:
    json.dump(output_data, f, indent=2)

unreal.log_warning("")
unreal.log_warning("=" * 70)
unreal.log_warning("EXTRACTION COMPLETE")
unreal.log_warning("=" * 70)
