import unreal

def log(msg):
    unreal.log_warning(str(msg))

# Check what item tags exist in the BACKUP project
log("=== Checking BACKUP Item Tags ===")

# Load specific items
items_to_check = [
    "/Game/SoulslikeFramework/Data/Items/DA_HealthFlask"
]

for item_path in items_to_check:
    loaded = unreal.load_asset(item_path)
    if loaded:
        log(f"ITEM Loaded: {item_path}")
        log(f"ITEM Class: {loaded.get_class().get_name()}")

        try:
            item_info = loaded.get_editor_property('item_information')
            if item_info:
                log(f"ITEM item_information found")

                # Get all properties of item_info
                item_tag = item_info.get_editor_property('item_tag')
                log(f"ITEM ItemTag: {item_tag}")

                # Try to get tag_name from the gameplay tag
                try:
                    tag_name = item_tag.get_editor_property('tag_name')
                    log(f"ITEM ItemTag.tag_name: {tag_name}")
                except Exception as e:
                    log(f"ITEM ItemTag.tag_name error: {e}")

                # List properties of item_info
                for prop in ['item_name', 'item_description', 'item_tag', 'item_category']:
                    try:
                        val = item_info.get_editor_property(prop)
                        log(f"ITEM   {prop}: {val}")
                    except Exception as e:
                        log(f"ITEM   {prop}: error - {e}")
            else:
                log(f"ITEM No item_information")
        except Exception as e:
            log(f"ITEM Error: {e}")
    else:
        log(f"ITEM {item_path}: Failed to load")

# Also check the NPC's dialog data asset
log("=== Checking BACKUP Dialog Assets ===")

dialog_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
loaded = unreal.load_asset(dialog_path)
if loaded:
    log(f"DIALOG Loaded: {dialog_path}")
    log(f"DIALOG Class: {loaded.get_class().get_name()}")

    # Export to JSON to see full structure
    try:
        # Use EditorAssetLibrary to get text export
        text = unreal.EditorAssetLibrary.get_metadata_tag_values(dialog_path)
        log(f"DIALOG Metadata: {text}")
    except:
        pass

    # Try to find the one-liner dialog table
    try:
        requirement = loaded.get_editor_property('requirement')
        log(f"DIALOG Requirement count: {len(requirement) if requirement else 0}")
    except:
        pass

    try:
        default_table = loaded.get_editor_property('default_dialog_table')
        log(f"DIALOG DefaultTable: {default_table}")
    except:
        pass

log("=== Done ===")
