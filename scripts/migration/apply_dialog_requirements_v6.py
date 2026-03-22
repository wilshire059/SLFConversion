# apply_dialog_requirements_v6.py
# Apply dialog requirements using import_text on the ENTIRE requirement struct
# This ensures TSoftObjectPtr paths persist correctly

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(str(msg))

log("=== Applying Dialog Requirements V6 ===")

dialog_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
dialog = unreal.EditorAssetLibrary.load_asset(dialog_path)

if not dialog:
    log(f"Failed to load dialog: {dialog_path}")
else:
    log(f"Loaded dialog: {dialog.get_class().get_name()}")

    # Table paths (full game paths WITH asset name suffix for TSoftObjectPtr)
    tables = [
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress.DT_ShowcaseGuideNpc_NoProgress",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress.DT_ShowcaseGuideNpc_Progress",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed.DT_ShowcaseGuideNpc_Completed"
    ]

    # States: NotStarted=0, InProgress=1, Completed=2
    # In import_text format they use the enum name
    states = ["NotStarted", "InProgress", "Completed"]

    new_requirements = []

    for i in range(3):
        try:
            # Create the requirement struct
            req = unreal.SLFDialogRequirement()

            # Build the complete import text for the requirement struct
            # Format for FSLFDialogRequirement:
            # (Container=((ProgressContainer=(GameplayTags=((TagName="..."))),State=X)),RelatedDialogTable=Path)

            # Build progress part
            if i == 0:
                # NotStarted is default
                progress_str = '(ProgressContainer=(GameplayTags=((TagName="SoulslikeFramework.Chapters.Demo"))))'
            else:
                progress_str = f'(ProgressContainer=(GameplayTags=((TagName="SoulslikeFramework.Chapters.Demo"))),State={states[i]})'

            # Build full requirement import text
            # Use the asset path format for TSoftObjectPtr
            table_path = tables[i]
            req_import = f'(Container=({progress_str}),RelatedDialogTable="{table_path}")'

            log(f"Req {i}: Importing: {req_import[:100]}...")
            req.import_text(req_import)

            # Verify the import
            export = req.export_text()
            log(f"Req {i}: Export after import: {export[:150]}...")

            new_requirements.append(req)

        except Exception as e:
            log(f"Req {i} error: {e}")
            import traceback
            traceback.print_exc()

    if len(new_requirements) == 3:
        try:
            dialog.set_editor_property('requirement', new_requirements)
            log(f"Set {len(new_requirements)} requirements on dialog")

            # Save the asset
            result = unreal.EditorAssetLibrary.save_loaded_asset(dialog, False)
            log(f"Save result: {result}")

            # Double-check by exporting the dialog requirement property
            req_check = dialog.get_editor_property('requirement')
            log(f"After save: {len(req_check)} requirements")
            for j, r in enumerate(req_check):
                log(f"  Req {j}: {r.export_text()[:100]}")

        except Exception as e:
            log(f"Set/Save error: {e}")
            import traceback
            traceback.print_exc()
    else:
        log(f"Only created {len(new_requirements)} requirements, expected 3")

log("\n=== Done ===")
