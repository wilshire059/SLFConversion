# extract_armor_stat_changes.py
# Extract StatChanges from armor items in bp_only
# Run on bp_only project to get the expected values
import unreal
import json
import os

def extract_armor_stat_changes():
    """Extract StatChanges for all armor items from bp_only."""

    armor_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor02",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleHat",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleBracers",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleGreaves",
    ]

    results = {}

    print("\n" + "="*80)
    print("EXTRACTING ARMOR STAT CHANGES FROM bp_only")
    print("="*80)

    for path in armor_paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if not asset:
            print(f"\n[MISSING] {path}")
            continue

        item_name = path.split("/")[-1]
        print(f"\n[{item_name}]")

        item_data = {
            "stat_changes": {},
            "damage_negation_stats": {},
            "resistance_stats": {}
        }

        # Use export_text to get all data
        try:
            text = unreal.EditorAssetLibrary.get_metadata_tag(asset, "ItemInformation")
            print(f"  Metadata: {text if text else 'None'}")
        except:
            pass

        # Try different property access approaches
        try:
            # Direct property access for Blueprint data assets
            item_info = None

            # Try getting the struct property
            if hasattr(asset, 'item_information'):
                item_info = asset.item_information
            elif hasattr(asset, 'ItemInformation'):
                item_info = asset.ItemInformation

            if item_info is None:
                # Try via get_editor_property
                try:
                    item_info = asset.get_editor_property("item_information")
                except:
                    try:
                        item_info = asset.get_editor_property("ItemInformation")
                    except:
                        pass

            if item_info:
                print(f"  ItemInformation: FOUND")

                # Get EquipmentDetails
                equip_details = None
                try:
                    equip_details = item_info.get_editor_property("equipment_details")
                except:
                    try:
                        equip_details = item_info.equipment_details
                    except:
                        pass

                if equip_details:
                    print(f"  EquipmentDetails: FOUND")

                    # Get StatChanges
                    try:
                        stat_changes = equip_details.get_editor_property("stat_changes")
                        if stat_changes:
                            print(f"  StatChanges count: {len(stat_changes)}")
                            for tag, stat_data in stat_changes.items():
                                tag_str = str(tag)
                                if hasattr(stat_data, 'delta'):
                                    delta = stat_data.delta
                                else:
                                    delta = stat_data.get_editor_property("delta") if hasattr(stat_data, 'get_editor_property') else 0
                                item_data["stat_changes"][tag_str] = delta
                                print(f"    - {tag_str}: {delta}")
                        else:
                            print("  StatChanges: EMPTY")
                    except Exception as e:
                        print(f"  StatChanges ERROR: {e}")

                    # Get DamageNegationStats
                    try:
                        dmg_negation = equip_details.get_editor_property("damage_negation_stats")
                        if dmg_negation:
                            print(f"  DamageNegationStats count: {len(dmg_negation)}")
                            for tag, value in dmg_negation.items():
                                tag_str = str(tag)
                                item_data["damage_negation_stats"][tag_str] = value
                                print(f"    - {tag_str}: {value}")
                        else:
                            print("  DamageNegationStats: EMPTY")
                    except Exception as e:
                        print(f"  DamageNegationStats ERROR: {e}")

                    # Get ResistanceStats
                    try:
                        resistance = equip_details.get_editor_property("resistance_stats")
                        if resistance:
                            print(f"  ResistanceStats count: {len(resistance)}")
                            for tag, value in resistance.items():
                                tag_str = str(tag)
                                item_data["resistance_stats"][tag_str] = value
                                print(f"    - {tag_str}: {value}")
                        else:
                            print("  ResistanceStats: EMPTY")
                    except Exception as e:
                        print(f"  ResistanceStats ERROR: {e}")
                else:
                    print("  EquipmentDetails: NULL")
            else:
                print("  ItemInformation: NULL")

        except Exception as e:
            print(f"  ERROR: {e}")
            import traceback
            traceback.print_exc()

        results[item_name] = item_data

    # Save to cache
    output_path = "C:/scripts/SLFConversion/migration_cache/armor_stat_changes.json"
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)

    print(f"\nSaved to: {output_path}")
    print("="*80 + "\n")

if __name__ == "__main__":
    extract_armor_stat_changes()
