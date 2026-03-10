# check_dialog_requirements.py
# Check the Requirements data in dialog assets

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(str(msg))

log("=== Checking Dialog Requirements ===")

dialog_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
dialog = unreal.EditorAssetLibrary.load_asset(dialog_path)

if not dialog:
    log(f"Failed to load dialog: {dialog_path}")
else:
    log(f"Loaded dialog: {dialog.get_class().get_name()}")

    # Get Requirements array
    try:
        requirements = dialog.get_editor_property('requirement')
        log(f"Requirements count: {len(requirements) if requirements else 0}")

        if requirements:
            for i, req in enumerate(requirements):
                log(f"\n--- Requirement {i} ---")

                # Get Container
                try:
                    container = req.get_editor_property('container')
                    log(f"  Container length: {len(container) if container else 0}")

                    if container:
                        for j, progress in enumerate(container):
                            log(f"    Progress {j}:")
                            try:
                                progress_container = progress.get_editor_property('progress_container')
                                log(f"      ProgressContainer: {progress_container}")
                            except Exception as e:
                                log(f"      ProgressContainer error: {e}")

                            try:
                                state = progress.get_editor_property('state')
                                log(f"      State: {state}")
                            except Exception as e:
                                log(f"      State error: {e}")
                except Exception as e:
                    log(f"  Container error: {e}")

                # Get RelatedDialogTable
                try:
                    table = req.get_editor_property('related_dialog_table')
                    log(f"  RelatedDialogTable: {table}")
                except Exception as e:
                    log(f"  RelatedDialogTable error: {e}")

    except Exception as e:
        log(f"Error getting requirements: {e}")

# Also check the default dialog table
log("\n=== Default Dialog Table ===")
try:
    default_table = dialog.get_editor_property('default_dialog_table')
    log(f"DefaultDialogTable: {default_table}")
except Exception as e:
    log(f"DefaultDialogTable error: {e}")

log("\n=== Done ===")
