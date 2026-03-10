"""Verify dialog DataTables use C++ struct."""
import unreal

dialog_tables = [
    "/Game/SoulslikeFramework/Data/Dialog/DT_GenericDefaultDialog",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DT_ShowcaseVendorNpc_Generic",
]

unreal.log_warning("=" * 70)
unreal.log_warning("DIALOG DATATABLE VERIFICATION")
unreal.log_warning("=" * 70)

for dt_path in dialog_tables:
    try:
        dt = unreal.EditorAssetLibrary.load_asset(dt_path)
        if dt:
            row_struct = dt.get_editor_property("RowStruct")
            struct_path = row_struct.get_path_name() if row_struct else "None"
            is_cpp = "/Script/" in struct_path if struct_path else False
            row_names = dt.get_row_names()

            unreal.log_warning(f"")
            unreal.log_warning(f"{dt_path.split('/')[-1]}:")
            unreal.log_warning(f"  RowStruct: {struct_path}")
            unreal.log_warning(f"  Is C++: {'YES' if is_cpp else 'NO'}")
            unreal.log_warning(f"  Row count: {len(row_names)}")
        else:
            unreal.log_warning(f"{dt_path}: FAILED TO LOAD")
    except Exception as e:
        unreal.log_warning(f"{dt_path}: ERROR - {e}")

unreal.log_warning("")
unreal.log_warning("=" * 70)
