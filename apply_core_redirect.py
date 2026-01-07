# apply_core_redirect.py
# Proper sequence to apply Core Redirect:
# 1. Delete Blueprint struct (so Core Redirect kicks in)
# 2. Load asset (Core Redirect applies during deserialization)
# 3. Check data is preserved
# 4. Save asset with new struct type

import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/redirect_result.txt"
lines = []

# Step 1: Delete the Blueprint struct file
bp_struct_path = "C:/scripts/SLFConversion/Content/SoulslikeFramework/Structures/Animation/FDodgeMontages.uasset"
if os.path.exists(bp_struct_path):
    os.remove(bp_struct_path)
    lines.append("Step 1: Deleted FDodgeMontages.uasset")
else:
    lines.append("Step 1: FDodgeMontages.uasset already deleted")

# Step 2: Load the action asset (Core Redirect should apply during load)
asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
lines.append(f"\nStep 2: Loading {asset_path}...")
asset = unreal.EditorAssetLibrary.load_asset(asset_path)

if asset:
    lines.append(f"  Loaded: {asset}")
    lines.append(f"  Class: {asset.get_class().get_name()}")

    # Step 3: Check RelevantData
    lines.append("\nStep 3: Checking RelevantData...")
    try:
        rd = asset.get_editor_property('RelevantData')
        lines.append(f"  RelevantData type: {type(rd)}")

        # Try to get the struct data
        export = rd.export_text()
        if export:
            lines.append(f"  export_text():\n{export[:1000]}")

            if "SLFConversion" in export or "SLFDodgeMontages" in export:
                lines.append("\n  *** SUCCESS: Data converted to C++ struct! ***")
            elif "FDodgeMontages" in export:
                lines.append("\n  *** WARNING: Still references Blueprint struct type ***")
            else:
                lines.append("\n  *** Data present, checking format... ***")
        else:
            lines.append("  export_text() returned None - DATA EMPTY!")

    except Exception as e:
        lines.append(f"  Error: {e}")

    # Step 4: Save if data looks good
    if export and export != "None":
        lines.append("\nStep 4: Saving asset...")
        result = unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
        lines.append(f"  Save result: {result}")
else:
    lines.append("  ERROR: Could not load asset!")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))
