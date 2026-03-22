"""
Fix DA_ExampleDialog by setting the correct DefaultDialogTable
"""
import unreal

# Load DA_ExampleDialog
dialog_asset_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
dialog_asset = unreal.load_asset(dialog_asset_path)

if not dialog_asset:
    print(f"ERROR: Failed to load dialog asset: {dialog_asset_path}")
else:
    print(f"Loaded: {dialog_asset.get_name()} (Class: {dialog_asset.get_class().get_name()})")

    # Load the correct DataTable
    correct_table_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress"
    correct_table = unreal.load_asset(correct_table_path)

    if correct_table:
        print(f"Loaded correct table: {correct_table.get_name()}")

        # Set the DefaultDialogTable property
        try:
            dialog_asset.set_editor_property("DefaultDialogTable", correct_table)
            print("Set DefaultDialogTable to DT_ShowcaseGuideNpc_NoProgress")

            # Save the asset
            if unreal.EditorAssetLibrary.save_asset(dialog_asset_path, only_if_is_dirty=False):
                print("Saved DA_ExampleDialog")
            else:
                print("WARNING: Failed to save DA_ExampleDialog")
        except Exception as e:
            print(f"ERROR setting property: {e}")
    else:
        print(f"ERROR: Failed to load table: {correct_table_path}")

# Verify
dialog_asset2 = unreal.load_asset(dialog_asset_path)
if dialog_asset2:
    dt_prop = dialog_asset2.get_editor_property("DefaultDialogTable")
    print(f"Verification - DefaultDialogTable: {dt_prop}")
