"""
Migrate Dialog DataTables using C++ SLFAutomationLibrary.
This is the AAA solution - uses proper C++ APIs to set RowStruct.
"""
import unreal

def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("CALLING C++ MigrateAllDialogDataTables()")
    unreal.log_warning("="*70 + "\n")

    # Call the C++ function
    result = unreal.SLFAutomationLibrary.migrate_all_dialog_data_tables()

    unreal.log_warning("\n=== C++ MIGRATION RESULT ===")
    unreal.log_warning(result)

    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("MIGRATION SCRIPT COMPLETE")
    unreal.log_warning("="*70 + "\n")

if __name__ == "__main__":
    main()
