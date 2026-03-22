"""
Reparent individual dialog data assets to C++ UPDA_Dialog.
Also clean up shadowing variables from the parent Blueprint.
"""
import unreal

# Individual dialog data assets to check/reparent
DIALOG_ASSETS = [
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor",
]

# Parent PDA Blueprints that need variable cleanup
PDA_BLUEPRINTS = {
    "/Game/SoulslikeFramework/Data/Dialog/PDA_Dialog": [
        "Requirement",
        "DefaultDialogTable",
    ],
    "/Game/SoulslikeFramework/Data/Vendor/PDA_Vendor": [
        "Items",
        "DefaultSlotCount",
        "DefaultSlotsPerRow",
        "CachedItems",
        "OnItemStockUpdated",
    ],
}

def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("REPARENTING AND CLEANING UP DIALOG ASSETS")
    unreal.log_warning("="*70 + "\n")

    # Step 1: Check dialog data asset parent classes
    unreal.log_warning("\n--- Checking Dialog Data Assets ---")
    for asset_path in DIALOG_ASSETS:
        asset_name = asset_path.split("/")[-1]
        unreal.log_warning(f"\n=== {asset_name} ===")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            unreal.log_warning(f"  [FAIL] Could not load")
            continue

        unreal.log_warning(f"  [OK] Loaded")
        unreal.log_warning(f"  Class: {asset.get_class().get_name()}")

        # These are data assets (instances), not Blueprint classes
        # They should inherit from the PDA_Dialog class which is now reparented to UPDA_Dialog
        # Check if it's a Blueprint data asset instance
        if hasattr(asset, 'get_class'):
            class_obj = asset.get_class()
            class_name = class_obj.get_name()

            # If still PDA_Dialog_C, the parent reparent worked and children inherit
            unreal.log_warning(f"  Class name: {class_name}")

            # Check DefaultDialogTable property
            try:
                default_table = asset.get_editor_property('default_dialog_table')
                if default_table:
                    if hasattr(default_table, 'to_soft_object_path'):
                        path = str(default_table.to_soft_object_path())
                    elif hasattr(default_table, 'get_path_name'):
                        path = default_table.get_path_name()
                    else:
                        path = str(default_table)
                    unreal.log_warning(f"  DefaultDialogTable: {path}")
                else:
                    unreal.log_warning(f"  DefaultDialogTable: None/Empty")
            except Exception as e:
                unreal.log_warning(f"  DefaultDialogTable: Error - {e}")

            # Check Requirement property
            try:
                requirements = asset.get_editor_property('requirement')
                if requirements:
                    unreal.log_warning(f"  Requirements: {len(requirements)} entries")
                else:
                    unreal.log_warning(f"  Requirements: None/Empty")
            except Exception as e:
                unreal.log_warning(f"  Requirements: Error - {e}")

    # Step 2: Clean up shadowing variables from PDA Blueprints
    unreal.log_warning("\n--- Cleaning Up Shadowing Variables ---")
    for bp_path, var_names in PDA_BLUEPRINTS.items():
        bp_name = bp_path.split("/")[-1]
        unreal.log_warning(f"\n=== {bp_name} ===")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"  [FAIL] Could not load Blueprint")
            continue

        unreal.log_warning(f"  [OK] Loaded Blueprint")

        # Try to remove each shadowing variable
        for var_name in var_names:
            try:
                result = unreal.SLFAutomationLibrary.remove_variable(bp, var_name)
                if result:
                    unreal.log_warning(f"  [OK] Removed variable: {var_name}")
                else:
                    unreal.log_warning(f"  [INFO] Variable not found or couldn't remove: {var_name}")
            except Exception as e:
                unreal.log_warning(f"  [FAIL] Error removing {var_name}: {e}")

        # Save the Blueprint
        try:
            result = unreal.SLFAutomationLibrary.compile_and_save(bp)
            if result:
                unreal.log_warning(f"  [OK] Compiled and saved")
            else:
                unreal.log_warning(f"  [WARN] Save may have issues")
        except Exception as e:
            unreal.log_warning(f"  [FAIL] Save error: {e}")

    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("COMPLETE")
    unreal.log_warning("="*70 + "\n")

if __name__ == "__main__":
    main()
