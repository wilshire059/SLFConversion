"""
Fix DA_ExampleDialog and DA_ExampleDialog_Vendor DefaultDialogTable properties.
These got corrupted during migration and point to placeholder instead of real content.
"""
import unreal

# Fixes to apply
DIALOG_FIXES = {
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog": {
        "default_dialog_table": "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress"
    },
    # Vendor already has the correct table (DT_ShowcaseVendorNpc_Generic), leave it alone
}

def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("FIXING DIALOG TABLE REFERENCES")
    unreal.log_warning("="*70 + "\n")

    for asset_path, fixes in DIALOG_FIXES.items():
        asset_name = asset_path.split("/")[-1]
        unreal.log_warning(f"\n=== {asset_name} ===")

        # Load the dialog data asset
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            unreal.log_warning(f"  [FAIL] Could not load")
            continue

        unreal.log_warning(f"  [OK] Loaded")

        # Check current DefaultDialogTable
        try:
            current_table = asset.get_editor_property('default_dialog_table')
            if current_table:
                if hasattr(current_table, 'to_soft_object_path'):
                    current_path = str(current_table.to_soft_object_path())
                elif hasattr(current_table, 'get_path_name'):
                    current_path = current_table.get_path_name()
                else:
                    current_path = str(current_table)
                unreal.log_warning(f"  Current DefaultDialogTable: {current_path}")
            else:
                unreal.log_warning(f"  Current DefaultDialogTable: None")
        except Exception as e:
            unreal.log_warning(f"  Current DefaultDialogTable: Error - {e}")

        # Fix the DefaultDialogTable
        if "default_dialog_table" in fixes:
            new_table_path = fixes["default_dialog_table"]
            unreal.log_warning(f"  Fixing to: {new_table_path}")

            # Load the correct data table
            new_table = unreal.EditorAssetLibrary.load_asset(new_table_path)
            if new_table:
                unreal.log_warning(f"  [OK] Loaded new table: {new_table.get_name()}")

                # Set the property
                try:
                    # Create soft object reference to the table
                    soft_ref = unreal.SoftObjectPath(new_table_path + "." + new_table_path.split("/")[-1])
                    asset.set_editor_property('default_dialog_table', soft_ref)
                    unreal.log_warning(f"  [OK] Set DefaultDialogTable")

                    # Verify
                    verify = asset.get_editor_property('default_dialog_table')
                    if verify:
                        unreal.log_warning(f"  [OK] Verified: {verify}")

                except Exception as e:
                    unreal.log_warning(f"  [FAIL] Set error: {e}")

                    # Try alternative approach - set using DataTable directly
                    try:
                        asset.set_editor_property('default_dialog_table', new_table)
                        unreal.log_warning(f"  [OK] Set via direct assignment")
                    except Exception as e2:
                        unreal.log_warning(f"  [FAIL] Direct assignment error: {e2}")
            else:
                unreal.log_warning(f"  [FAIL] Could not load new table")

        # Save the asset
        try:
            if unreal.EditorAssetLibrary.save_asset(asset_path):
                unreal.log_warning(f"  [OK] Saved")
            else:
                unreal.log_warning(f"  [WARN] Save returned false")
        except Exception as e:
            unreal.log_warning(f"  [FAIL] Save error: {e}")

    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("COMPLETE")
    unreal.log_warning("="*70 + "\n")

if __name__ == "__main__":
    main()
