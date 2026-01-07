# verify_action_cast.py
# Verify that DA_Action_* assets can now cast to UPDA_Action

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"

lines = []
lines.append("=" * 60)
lines.append("VERIFYING ACTION ASSETS CAST TO UPDA_Action")
lines.append("=" * 60)

# Check an action asset
asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
asset = unreal.EditorAssetLibrary.load_asset(asset_path)

if asset:
    lines.append(f"\nAsset: DA_Action_Dodge")
    lines.append(f"  Asset class: {asset.get_class().get_name()}")

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

    # Try to get RelevantData directly (should work now with C++ parent)
    lines.append(f"\n  Checking direct property access:")
    try:
        relevant_data = asset.get_editor_property('RelevantData')
        lines.append(f"    RelevantData: {relevant_data}")
        if relevant_data and hasattr(relevant_data, 'export_text'):
            export = relevant_data.export_text()
            lines.append(f"    Export: {export[:150]}...")
    except Exception as e:
        lines.append(f"    RelevantData: [ERROR] {e}")

    try:
        action_class = asset.get_editor_property('ActionClass')
        lines.append(f"    ActionClass: {action_class}")
    except Exception as e:
        lines.append(f"    ActionClass: [ERROR] {e}")

lines.append("")
lines.append("=" * 60)
lines.append("If inheritance shows UPDA_Action in chain, Cast<UPDA_Action>() will work!")
lines.append("=" * 60)

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
