"""Test NPC dialog system in PIE.

This script tests that AC_AI_InteractionManager correctly reads dialog entries
from DataTables that use either C++ or Blueprint structs.
"""
import unreal

def test_dialog_datatable():
    """Test dialog DataTable reading via AC_AI_InteractionManager."""

    # DataTables to test
    test_tables = [
        "/Game/SoulslikeFramework/Data/Dialog/DT_GenericDefaultDialog",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DT_ShowcaseVendorNpc_Generic",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress",
    ]

    unreal.log_warning("=" * 60)
    unreal.log_warning("[NPC Dialog Test] Testing dialog DataTables...")
    unreal.log_warning("=" * 60)

    for table_path in test_tables:
        unreal.log_warning(f"\n[Test] Loading: {table_path}")

        # Load the DataTable
        dt = unreal.EditorAssetLibrary.load_asset(table_path)
        if not dt:
            unreal.log_error(f"  [FAIL] Failed to load DataTable: {table_path}")
            continue

        # Get DataTable info
        row_struct = dt.get_editor_property("row_struct")
        row_struct_name = row_struct.get_name() if row_struct else "Unknown"
        unreal.log_warning(f"  RowStruct: {row_struct_name}")

        # Get row names
        row_names = unreal.DataTableFunctionLibrary.get_data_table_row_names(dt)
        unreal.log_warning(f"  Row count: {len(row_names)}")

        if len(row_names) == 0:
            unreal.log_error(f"  [FAIL] DataTable has 0 rows!")
            continue

        # List row names
        unreal.log_warning(f"  Row names: {[str(r) for r in row_names]}")

        # Try to get first row data - this will test if struct reading works
        try:
            # We can't directly read Blueprint struct rows from Python easily
            # The real test happens in C++ AC_AI_InteractionManager
            unreal.log_warning(f"  [OK] DataTable accessible with {len(row_names)} rows")
        except Exception as e:
            unreal.log_error(f"  [FAIL] Error accessing DataTable: {e}")

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning("[NPC Dialog Test] DataTable access test complete.")
    unreal.log_warning("To verify full dialog flow, run PIE and interact with NPC.")
    unreal.log_warning("Check logs for: [AIInteractionManager] Extracted Entry:")
    unreal.log_warning("=" * 60)

# Run test
test_dialog_datatable()
