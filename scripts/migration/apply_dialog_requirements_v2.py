# apply_dialog_requirements_v2.py
# Apply dialog requirements data by building complete struct and setting array

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(str(msg))

log("=== Applying Dialog Requirements V2 ===")

dialog_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
dialog = unreal.EditorAssetLibrary.load_asset(dialog_path)

if not dialog:
    log(f"Failed to load dialog: {dialog_path}")
else:
    log(f"Loaded dialog: {dialog.get_class().get_name()}")

    # Create a new array of requirements
    # First, create FSLFDialogRequirement and FSLFDialogProgress instances

    # Create 3 requirements
    new_requirements = []

    # Table paths
    tables = [
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed"
    ]

    # Create the GameplayTag
    demo_tag = unreal.GameplayTag()
    demo_tag.set_editor_property('tag_name', unreal.Name("SoulslikeFramework.Chapters.Demo"))
    log(f"Demo tag created: {demo_tag}")

    # Progress states
    progress_states = [
        unreal.ESLFProgress.NOT_STARTED,
        unreal.ESLFProgress.IN_PROGRESS,
        unreal.ESLFProgress.COMPLETED
    ]

    for i in range(3):
        try:
            # Create FSLFDialogProgress
            progress = unreal.SLFDialogProgress()

            # Create GameplayTagContainer and add the tag
            tag_container = unreal.GameplayTagContainer()
            # Try adding tag via add_tag
            try:
                tag_container.add_tag(demo_tag)
                log(f"Req {i}: Added tag to container")
            except Exception as e:
                log(f"Req {i}: add_tag error: {e}")

            progress.set_editor_property('progress_container', tag_container)
            progress.set_editor_property('state', progress_states[i])
            log(f"Req {i}: Created progress with state {progress_states[i]}")

            # Create FSLFDialogRequirement
            req = unreal.SLFDialogRequirement()

            # Set Container (array of FSLFDialogProgress)
            req.set_editor_property('container', [progress])

            # Load and set the dialog table
            table = unreal.EditorAssetLibrary.load_asset(tables[i])
            if table:
                req.set_editor_property('related_dialog_table', table)
                log(f"Req {i}: Set table to {table.get_name()}")
            else:
                log(f"Req {i}: Failed to load table {tables[i]}")

            new_requirements.append(req)

        except Exception as e:
            log(f"Req {i} creation error: {e}")

    # Set the requirements array on the dialog asset
    try:
        dialog.set_editor_property('requirement', new_requirements)
        log(f"Set {len(new_requirements)} requirements on dialog")
    except Exception as e:
        log(f"Error setting requirements: {e}")

    # Save the asset
    try:
        saved = unreal.EditorAssetLibrary.save_loaded_asset(dialog)
        log(f"Save result: {saved}")
    except Exception as e:
        log(f"Save error: {e}")

    # Verify
    try:
        verify_req = dialog.get_editor_property('requirement')
        log(f"Verification: {len(verify_req)} requirements")
        for i, req in enumerate(verify_req):
            container = req.get_editor_property('container')
            table = req.get_editor_property('related_dialog_table')
            log(f"  Req {i}: Container len={len(container) if container else 0}, Table={table}")
    except Exception as e:
        log(f"Verification error: {e}")

log("\n=== Done ===")
