# apply_dialog_requirements.py
# Apply dialog requirements data to C++ DA_ExampleDialog asset

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(str(msg))

log("=== Applying Dialog Requirements ===")

dialog_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
dialog = unreal.EditorAssetLibrary.load_asset(dialog_path)

if not dialog:
    log(f"Failed to load dialog: {dialog_path}")
else:
    log(f"Loaded dialog: {dialog.get_class().get_name()}")

    # The requirements data extracted from backup:
    # Req 0: Tag=SoulslikeFramework.Chapters.Demo, State=NotStarted, Table=DT_ShowcaseGuideNpc_NoProgress
    # Req 1: Tag=SoulslikeFramework.Chapters.Demo, State=InProgress, Table=DT_ShowcaseGuideNpc_Progress
    # Req 2: Tag=SoulslikeFramework.Chapters.Demo, State=Completed, Table=DT_ShowcaseGuideNpc_Completed

    # Get progress enum values
    # ESLFProgress: NotStarted=0, InProgress=1, Completed=2
    try:
        progress_enum = unreal.load_object(None, "/Script/SLFConversion.ESLFProgress")
        log(f"Progress enum: {progress_enum}")
    except:
        log("Couldn't load progress enum directly")

    # Create requirements using import_text
    requirements = dialog.get_editor_property('requirement')
    log(f"Current requirements count: {len(requirements) if requirements else 0}")

    if requirements and len(requirements) >= 3:
        # Table paths
        tables = [
            "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress.DT_ShowcaseGuideNpc_NoProgress",
            "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress.DT_ShowcaseGuideNpc_Progress",
            "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed.DT_ShowcaseGuideNpc_Completed"
        ]

        # States: NotStarted=0, InProgress=1, Completed=2
        states = [0, 1, 2]

        # Tag for all requirements
        tag_name = "SoulslikeFramework.Chapters.Demo"

        for i in range(3):
            try:
                req = requirements[i]

                # Build the import text for this requirement
                # Format: (Container=((ProgressContainer=(GameplayTags=((TagName="..."))),State=X)),RelatedDialogTable="...")

                if states[i] == 0:
                    # NotStarted - no State field needed (default)
                    import_text = f'(Container=((ProgressContainer=(GameplayTags=((TagName="{tag_name}"))))),RelatedDialogTable="{tables[i]}")'
                else:
                    # InProgress or Completed
                    import_text = f'(Container=((ProgressContainer=(GameplayTags=((TagName="{tag_name}"))),State={states[i]})),RelatedDialogTable="{tables[i]}")'

                log(f"Req {i} import_text: {import_text}")

                # Try to import
                req.import_text(import_text)
                log(f"Req {i} imported successfully")

                # Verify
                export = req.export_text()
                log(f"Req {i} export after import: {export[:200]}")

            except Exception as e:
                log(f"Req {i} error: {e}")

        # Save the asset
        try:
            unreal.EditorAssetLibrary.save_asset(dialog_path)
            log("Dialog asset saved")
        except Exception as e:
            log(f"Save error: {e}")
    else:
        log("Requirements array not properly initialized!")

log("\n=== Done ===")
