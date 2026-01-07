# test_dodge_action.py
# Verify that dodge action data is correctly loaded via Core Redirect

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"
lines = []

lines.append("=" * 60)
lines.append("TESTING DODGE ACTION DATA LOADING")
lines.append("=" * 60)

# Load the dodge action data asset
asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
asset = unreal.EditorAssetLibrary.load_asset(asset_path)

if not asset:
    lines.append("ERROR: Could not load DA_Action_Dodge")
else:
    lines.append(f"\nAsset loaded: {asset}")
    lines.append(f"  Class: {asset.get_class().get_name()}")

    # Check parent class chain
    asset_class = asset.get_class()
    lines.append(f"\n  Inheritance chain:")
    current = asset_class
    depth = 0
    while current and depth < 10:
        lines.append(f"    {'  ' * depth}{current.get_name()}")
        if hasattr(current, 'get_super_struct'):
            current = current.get_super_struct()
        else:
            current = None
        depth += 1

    # Check if we can get RelevantData
    lines.append(f"\n  Checking RelevantData property:")
    try:
        relevant_data = asset.get_editor_property('RelevantData')
        lines.append(f"    RelevantData type: {type(relevant_data)}")
        lines.append(f"    RelevantData value: {relevant_data}")

        if relevant_data and hasattr(relevant_data, 'export_text'):
            export = relevant_data.export_text()
            lines.append(f"\n    Export text:")
            # Split into lines for readability
            for line in export.split(','):
                lines.append(f"      {line.strip()}")

    except Exception as e:
        lines.append(f"    ERROR getting RelevantData: {e}")

    # Check ActionClass
    lines.append(f"\n  Checking ActionClass property:")
    try:
        action_class = asset.get_editor_property('ActionClass')
        lines.append(f"    ActionClass: {action_class}")
    except Exception as e:
        lines.append(f"    ERROR: {e}")

lines.append("")
lines.append("=" * 60)
lines.append("If you see Forward= paths above, the data is preserved!")
lines.append("The C++ code will use GetPtr<FSLFDodgeMontages>() to extract it")
lines.append("=" * 60)

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
