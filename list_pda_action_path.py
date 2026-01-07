# list_pda_action_path.py
# Find the correct path to PDA_Action

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"
lines = []

# Try different paths
paths_to_check = [
    "/Game/SoulslikeFramework/Data/Actions/PDA_Action",
    "/Game/SoulslikeFramework/Data/Actions/ActionData/PDA_Action",
    "/Game/SoulslikeFramework/Data/PDA_Action",
]

lines.append("=" * 60)
lines.append("SEARCHING FOR PDA_Action")
lines.append("=" * 60)

for path in paths_to_check:
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset:
        lines.append(f"FOUND: {path}")
        lines.append(f"  Class: {asset.get_class().get_name()}")
        gen_class = asset.generated_class()
        lines.append(f"  Generated class: {gen_class}")
        if hasattr(gen_class, 'get_super_struct'):
            parent = gen_class.get_super_struct()
            lines.append(f"  Parent class: {parent}")
    else:
        lines.append(f"NOT FOUND: {path}")

# List assets in ActionData folder
lines.append("")
lines.append("Assets in ActionData folder:")
actions_path = "/Game/SoulslikeFramework/Data/Actions/ActionData"
assets = unreal.EditorAssetLibrary.list_assets(actions_path, recursive=False, include_folder=False)
for a in assets[:30]:
    lines.append(f"  {a}")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
