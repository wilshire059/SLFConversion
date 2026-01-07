# apply_action_data.py
# Apply extracted action data back to assets after reparenting
# Run AFTER reparent_pda_action.py

import unreal
import json

INPUT_FILE = "C:/scripts/SLFConversion/action_data.json"
OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"

def apply_action_data():
    """
    Apply extracted action data back to DA_Action_* assets.
    This restores any data that might have been lost during reparenting.
    """
    lines = []
    lines.append("=" * 60)
    lines.append("APPLYING ACTION DATA TO REPARENTED ASSETS")
    lines.append("=" * 60)

    # Load extracted data
    try:
        with open(INPUT_FILE, 'r') as f:
            action_data = json.load(f)
        lines.append(f"\nLoaded data for {len(action_data)} assets from {INPUT_FILE}")
    except Exception as e:
        lines.append(f"\nERROR: Could not load {INPUT_FILE}: {e}")
        with open(OUTPUT_FILE, 'w') as f:
            f.write('\n'.join(lines))
        return

    success_count = 0
    skip_count = 0
    error_count = 0

    for asset_name, data in action_data.items():
        asset_path = data.get("asset_path")
        if not asset_path:
            lines.append(f"\n{asset_name}: [SKIP] No asset_path in data")
            skip_count += 1
            continue

        lines.append(f"\n{asset_name}:")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            lines.append(f"  [ERROR] Could not load asset")
            error_count += 1
            continue

        try:
            modified = False

            # Apply ActionClass (TSoftClassPtr)
            if "ActionClass" in data:
                try:
                    # Load the class from the path
                    class_path = data["ActionClass"]
                    # Clean up path if needed
                    if "'" in class_path:
                        class_path = class_path.split("'")[1] if "'" in class_path else class_path

                    action_class = unreal.load_class(None, class_path)
                    if action_class:
                        asset.set_editor_property('ActionClass', action_class)
                        lines.append(f"  ActionClass: {class_path}")
                        modified = True
                except Exception as e:
                    lines.append(f"  ActionClass: [FAILED] {e}")

            # Apply ActionMontage (TSoftObjectPtr)
            if "ActionMontage" in data:
                try:
                    montage_path = data["ActionMontage"]
                    if "'" in montage_path:
                        montage_path = montage_path.split("'")[1] if "'" in montage_path else montage_path

                    montage = unreal.load_asset(montage_path)
                    if montage:
                        asset.set_editor_property('ActionMontage', montage)
                        lines.append(f"  ActionMontage: {montage_path}")
                        modified = True
                except Exception as e:
                    lines.append(f"  ActionMontage: [FAILED] {e}")

            # Apply StaminaCost
            if "StaminaCost" in data:
                try:
                    asset.set_editor_property('StaminaCost', data["StaminaCost"])
                    lines.append(f"  StaminaCost: {data['StaminaCost']}")
                    modified = True
                except Exception as e:
                    lines.append(f"  StaminaCost: [FAILED] {e}")

            # Apply bCanBeInterrupted
            if "bCanBeInterrupted" in data:
                try:
                    asset.set_editor_property('bCanBeInterrupted', data["bCanBeInterrupted"])
                    lines.append(f"  bCanBeInterrupted: {data['bCanBeInterrupted']}")
                    modified = True
                except Exception as e:
                    lines.append(f"  bCanBeInterrupted: [FAILED] {e}")

            # Apply RequiredStatAmount
            if "RequiredStatAmount" in data:
                try:
                    asset.set_editor_property('RequiredStatAmount', data["RequiredStatAmount"])
                    lines.append(f"  RequiredStatAmount: {data['RequiredStatAmount']}")
                    modified = True
                except Exception as e:
                    lines.append(f"  RequiredStatAmount: [FAILED] {e}")

            # Note: RelevantData (FInstancedStruct) should be preserved automatically
            # since the property exists in both Blueprint and C++ with same type.
            # Core Redirects handle the struct type conversion.

            if modified:
                # Save the asset
                save_result = unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
                if save_result:
                    lines.append(f"  [SAVED]")
                    success_count += 1
                else:
                    lines.append(f"  [SAVE FAILED]")
                    error_count += 1
            else:
                lines.append(f"  [SKIP] No properties to apply")
                skip_count += 1

        except Exception as e:
            lines.append(f"  [ERROR] {e}")
            error_count += 1

    lines.append("")
    lines.append("=" * 60)
    lines.append("SUMMARY")
    lines.append("=" * 60)
    lines.append(f"  Success: {success_count}")
    lines.append(f"  Skipped: {skip_count}")
    lines.append(f"  Errors: {error_count}")
    lines.append("")
    lines.append("Next: Remove reflection from action classes and test in PIE")

    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))

    print(f"Output written to {OUTPUT_FILE}")
    print(f"Success: {success_count}, Skipped: {skip_count}, Errors: {error_count}")


if __name__ == "__main__":
    apply_action_data()
