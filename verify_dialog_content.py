"""Verify dialog content and GameplayEvents."""
import unreal

dialog_tables = [
    "/Game/SoulslikeFramework/Data/Dialog/DT_GenericDefaultDialog",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DT_ShowcaseVendorNpc_Generic",
]

unreal.log_warning("=" * 70)
unreal.log_warning("DIALOG CONTENT VERIFICATION")
unreal.log_warning("=" * 70)

for dt_path in dialog_tables:
    table_name = dt_path.split('/')[-1]
    try:
        dt = unreal.EditorAssetLibrary.load_asset(dt_path)
        if not dt:
            unreal.log_warning(f"{table_name}: FAILED TO LOAD")
            continue

        unreal.log_warning(f"")
        unreal.log_warning(f"=== {table_name} ===")

        row_names = dt.get_row_names()
        for row_name in row_names:
            # Get the row using find_row (returns the row struct)
            # Since we can't directly access struct fields from Python,
            # we use the C++ function via the automation library
            unreal.log_warning(f"  Row: {row_name}")

    except Exception as e:
        unreal.log_warning(f"{table_name}: ERROR - {e}")

# Test reading a specific row using a C++ function
unreal.log_warning("")
unreal.log_warning("=" * 70)
unreal.log_warning("Testing GetDialogEntryText function...")

# Check if the function exists
if hasattr(unreal.SLFAutomationLibrary, 'get_dialog_entry_text'):
    text = unreal.SLFAutomationLibrary.get_dialog_entry_text(
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress",
        "NewRow"
    )
    unreal.log_warning(f"NoProgress Row0: {text}")
else:
    unreal.log_warning("get_dialog_entry_text function not available")
    unreal.log_warning("(Would need to add this function to SLFAutomationLibrary)")

unreal.log_warning("")
unreal.log_warning("=" * 70)
unreal.log_warning("VERIFICATION COMPLETE")
unreal.log_warning("=" * 70)
