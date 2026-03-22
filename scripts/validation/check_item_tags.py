import unreal

def log(msg):
    unreal.log_warning(str(msg))

# Check what item tags exist in the project
log("=== Checking Item Tags ===")

# Load specific items
items_to_check = [
    "/Game/SoulslikeFramework/Data/Items/DA_HealthFlask",
    "/Game/SoulslikeFramework/Data/Items/DA_MagicSpell",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01"
]

for item_path in items_to_check:
    loaded = unreal.load_asset(item_path)
    if loaded:
        try:
            item_info = loaded.get_editor_property('item_information')
            if item_info:
                item_tag = item_info.get_editor_property('item_tag')
                log(f"ITEM {item_path}: ItemTag = {item_tag}")
            else:
                log(f"ITEM {item_path}: No item_information")
        except Exception as e:
            log(f"ITEM {item_path}: Error - {e}")
    else:
        log(f"ITEM {item_path}: Failed to load")

# Also check the NPC's dialog data asset
log("=== Checking Dialog Assets ===")

# The ShowcaseGuide NPC uses this dialog
dialog_paths = [
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
]

for dialog_path in dialog_paths:
    loaded = unreal.load_asset(dialog_path)
    if loaded:
        log(f"DIALOG Loaded: {dialog_path}")
        log(f"DIALOG Class: {loaded.get_class().get_name()}")

        # Try to get requirement
        try:
            requirement = loaded.get_editor_property('requirement')
            log(f"DIALOG Requirement: {requirement} (len={len(requirement) if requirement else 0})")
            if requirement:
                for i, req in enumerate(requirement):
                    log(f"DIALOG   Req[{i}]: {req}")
                    try:
                        container = req.get_editor_property('container')
                        related_table = req.get_editor_property('related_dialog_table')
                        log(f"DIALOG     Container len: {len(container) if container else 0}")
                        log(f"DIALOG     RelatedTable: {related_table}")
                        if container:
                            for j, progress in enumerate(container):
                                try:
                                    prog_container = progress.get_editor_property('progress_container')
                                    state = progress.get_editor_property('state')
                                    log(f"DIALOG       Progress[{j}]: Tag={prog_container}, State={state}")
                                except Exception as e2:
                                    log(f"DIALOG       Progress[{j}]: Error - {e2}")
                    except Exception as e:
                        log(f"DIALOG     Req access error: {e}")
        except Exception as e:
            log(f"DIALOG Requirement error: {e}")

        try:
            default_table = loaded.get_editor_property('default_dialog_table')
            log(f"DIALOG DefaultDialogTable: {default_table}")
        except Exception as e:
            log(f"DIALOG DefaultDialogTable error: {e}")
    else:
        log(f"DIALOG {dialog_path}: Failed to load")

log("=== Done ===")
