# verify_dialog_requirements.py
# Verify that dialog requirements persisted correctly

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(str(msg))

log("=== Verifying Dialog Requirements ===")

dialog_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
dialog = unreal.EditorAssetLibrary.load_asset(dialog_path)

if not dialog:
    log(f"Failed to load dialog: {dialog_path}")
else:
    log(f"Loaded dialog: {dialog.get_class().get_name()}")

    try:
        req = dialog.get_editor_property('requirement')
        log(f"Found {len(req)} requirements")

        for i, r in enumerate(req):
            export = r.export_text()
            log(f"Req {i}: {export}")

            # Also check Container
            container = r.get_editor_property('container')
            log(f"  Container has {len(container)} entries")
            for j, prog in enumerate(container):
                state = prog.get_editor_property('state')
                pc = prog.get_editor_property('progress_container')
                log(f"    Progress {j}: State={state}, Container={pc.export_text()}")

            # Check table
            table = r.get_editor_property('related_dialog_table')
            log(f"  Table: {table}")

    except Exception as e:
        log(f"Error: {e}")
        import traceback
        traceback.print_exc()

log("\n=== Done ===")
