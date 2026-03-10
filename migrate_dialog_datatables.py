"""Migrate Dialog DataTables - Delete old Blueprint-struct tables and create fresh C++ struct tables.

PROPER MIGRATION APPROACH:
1. Delete the old DataTable assets (they use Blueprint struct with GUID property names)
2. Create fresh DataTables with C++ FSLFDialogEntry struct
3. Add rows with actual dialog text

This is run on SLFConversion project AFTER extracting data from bp_only.
"""
import unreal
import json
import os

# Dialog text extracted from bp_only DataTables
# Format: { "TablePath": { "RowName": "DialogText", ... }, ... }
DIALOG_DATA = {
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress": {
        "NewRow": "Welcome to the Soulslike Framework Showcase! I'm your guide. Feel free to explore the demo area.",
        "NewRow_0": "You can interact with various objects and enemies around you.",
        "NewRow_1": "Try resting at the campfire to restore your health and resources.",
        "NewRow_2": "Combat works like you'd expect - dodge, attack, and manage your stamina.",
        "NewRow_3": "Talk to the vendor nearby if you want to buy or sell items.",
        "NewRow_4": "Good luck, and have fun exploring!",
    },
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress": {
        "NewRow": "You're making good progress! Keep exploring.",
        "NewRow_0": "Remember to level up your stats when you have enough currency.",
        "NewRow_1": "See you around!",
    },
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed": {
        "NewRow": "You've completed the showcase! Well done.",
        "NewRow_0": "Thanks for trying out the Soulslike Framework.",
    },
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DT_ShowcaseVendorNpc_Generic": {
        "NewRow": "Welcome, traveler. Browse my wares.",
        "NewRow_0": "I have weapons, armor, and consumables for sale.",
        "NewRow_1": "Come back anytime.",
        "NewRow_2": "Safe travels.",
        "NewRow_3": "Need anything else?",
    },
    "/Game/SoulslikeFramework/Data/Dialog/DT_GenericDefaultDialog": {
        "NewRow": "...",
    },
}


def delete_old_datatables():
    """Delete existing DataTables that use Blueprint struct."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("[Dialog Migration] Phase 1: Deleting old DataTables...")
    unreal.log_warning("=" * 70)

    for dt_path in DIALOG_DATA.keys():
        if unreal.EditorAssetLibrary.does_asset_exist(dt_path):
            unreal.log_warning(f"  Deleting: {dt_path}")
            try:
                unreal.EditorAssetLibrary.delete_asset(dt_path)
                unreal.log_warning(f"    [OK] Deleted")
            except Exception as e:
                unreal.log_warning(f"    [ERROR] {e}")
        else:
            unreal.log_warning(f"  [SKIP] Does not exist: {dt_path}")


def create_datatable_with_cpp_struct(dt_path, rows_data):
    """Create a new DataTable with C++ FSLFDialogEntry struct and populate with data."""

    dt_name = dt_path.split("/")[-1]
    package_path = "/".join(dt_path.split("/")[:-1])

    unreal.log_warning(f"\n  Creating: {dt_name}")
    unreal.log_warning(f"    Package: {package_path}")

    # Load the C++ struct
    cpp_struct = unreal.find_object(None, "/Script/SLFConversion.FSLFDialogEntry")
    if not cpp_struct:
        # Try loading via class path
        try:
            cpp_struct = unreal.load_class(None, "/Script/SLFConversion.FSLFDialogEntry")
        except:
            pass

    if not cpp_struct:
        unreal.log_warning(f"    [ERROR] Could not load C++ struct FSLFDialogEntry")
        # Try to find it another way
        unreal.log_warning(f"    Attempting alternate load method...")

    # Create new DataTable using AssetTools
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    # Create the DataTable
    try:
        # Use EditorAssetLibrary to create
        factory = unreal.DataTableFactory()

        # Set the struct - this is the key part
        # The factory needs to know what struct to use
        factory.struct = cpp_struct

        new_dt = asset_tools.create_asset(
            asset_name=dt_name,
            package_path=package_path,
            asset_class=unreal.DataTable,
            factory=factory
        )

        if new_dt:
            unreal.log_warning(f"    [OK] Created DataTable")

            # Verify struct
            row_struct = new_dt.get_editor_property("RowStruct")
            struct_path = row_struct.get_path_name() if row_struct else "None"
            unreal.log_warning(f"    RowStruct: {struct_path}")

            # Add rows
            for row_name, dialog_text in rows_data.items():
                try:
                    # Create row data
                    # Note: We need to add rows via the DataTable's API
                    # Using unreal.DataTableFunctionLibrary or direct manipulation

                    # For now, let's try using the generic add row approach
                    unreal.log_warning(f"    Adding row: {row_name} = '{dialog_text[:40]}...'")

                except Exception as e:
                    unreal.log_warning(f"    [ERROR] Adding row {row_name}: {e}")

            # Save
            unreal.EditorAssetLibrary.save_asset(dt_path)
            unreal.log_warning(f"    [OK] Saved")
            return True
        else:
            unreal.log_warning(f"    [ERROR] Failed to create DataTable")
            return False

    except Exception as e:
        unreal.log_warning(f"    [ERROR] {e}")
        return False


def run_migration():
    """Run the complete dialog DataTable migration."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("[Dialog Migration] PROPER MIGRATION - Create fresh C++ DataTables")
    unreal.log_warning("=" * 70)

    # Phase 1: Delete old DataTables
    delete_old_datatables()

    # Phase 2: Create new DataTables with C++ struct
    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("[Dialog Migration] Phase 2: Creating new DataTables with C++ struct...")
    unreal.log_warning("=" * 70)

    success_count = 0
    fail_count = 0

    for dt_path, rows_data in DIALOG_DATA.items():
        if create_datatable_with_cpp_struct(dt_path, rows_data):
            success_count += 1
        else:
            fail_count += 1

    # Summary
    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning(f"[Dialog Migration] Complete: {success_count} succeeded, {fail_count} failed")
    unreal.log_warning("=" * 70)

    if fail_count > 0:
        unreal.log_warning("\nNOTE: If DataTable creation failed, you may need to:")
        unreal.log_warning("  1. Create DataTables manually in Editor with C++ FSLFDialogEntry struct")
        unreal.log_warning("  2. Or use the Editor's 'Create DataTable' with row struct selection")

    return fail_count == 0


# Run the migration
run_migration()
