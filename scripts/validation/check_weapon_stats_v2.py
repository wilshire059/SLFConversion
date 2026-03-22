# check_weapon_stats_v2.py
# Check weapon StatChanges - simplified approach
import unreal

def check_weapons():
    output_file = "C:/scripts/SLFConversion/migration_cache/weapon_stats_check.txt"

    weapon_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
        "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
        "/Game/SoulslikeFramework/Data/Items/DA_Katana",
        "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
        "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
        "/Game/SoulslikeFramework/Data/Items/DA_BossMace",
    ]

    with open(output_file, 'w') as f:
        f.write("="*80 + "\n")
        f.write("WEAPON STAT CHANGES CHECK\n")
        f.write("="*80 + "\n\n")

        for path in weapon_paths:
            asset = unreal.EditorAssetLibrary.load_asset(path)
            if not asset:
                f.write(f"[{path.split('/')[-1]}] NOT FOUND\n\n")
                continue

            f.write(f"[{asset.get_name()}]\n")
            f.write(f"  Class: {asset.get_class().get_name()}\n")

            # Get ItemInformation (C++ property name, snake_case in Python)
            try:
                item_info = asset.get_editor_property("item_information")
                f.write(f"  ItemInformation: FOUND ({type(item_info).__name__})\n")

                # Get EquipmentDetails
                try:
                    equip_details = item_info.get_editor_property("equipment_details")
                    f.write(f"  EquipmentDetails: FOUND ({type(equip_details).__name__})\n")

                    # Get StatChanges
                    try:
                        stat_changes = equip_details.get_editor_property("stat_changes")
                        f.write(f"  StatChanges count: {len(stat_changes)}\n")

                        has_weight = False
                        for key, value in stat_changes.items():
                            try:
                                tag_str = key.tag_name if hasattr(key, 'tag_name') else str(key)
                            except:
                                tag_str = str(key)
                            try:
                                delta = value.delta if hasattr(value, 'delta') else value.get_editor_property("delta")
                            except:
                                delta = str(value)
                            f.write(f"    - {tag_str}: {delta}\n")
                            if "Weight" in str(tag_str):
                                has_weight = True

                        if not has_weight and len(stat_changes) == 0:
                            f.write(f"  *** NO STAT CHANGES ***\n")
                        elif not has_weight:
                            f.write(f"  *** NO WEIGHT STAT ***\n")

                    except Exception as e:
                        f.write(f"  StatChanges error: {e}\n")
                except Exception as e:
                    f.write(f"  EquipmentDetails error: {e}\n")
            except Exception as e:
                f.write(f"  ItemInformation error: {e}\n")

            f.write("\n")

        # Compare with armor
        f.write("="*80 + "\n")
        f.write("ARMOR COMPARISON\n")
        f.write("="*80 + "\n\n")

        armor_path = "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor"
        armor = unreal.EditorAssetLibrary.load_asset(armor_path)
        if armor:
            f.write(f"[{armor.get_name()}]\n")
            f.write(f"  Class: {armor.get_class().get_name()}\n")

            try:
                item_info = armor.get_editor_property("item_information")
                equip_details = item_info.get_editor_property("equipment_details")
                stat_changes = equip_details.get_editor_property("stat_changes")
                f.write(f"  StatChanges count: {len(stat_changes)}\n")
                for key, value in stat_changes.items():
                    try:
                        tag_str = key.tag_name if hasattr(key, 'tag_name') else str(key)
                    except:
                        tag_str = str(key)
                    try:
                        delta = value.delta if hasattr(value, 'delta') else value.get_editor_property("delta")
                    except:
                        delta = str(value)
                    f.write(f"    - {tag_str}: {delta}\n")
            except Exception as e:
                f.write(f"  Error: {e}\n")

        f.write("\n" + "="*80 + "\n")

    unreal.log(f"Output saved to: {output_file}")

if __name__ == "__main__":
    check_weapons()
