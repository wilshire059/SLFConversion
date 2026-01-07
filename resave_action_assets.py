# resave_action_assets.py
# Force re-save all action data assets to apply Core Redirects
# This converts FInstancedStruct from Blueprint structs to C++ structs

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"

def resave_action_assets():
    """
    Re-save all action data assets.
    When Unreal loads an asset with a Blueprint struct that has a Core Redirect,
    it automatically converts to the C++ struct. Re-saving persists this change.
    """
    lines = []
    lines.append("=" * 70)
    lines.append("RE-SAVING ACTION DATA ASSETS TO APPLY CORE REDIRECTS")
    lines.append("=" * 70)
    lines.append("")

    actions_path = "/Game/SoulslikeFramework/Data/Actions/ActionData"
    assets = unreal.EditorAssetLibrary.list_assets(actions_path, recursive=True, include_folder=False)

    saved_count = 0
    failed_count = 0

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]

        # Skip the parent Blueprint itself
        if asset_name == "PDA_Action":
            lines.append(f"  {asset_name}: [SKIP] Parent Blueprint")
            continue

        # Load the asset (this triggers Core Redirect conversion during deserialization)
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            lines.append(f"  {asset_name}: [ERROR] Could not load")
            failed_count += 1
            continue

        try:
            # Force save (only_if_is_dirty=False forces save even if not dirty)
            # The Core Redirect is applied during load - re-saving persists the converted struct type
            result = unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)

            if result:
                lines.append(f"  {asset_name}: [SAVED]")
                saved_count += 1
            else:
                lines.append(f"  {asset_name}: [FAILED] Save returned False")
                failed_count += 1

        except Exception as e:
            lines.append(f"  {asset_name}: [ERROR] {e}")
            failed_count += 1

    lines.append("")
    lines.append("=" * 70)
    lines.append("SUMMARY")
    lines.append("=" * 70)
    lines.append(f"  Successfully saved: {saved_count}")
    lines.append(f"  Failed: {failed_count}")
    lines.append("")
    lines.append("Next step: Run check_action_struct_types.py to verify conversion")

    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))

    print(f"Output written to {OUTPUT_FILE}")
    print(f"Saved: {saved_count}, Failed: {failed_count}")


if __name__ == "__main__":
    resave_action_assets()
