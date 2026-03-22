# check_dodge_struct.py
# Check what struct type is stored in DA_Action_Dodge.RelevantData

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/struct_check_result.txt"
lines = []

asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
asset = unreal.EditorAssetLibrary.load_asset(asset_path)

if asset:
    lines.append(f"Asset: {asset}")
    lines.append(f"Class: {asset.get_class().get_name()}")

    try:
        rd = asset.get_editor_property('RelevantData')
        lines.append(f"\nRelevantData: {rd}")

        # Try export_text
        export = rd.export_text()
        lines.append(f"\nexport_text():\n{export}")

        # Check struct type in export
        if "SLFConversion" in export:
            lines.append("\n*** SUCCESS: Core Redirect applied - using C++ struct! ***")
        elif "FDodgeMontages" in export:
            lines.append("\n*** FAILURE: Still using Blueprint struct FDodgeMontages ***")

    except Exception as e:
        lines.append(f"Error: {e}")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))
