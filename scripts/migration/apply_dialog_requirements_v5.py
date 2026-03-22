# apply_dialog_requirements_v5.py
# Apply dialog requirements using import_text on all structs

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(str(msg))

log("=== Applying Dialog Requirements V5 ===")

dialog_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
dialog = unreal.EditorAssetLibrary.load_asset(dialog_path)

if not dialog:
    log(f"Failed to load dialog: {dialog_path}")
else:
    log(f"Loaded dialog: {dialog.get_class().get_name()}")

    tables = [
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress.DT_ShowcaseGuideNpc_NoProgress",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress.DT_ShowcaseGuideNpc_Progress",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed.DT_ShowcaseGuideNpc_Completed"
    ]

    # States for import_text
    states = ["NotStarted", "InProgress", "Completed"]

    new_requirements = []

    for i in range(3):
        try:
            # Create the requirement struct
            req = unreal.SLFDialogRequirement()

            # Create progress struct and use import_text
            progress = unreal.SLFDialogProgress()

            # Build the import text for the progress struct
            # Format: (ProgressContainer=(GameplayTags=((TagName="..."))),State=X)
            if i == 0:
                # NotStarted is default, no need to specify
                progress_import = '(ProgressContainer=(GameplayTags=((TagName="SoulslikeFramework.Chapters.Demo"))))'
            else:
                progress_import = f'(ProgressContainer=(GameplayTags=((TagName="SoulslikeFramework.Chapters.Demo"))),State={states[i]})'

            progress.import_text(progress_import)
            log(f"Req {i}: Progress imported, export: {progress.export_text()}")

            # Set Container array
            req.set_editor_property('container', [progress])

            # Load the table
            table = unreal.EditorAssetLibrary.load_asset(tables[i])
            if table:
                req.set_editor_property('related_dialog_table', table)

            log(f"Req {i}: Requirement export: {req.export_text()[:150]}")
            new_requirements.append(req)

        except Exception as e:
            log(f"Req {i} error: {e}")
            import traceback
            traceback.print_exc()

    if len(new_requirements) == 3:
        try:
            dialog.set_editor_property('requirement', new_requirements)
            log(f"Set {len(new_requirements)} requirements")

            # Save
            result = unreal.EditorAssetLibrary.save_loaded_asset(dialog, False)
            log(f"Save result: {result}")

        except Exception as e:
            log(f"Set/Save error: {e}")

    # Verify by loading fresh
    log("\n=== Verification ===")
    dialog2 = unreal.EditorAssetLibrary.load_asset(dialog_path)
    try:
        req2 = dialog2.get_editor_property('requirement')
        log(f"Loaded {len(req2)} requirements")
        for j, r in enumerate(req2):
            log(f"  Req {j}: {r.export_text()}")
    except Exception as e:
        log(f"Verify error: {e}")

log("\n=== Done ===")
