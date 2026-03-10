# apply_dialog_requirements_v4.py
# Apply dialog requirements using import_text on structs

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(str(msg))

log("=== Applying Dialog Requirements V4 ===")

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

    # States: NotStarted=0, InProgress=1, Completed=2
    states = ["NotStarted", "InProgress", "Completed"]

    new_requirements = []

    for i in range(3):
        try:
            # Create the requirement struct
            req = unreal.SLFDialogRequirement()

            # Create progress struct
            progress = unreal.SLFDialogProgress()

            # Create GameplayTagContainer and import the tag
            container = unreal.GameplayTagContainer()
            container_import = '(GameplayTags=((TagName="SoulslikeFramework.Chapters.Demo")))'
            container.import_text(container_import)
            log(f"Req {i}: Container imported, export: {container.export_text()}")

            progress.set_editor_property('progress_container', container)

            # Set state using enum
            if i == 0:
                progress.set_editor_property('state', unreal.ESLFProgress.NOT_STARTED)
            elif i == 1:
                progress.set_editor_property('state', unreal.ESLFProgress.IN_PROGRESS)
            else:
                progress.set_editor_property('state', unreal.ESLFProgress.COMPLETED)

            log(f"Req {i}: Progress export: {progress.export_text()}")

            # Set Container array
            req.set_editor_property('container', [progress])

            # Load the table
            table = unreal.EditorAssetLibrary.load_asset(tables[i])
            if table:
                req.set_editor_property('related_dialog_table', table)
                log(f"Req {i}: Table set")

            log(f"Req {i}: Requirement export: {req.export_text()[:150]}")
            new_requirements.append(req)

        except Exception as e:
            log(f"Req {i} error: {e}")

    if len(new_requirements) == 3:
        try:
            dialog.set_editor_property('requirement', new_requirements)
            log(f"Set {len(new_requirements)} requirements")

            # Save
            result = unreal.EditorAssetLibrary.save_loaded_asset(dialog, False)
            log(f"Save result: {result}")

            # Verify by reloading
            dialog2 = unreal.EditorAssetLibrary.load_asset(dialog_path)
            req2 = dialog2.get_editor_property('requirement')
            log(f"Verify after reload: {len(req2)} requirements")
            for i, r in enumerate(req2):
                log(f"  Req {i}: {r.export_text()[:100]}")

        except Exception as e:
            log(f"Set/Save error: {e}")

log("\n=== Done ===")
