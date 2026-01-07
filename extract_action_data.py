# extract_action_data.py
# Extracts all action asset data before reparenting PDA_Action to C++
# Run in Unreal BEFORE reparenting

import unreal
import json
import re

OUTPUT_FILE = "C:/scripts/SLFConversion/action_data.json"

def extract_action_data():
    """
    Extract all data from DA_Action_* assets using export_text().
    This preserves data before reparenting PDA_Action to UPDA_Action.
    """
    print("")
    print("=" * 60)
    print("EXTRACTING ACTION DATA FROM ACTION ASSETS")
    print("=" * 60)

    action_data = {}
    actions_path = "/Game/SoulslikeFramework/Data/Actions/ActionData"

    assets = unreal.EditorAssetLibrary.list_assets(actions_path, recursive=True, include_folder=False)

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]

        # Skip the parent PDA_Action Blueprint
        if asset_name == "PDA_Action":
            print(f"\n[SKIP] {asset_name} - Parent Blueprint")
            continue

        print(f"\nProcessing: {asset_name}")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            print(f"  [ERROR] Could not load")
            continue

        asset_class = asset.get_class()
        print(f"  Class: {asset_class.get_name()}")

        try:
            data = {
                "asset_path": asset_path,
                "class_name": asset_class.get_name()
            }

            # Extract ActionClass (TSoftClassPtr)
            try:
                action_class = asset.get_editor_property('ActionClass')
                if action_class:
                    # Get the path string from the soft class ptr
                    class_path = str(action_class)
                    if class_path and class_path != "None":
                        data["ActionClass"] = class_path
                        print(f"  ActionClass: {class_path}")
            except Exception as e:
                print(f"  ActionClass: [not found] {e}")

            # Extract ActionMontage (TSoftObjectPtr)
            try:
                action_montage = asset.get_editor_property('ActionMontage')
                if action_montage:
                    montage_path = str(action_montage)
                    if montage_path and montage_path != "None":
                        data["ActionMontage"] = montage_path
                        print(f"  ActionMontage: {montage_path}")
            except Exception as e:
                print(f"  ActionMontage: [not found] {e}")

            # Extract StaminaCost
            try:
                stamina_cost = asset.get_editor_property('StaminaCost')
                if stamina_cost is not None:
                    data["StaminaCost"] = float(stamina_cost)
                    print(f"  StaminaCost: {stamina_cost}")
            except:
                pass

            # Extract bCanBeInterrupted
            try:
                can_interrupt = asset.get_editor_property('bCanBeInterrupted')
                if can_interrupt is not None:
                    data["bCanBeInterrupted"] = bool(can_interrupt)
                    print(f"  bCanBeInterrupted: {can_interrupt}")
            except:
                pass

            # Extract RequiredStatTag (GameplayTag)
            try:
                stat_tag = asset.get_editor_property('RequiredStatTag')
                if stat_tag:
                    tag_name = str(stat_tag.tag_name) if hasattr(stat_tag, 'tag_name') else str(stat_tag)
                    if tag_name and tag_name != "None" and tag_name != "":
                        data["RequiredStatTag"] = tag_name
                        print(f"  RequiredStatTag: {tag_name}")
            except:
                pass

            # Extract RequiredStatAmount
            try:
                stat_amount = asset.get_editor_property('RequiredStatAmount')
                if stat_amount is not None and stat_amount != 0:
                    data["RequiredStatAmount"] = float(stat_amount)
                    print(f"  RequiredStatAmount: {stat_amount}")
            except:
                pass

            # Extract RelevantData (FInstancedStruct) via export_text()
            # This is the complex one - we need the full struct contents
            try:
                relevant_data = asset.get_editor_property('RelevantData')
                if relevant_data and hasattr(relevant_data, 'export_text'):
                    export = relevant_data.export_text()
                    if export and export.strip() and export != "()":
                        data["RelevantData_Export"] = export
                        print(f"  RelevantData: {export[:80]}...")
            except Exception as e:
                print(f"  RelevantData: [export failed] {e}")

            # Only save if we got some data
            if len(data) > 2:  # More than just asset_path and class_name
                action_data[asset_name] = data
                print(f"  [OK] Extracted {len(data)} properties")
            else:
                print(f"  [WARN] No data extracted")

        except Exception as e:
            print(f"  [ERROR] {e}")
            import traceback
            traceback.print_exc()

    # Save to JSON file
    print("")
    print("=" * 60)
    print(f"SAVING TO {OUTPUT_FILE}")
    print("=" * 60)

    with open(OUTPUT_FILE, 'w') as f:
        json.dump(action_data, f, indent=2)

    print(f"\nExtracted data from {len(action_data)} action assets")

    return action_data


if __name__ == "__main__":
    extract_action_data()
