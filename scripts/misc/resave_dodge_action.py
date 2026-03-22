# resave_dodge_action.py
# Re-save DA_Action_Dodge to apply Core Redirect for FDodgeMontages -> FSLFDodgeMontages

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/resave_result.txt"
lines = []

asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
asset = unreal.EditorAssetLibrary.load_asset(asset_path)

if asset:
    lines.append(f"Loaded: {asset}")

    # Check RelevantData struct type BEFORE save
    relevant_data = asset.get_editor_property('RelevantData')
    if relevant_data and hasattr(relevant_data, 'get_script_struct'):
        ss = relevant_data.get_script_struct()
        if ss:
            lines.append(f"BEFORE: RelevantData struct: {ss.get_name()} ({ss.get_path_name()})")
        else:
            lines.append("BEFORE: RelevantData struct is NULL")
    else:
        lines.append("BEFORE: No RelevantData or no get_script_struct")

    # Save the asset
    result = unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
    lines.append(f"Save result: {result}")

    # Reload and check again
    asset2 = unreal.EditorAssetLibrary.load_asset(asset_path)
    relevant_data2 = asset2.get_editor_property('RelevantData')
    if relevant_data2 and hasattr(relevant_data2, 'get_script_struct'):
        ss2 = relevant_data2.get_script_struct()
        if ss2:
            lines.append(f"AFTER: RelevantData struct: {ss2.get_name()} ({ss2.get_path_name()})")
            if "SLFConversion" in ss2.get_path_name():
                lines.append("SUCCESS: Core Redirect applied!")
            else:
                lines.append("FAILURE: Still using Blueprint struct")
        else:
            lines.append("AFTER: RelevantData struct is NULL")
else:
    lines.append("ERROR: Could not load asset")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))
