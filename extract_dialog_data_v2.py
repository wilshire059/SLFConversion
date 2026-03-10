"""Extract dialog data from bp_only DataTables using export."""
import unreal
import os

dialog_tables = [
    "/Game/SoulslikeFramework/Data/Dialog/DT_GenericDefaultDialog",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DT_ShowcaseVendorNpc_Generic",
]

output_dir = "C:/scripts/SLFConversion/migration_cache"

unreal.log_warning("=" * 70)
unreal.log_warning("EXTRACTING DIALOG DATA FROM BP_ONLY (V2)")
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

        # Try to export as JSON
        json_path = f"{output_dir}/{table_name}.json"
        csv_path = f"{output_dir}/{table_name}.csv"

        # Export to JSON
        success = unreal.DataTableFunctionLibrary.export_data_table_to_json_file(dt, json_path)
        if success:
            unreal.log_warning(f"  JSON export: {json_path}")
        else:
            unreal.log_warning(f"  JSON export FAILED")

        # Export to CSV
        success2 = unreal.DataTableFunctionLibrary.export_data_table_to_csv_file(dt, csv_path)
        if success2:
            unreal.log_warning(f"  CSV export: {csv_path}")
        else:
            unreal.log_warning(f"  CSV export FAILED")

    except Exception as e:
        unreal.log_warning(f"{table_name}: ERROR - {e}")

unreal.log_warning("")
unreal.log_warning("=" * 70)
unreal.log_warning("EXTRACTION COMPLETE")
unreal.log_warning("=" * 70)
