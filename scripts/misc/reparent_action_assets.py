# reparent_action_assets.py
# Reparent DA_Action_* data assets directly to C++ UPDA_Action
# This bypasses the Blueprint PDA_Action naming collision

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/reparent_result.txt"
lines = []

lines.append("=" * 60)
lines.append("REPARENTING DA_ACTION ASSETS DIRECTLY TO C++ UPDA_ACTION")
lines.append("=" * 60)

# Load the C++ class
cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Action")
if not cpp_class:
    lines.append("ERROR: Could not load C++ PDA_Action class!")
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))
    raise Exception("Cannot load C++ class")

lines.append(f"\nC++ class: {cpp_class.get_name()} ({cpp_class.get_path_name()})")

# Find all DA_Action_* assets
asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
action_folder = "/Game/SoulslikeFramework/Data/Actions/ActionData"

# Get all assets in the folder
all_assets = unreal.EditorAssetLibrary.list_assets(action_folder, recursive=True)
lines.append(f"\nFound {len(all_assets)} assets in {action_folder}")

success_count = 0
skip_count = 0
fail_count = 0

for asset_path in all_assets:
    # Skip the parent PDA_Action Blueprint itself
    if asset_path.endswith("/PDA_Action"):
        lines.append(f"\nSKIP: {asset_path} (parent Blueprint)")
        skip_count += 1
        continue

    # Load the asset as a Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not bp:
        lines.append(f"\nFAIL: {asset_path} (could not load)")
        fail_count += 1
        continue

    # Check if it's a Blueprint (DataAsset definition Blueprint)
    if not isinstance(bp, unreal.Blueprint):
        lines.append(f"\nSKIP: {asset_path} (not a Blueprint, is {type(bp).__name__})")
        skip_count += 1
        continue

    asset_name = asset_path.split("/")[-1]
    gen_class = bp.generated_class()

    # Check current parent
    current_parent = None
    if gen_class and hasattr(gen_class, 'get_super_class'):
        current_parent = gen_class.get_super_class()

    if current_parent:
        parent_path = current_parent.get_path_name()
        if "/Script/SLFConversion" in parent_path:
            lines.append(f"\nSKIP: {asset_name} (already C++ parent: {current_parent.get_name()})")
            skip_count += 1
            continue

    lines.append(f"\n{asset_name}:")
    lines.append(f"  Current parent: {current_parent.get_name() if current_parent else 'None'}")

    # Reparent to C++
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if result:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
            lines.append(f"  REPARENTED to C++ UPDA_Action!")
            success_count += 1
        else:
            lines.append(f"  FAILED: reparent returned False")
            fail_count += 1
    except Exception as e:
        lines.append(f"  ERROR: {e}")
        fail_count += 1

lines.append("\n" + "=" * 60)
lines.append(f"SUMMARY: {success_count} reparented, {skip_count} skipped, {fail_count} failed")
lines.append("=" * 60)

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
