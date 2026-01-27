"""
Debug stat Blueprint parent class detection
"""

import unreal

def debug_stat_parent():
    """Debug how to detect the parent class of stat Blueprints."""

    results = []

    bp_path = "/Game/SoulslikeFramework/Data/Stats/Secondary/B_HP"
    bp_asset = unreal.load_asset(bp_path)

    if not bp_asset:
        results.append(f"ERROR: Failed to load: {bp_path}")
        with open("C:/scripts/SLFConversion/debug_results.txt", "w") as f:
            f.write("\n".join(results))
        return

    results.append(f"Loaded Blueprint: {bp_path}")
    results.append(f"Asset type: {type(bp_asset).__name__}")

    # Try to get generated class
    gen_class = bp_asset.generated_class()
    if gen_class:
        results.append(f"Generated class: {gen_class.get_name()}")
        results.append(f"Generated class full path: {gen_class.get_path_name()}")

    # Load the C++ UB_HP class
    cpp_hp_class = unreal.load_class(None, "/Script/SLFConversion.B_HP")
    cpp_stat_class = unreal.load_class(None, "/Script/SLFConversion.B_Stat")

    if cpp_hp_class:
        results.append(f"\nC++ UB_HP class: {cpp_hp_class.get_name()}")
        results.append(f"C++ UB_HP path: {cpp_hp_class.get_path_name()}")

    if cpp_stat_class:
        results.append(f"C++ UB_Stat class: {cpp_stat_class.get_name()}")
        results.append(f"C++ UB_Stat path: {cpp_stat_class.get_path_name()}")

    # Check if gen_class CDO can be cast to UB_Stat
    # by checking if it has UB_Stat properties
    if gen_class:
        results.append(f"\n--- Checking generated class properties ---")

        # Try to see if gen_class has StatInfo property (from UB_Stat)
        try:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                results.append(f"CDO type: {type(cdo).__name__}")

                # Check if it has StatInfo (should be inherited from UB_Stat)
                try:
                    stat_info = cdo.get_editor_property('stat_info')
                    results.append(f"CDO has stat_info: YES")
                except:
                    results.append(f"CDO has stat_info: NO")

                # Check if it has MinValue (should be inherited from UB_Stat)
                try:
                    min_val = cdo.get_editor_property('min_value')
                    results.append(f"CDO has min_value: YES ({min_val})")
                except:
                    results.append(f"CDO has min_value: NO")
        except Exception as e:
            results.append(f"Error getting CDO: {e}")

    # Use EditorAssetLibrary to get more info
    results.append(f"\n--- Using EditorAssetLibrary ---")
    try:
        # Get the Blueprint's parent from EditorAssetLibrary
        # Get class tags
        asset_data = unreal.EditorAssetLibrary.find_asset_data(bp_path)
        if asset_data:
            results.append(f"Asset data found: {asset_data.asset_name}")
            results.append(f"Asset class: {asset_data.asset_class_path}")

            # Try to get parent class from tags
            tag_value = asset_data.get_tag_value("ParentClass")
            results.append(f"ParentClass tag: {tag_value}")
    except Exception as e:
        results.append(f"EditorAssetLibrary error: {e}")

    # Write results
    with open("C:/scripts/SLFConversion/debug_results.txt", "w") as f:
        f.write("\n".join(results))


if __name__ == "__main__":
    debug_stat_parent()
