"""
Diagnose NPC dialog configuration in SLFConversion.
Check if DA_ExampleDialog is properly configured and accessible.
"""
import unreal
import json
import os

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/npc_dialog_diagnostic.json"

def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("DIAGNOSING NPC DIALOG CONFIGURATION")
    unreal.log_warning("="*70 + "\n")

    data = {"diagnosis": {}}

    # Check DA_ExampleDialog
    dialog_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
    unreal.log_warning(f"\n=== Checking {dialog_path} ===")

    dialog_asset = unreal.EditorAssetLibrary.load_asset(dialog_path)
    if dialog_asset:
        unreal.log_warning(f"  [OK] Asset loaded")
        unreal.log_warning(f"  Class: {dialog_asset.get_class().get_name()}")
        unreal.log_warning(f"  Full path: {dialog_asset.get_path_name()}")

        data["diagnosis"]["DA_ExampleDialog"] = {
            "loaded": True,
            "class": dialog_asset.get_class().get_name(),
            "path": dialog_asset.get_path_name()
        }

        # Check parent class
        parent_class = dialog_asset.get_class().get_super_class()
        if parent_class:
            unreal.log_warning(f"  Parent class: {parent_class.get_name()}")
            data["diagnosis"]["DA_ExampleDialog"]["parent_class"] = parent_class.get_name()

        # Try to get DefaultDialogTable property
        try:
            default_table = dialog_asset.get_editor_property('default_dialog_table')
            if default_table:
                if hasattr(default_table, 'get_path_name'):
                    path = default_table.get_path_name()
                    unreal.log_warning(f"  DefaultDialogTable: {path}")
                    data["diagnosis"]["DA_ExampleDialog"]["default_dialog_table"] = path
                elif hasattr(default_table, 'to_soft_object_path'):
                    path = str(default_table.to_soft_object_path())
                    unreal.log_warning(f"  DefaultDialogTable (soft): {path}")
                    data["diagnosis"]["DA_ExampleDialog"]["default_dialog_table"] = path
                else:
                    unreal.log_warning(f"  DefaultDialogTable: {default_table}")
                    data["diagnosis"]["DA_ExampleDialog"]["default_dialog_table"] = str(default_table)
            else:
                unreal.log_warning(f"  DefaultDialogTable: None/Empty")
                data["diagnosis"]["DA_ExampleDialog"]["default_dialog_table"] = None
        except Exception as e:
            unreal.log_warning(f"  DefaultDialogTable: Error - {e}")
            data["diagnosis"]["DA_ExampleDialog"]["default_dialog_table_error"] = str(e)

        # Try to get Requirement property
        try:
            requirements = dialog_asset.get_editor_property('requirement')
            if requirements:
                unreal.log_warning(f"  Requirements: {len(requirements)} entries")
                data["diagnosis"]["DA_ExampleDialog"]["requirements_count"] = len(requirements)
            else:
                unreal.log_warning(f"  Requirements: Empty/None")
                data["diagnosis"]["DA_ExampleDialog"]["requirements_count"] = 0
        except Exception as e:
            unreal.log_warning(f"  Requirements: Error - {e}")
            data["diagnosis"]["DA_ExampleDialog"]["requirements_error"] = str(e)

    else:
        unreal.log_warning(f"  [FAIL] Could not load asset")
        data["diagnosis"]["DA_ExampleDialog"] = {"loaded": False}

    # Check DA_ExampleDialog_Vendor
    vendor_dialog_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor"
    unreal.log_warning(f"\n=== Checking {vendor_dialog_path} ===")

    vendor_dialog = unreal.EditorAssetLibrary.load_asset(vendor_dialog_path)
    if vendor_dialog:
        unreal.log_warning(f"  [OK] Asset loaded")
        unreal.log_warning(f"  Class: {vendor_dialog.get_class().get_name()}")
        data["diagnosis"]["DA_ExampleDialog_Vendor"] = {
            "loaded": True,
            "class": vendor_dialog.get_class().get_name()
        }
    else:
        unreal.log_warning(f"  [FAIL] Could not load")
        data["diagnosis"]["DA_ExampleDialog_Vendor"] = {"loaded": False}

    # Check the NPC Blueprint
    npc_guide_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide"
    unreal.log_warning(f"\n=== Checking NPC Blueprint ===")
    unreal.log_warning(f"  Path: {npc_guide_path}")

    npc_bp = unreal.EditorAssetLibrary.load_asset(npc_guide_path)
    if npc_bp:
        unreal.log_warning(f"  [OK] Blueprint loaded")

        gen_class = npc_bp.generated_class()
        if gen_class:
            unreal.log_warning(f"  Generated class: {gen_class.get_name()}")

            parent = gen_class.get_super_class()
            if parent:
                unreal.log_warning(f"  Parent class: {parent.get_name()}")
                data["diagnosis"]["B_Soulslike_NPC_ShowcaseGuide"] = {
                    "loaded": True,
                    "generated_class": gen_class.get_name(),
                    "parent_class": parent.get_name()
                }

            # Check CDO properties
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Check DefaultDialogAsset property (from C++)
                try:
                    default_dialog = cdo.get_editor_property('default_dialog_asset')
                    if default_dialog:
                        if hasattr(default_dialog, 'get_path_name'):
                            path = default_dialog.get_path_name()
                            unreal.log_warning(f"  CDO DefaultDialogAsset: {path}")
                            data["diagnosis"]["B_Soulslike_NPC_ShowcaseGuide"]["default_dialog_asset"] = path
                        else:
                            path = str(default_dialog)
                            unreal.log_warning(f"  CDO DefaultDialogAsset (str): {path}")
                            data["diagnosis"]["B_Soulslike_NPC_ShowcaseGuide"]["default_dialog_asset"] = path
                    else:
                        unreal.log_warning(f"  CDO DefaultDialogAsset: None/Empty")
                        data["diagnosis"]["B_Soulslike_NPC_ShowcaseGuide"]["default_dialog_asset"] = None
                except Exception as e:
                    unreal.log_warning(f"  CDO DefaultDialogAsset: Error - {e}")
                    data["diagnosis"]["B_Soulslike_NPC_ShowcaseGuide"]["default_dialog_asset_error"] = str(e)

                # Check NPCDisplayName property
                try:
                    npc_name = cdo.get_editor_property('npc_display_name')
                    if npc_name:
                        unreal.log_warning(f"  CDO NPCDisplayName: {npc_name}")
                        data["diagnosis"]["B_Soulslike_NPC_ShowcaseGuide"]["npc_display_name"] = str(npc_name)
                except Exception as e:
                    unreal.log_warning(f"  CDO NPCDisplayName: Error - {e}")

    # Check DT_GenericDefaultDialog
    generic_dialog_path = "/Game/SoulslikeFramework/Data/Dialog/DT_GenericDefaultDialog"
    unreal.log_warning(f"\n=== Checking {generic_dialog_path} ===")

    generic_dialog = unreal.EditorAssetLibrary.load_asset(generic_dialog_path)
    if generic_dialog:
        unreal.log_warning(f"  [OK] Asset loaded - this might be the fallback containing 'Lorem ipsum'")
        unreal.log_warning(f"  Class: {generic_dialog.get_class().get_name()}")

        # Try to get row names
        if hasattr(generic_dialog, 'get_row_names'):
            row_names = generic_dialog.get_row_names()
            unreal.log_warning(f"  Row count: {len(row_names)}")
            data["diagnosis"]["DT_GenericDefaultDialog"] = {
                "loaded": True,
                "row_count": len(row_names),
                "rows": [str(r) for r in row_names[:5]]  # First 5 rows
            }

    # Save
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(data, f, indent=2)

    unreal.log_warning(f"\nSaved to: {OUTPUT_PATH}")
    unreal.log_warning("\n" + "="*70)

if __name__ == "__main__":
    main()
