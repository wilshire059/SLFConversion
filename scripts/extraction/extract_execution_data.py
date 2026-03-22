"""Extract execution animation data from bp_only project."""
import unreal
import json
import os

def extract_execution_data():
    """Extract ExecuteFront and ExecuteBack animation data from execution data assets."""

    output_path = "C:/scripts/SLFConversion/migration_cache/execution_anim_data.json"

    # Find all execution data assets
    execution_assets = [
        "/Game/SoulslikeFramework/Data/_AnimationData/Executions/DA_Execute_Generic",
    ]

    result = {}

    for asset_path in execution_assets:
        asset = unreal.load_asset(asset_path)
        if not asset:
            unreal.log_warning("Could not load: " + asset_path)
            continue

        asset_name = asset.get_name()
        unreal.log("Processing: " + asset_name)
        unreal.log("Asset class: " + str(asset.get_class().get_name()))

        data = {
            "path": asset_path,
            "ExecuteFront": {
                "Tag": "",
                "Animation": ""
            },
            "ExecuteBack": {
                "Tag": "",
                "Animation": ""
            }
        }

        # Try to get properties directly - list all available properties
        try:
            # Get all property names
            unreal.log("Getting properties...")

            # Try snake_case property names (Python convention)
            for prop_name in ["execute_front", "ExecuteFront"]:
                try:
                    exec_front = asset.get_editor_property(prop_name)
                    if exec_front:
                        unreal.log("Found " + prop_name + ": " + str(type(exec_front)))

                        # Try to get Animation property
                        for anim_name in ["animation", "Animation"]:
                            try:
                                front_anim = exec_front.get_editor_property(anim_name)
                                if front_anim:
                                    # Get the path from the soft object ptr
                                    anim_path = str(front_anim.get_path_name() if hasattr(front_anim, 'get_path_name') else front_anim)
                                    data["ExecuteFront"]["Animation"] = anim_path
                                    unreal.log("ExecuteFront.Animation: " + anim_path)
                                    break
                            except Exception as e:
                                pass

                        # Try to get Tag property
                        for tag_name in ["tag", "Tag"]:
                            try:
                                front_tag = exec_front.get_editor_property(tag_name)
                                if front_tag and hasattr(front_tag, 'tag_name'):
                                    data["ExecuteFront"]["Tag"] = str(front_tag.tag_name)
                                    unreal.log("ExecuteFront.Tag: " + str(front_tag.tag_name))
                                    break
                            except Exception as e:
                                pass
                        break
                except Exception as e:
                    unreal.log("Could not get " + prop_name + ": " + str(e))

            # Same for ExecuteBack
            for prop_name in ["execute_back", "ExecuteBack"]:
                try:
                    exec_back = asset.get_editor_property(prop_name)
                    if exec_back:
                        unreal.log("Found " + prop_name + ": " + str(type(exec_back)))

                        for anim_name in ["animation", "Animation"]:
                            try:
                                back_anim = exec_back.get_editor_property(anim_name)
                                if back_anim:
                                    anim_path = str(back_anim.get_path_name() if hasattr(back_anim, 'get_path_name') else back_anim)
                                    data["ExecuteBack"]["Animation"] = anim_path
                                    unreal.log("ExecuteBack.Animation: " + anim_path)
                                    break
                            except Exception as e:
                                pass

                        for tag_name in ["tag", "Tag"]:
                            try:
                                back_tag = exec_back.get_editor_property(tag_name)
                                if back_tag and hasattr(back_tag, 'tag_name'):
                                    data["ExecuteBack"]["Tag"] = str(back_tag.tag_name)
                                    unreal.log("ExecuteBack.Tag: " + str(back_tag.tag_name))
                                    break
                            except Exception as e:
                                pass
                        break
                except Exception as e:
                    unreal.log("Could not get " + prop_name + ": " + str(e))

        except Exception as e:
            unreal.log_error("Error processing asset: " + str(e))

        result[asset_name] = data

    # Save to JSON
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, 'w') as f:
        json.dump(result, f, indent=2)

    unreal.log("Saved execution data to: " + output_path)
    return result

if __name__ == "__main__":
    extract_execution_data()
