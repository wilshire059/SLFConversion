# apply_dialog_requirements_v3.py
# Apply dialog requirements using import_text on the whole requirement struct

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(str(msg))

log("=== Applying Dialog Requirements V3 ===")

dialog_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
dialog = unreal.EditorAssetLibrary.load_asset(dialog_path)

if not dialog:
    log(f"Failed to load dialog: {dialog_path}")
else:
    log(f"Loaded dialog: {dialog.get_class().get_name()}")

    # The requirements data we need to apply:
    # Req 0: Tag=SoulslikeFramework.Chapters.Demo, State=NotStarted, Table=DT_ShowcaseGuideNpc_NoProgress
    # Req 1: Tag=SoulslikeFramework.Chapters.Demo, State=InProgress, Table=DT_ShowcaseGuideNpc_Progress
    # Req 2: Tag=SoulslikeFramework.Chapters.Demo, State=Completed, Table=DT_ShowcaseGuideNpc_Completed

    tables = [
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress.DT_ShowcaseGuideNpc_NoProgress",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress.DT_ShowcaseGuideNpc_Progress",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed.DT_ShowcaseGuideNpc_Completed"
    ]

    # Try to request the tag from the gameplay tags manager
    try:
        gtm = unreal.GameplayTagsManager.get()
        demo_tag = gtm.request_gameplay_tag(unreal.Name("SoulslikeFramework.Chapters.Demo"))
        log(f"Requested tag: {demo_tag}")
    except Exception as e:
        log(f"Tag request error: {e}")
        demo_tag = None

    # Try to create a GameplayTagContainer with add_tag_fast
    try:
        container = unreal.GameplayTagContainer()
        # Check available methods
        methods = [m for m in dir(container) if not m.startswith('_')]
        log(f"Container methods: {methods}")
    except Exception as e:
        log(f"Container error: {e}")

    # Create requirements using the struct directly
    new_requirements = []

    for i in range(3):
        try:
            req = unreal.SLFDialogRequirement()

            # Create progress
            progress = unreal.SLFDialogProgress()

            # Use request_gameplay_tag to get the tag
            gtm = unreal.GameplayTagsManager.get()
            tag = gtm.request_gameplay_tag(unreal.Name("SoulslikeFramework.Chapters.Demo"))

            # Create container and add tag
            container = unreal.GameplayTagContainer()
            container.add_tag(tag)

            progress.set_editor_property('progress_container', container)

            # Set state
            if i == 0:
                progress.set_editor_property('state', unreal.ESLFProgress.NOT_STARTED)
            elif i == 1:
                progress.set_editor_property('state', unreal.ESLFProgress.IN_PROGRESS)
            else:
                progress.set_editor_property('state', unreal.ESLFProgress.COMPLETED)

            log(f"Req {i}: Progress created")

            # Set Container (array of progress)
            req.set_editor_property('container', [progress])

            # Load the table
            table = unreal.EditorAssetLibrary.load_asset(tables[i])
            if table:
                req.set_editor_property('related_dialog_table', table)
                log(f"Req {i}: Table set to {table.get_name()}")
            else:
                log(f"Req {i}: Failed to load table")

            new_requirements.append(req)
            log(f"Req {i}: Export: {req.export_text()[:100]}")

        except Exception as e:
            log(f"Req {i} error: {e}")

    if len(new_requirements) == 3:
        try:
            dialog.set_editor_property('requirement', new_requirements)
            log(f"Set requirements on dialog")

            # Mark package dirty and save
            unreal.EditorAssetLibrary.save_loaded_asset(dialog, False)
            log("Dialog saved")
        except Exception as e:
            log(f"Set/Save error: {e}")
    else:
        log(f"Only created {len(new_requirements)} requirements, expected 3")

log("\n=== Done ===")
