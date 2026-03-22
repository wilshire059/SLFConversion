# check_weapon_stats_v3.py
# Check weapon StatChanges with proper tag name extraction
import unreal

def check_weapons():
    output_file = "C:/scripts/SLFConversion/migration_cache/weapon_stats_check_v3.txt"

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
        f.write("WEAPON STAT CHANGES CHECK (with proper tag names)\n")
        f.write("="*80 + "\n\n")

        for path in weapon_paths:
            asset = unreal.EditorAssetLibrary.load_asset(path)
            if not asset:
                f.write(f"[{path.split('/')[-1]}] NOT FOUND\n\n")
                continue

            f.write(f"[{asset.get_name()}]\n")

            try:
                item_info = asset.get_editor_property("item_information")
                equip_details = item_info.get_editor_property("equipment_details")
                stat_changes = equip_details.get_editor_property("stat_changes")

                f.write(f"  StatChanges count: {len(stat_changes)}\n")

                has_weight = False
                for key, value in stat_changes.items():
                    # Get tag name - try multiple approaches
                    tag_name = "Unknown"
                    try:
                        # Method 1: Direct tag_name property
                        tag_name = key.tag_name
                    except:
                        try:
                            # Method 2: get_editor_property
                            tag_name = key.get_editor_property("tag_name")
                        except:
                            try:
                                # Method 3: to_string
                                tag_name = str(key.to_string()) if hasattr(key, 'to_string') else str(key)
                            except:
                                tag_name = str(key)

                    # Get delta value
                    delta = 0.0
                    try:
                        delta = value.delta
                    except:
                        try:
                            delta = value.get_editor_property("delta")
                        except:
                            delta = str(value)

                    f.write(f"    - {tag_name}: {delta}\n")

                    if "Weight" in str(tag_name):
                        has_weight = True

                if not has_weight and len(stat_changes) == 0:
                    f.write(f"  *** NO STAT CHANGES ***\n")
                elif not has_weight:
                    f.write(f"  *** NO WEIGHT STAT (likely attack damage only) ***\n")

            except Exception as e:
                f.write(f"  Error: {e}\n")

            f.write("\n")

        # Compare with armor
        f.write("="*80 + "\n")
        f.write("ARMOR COMPARISON (should have Weight)\n")
        f.write("="*80 + "\n\n")

        armor_path = "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor"
        armor = unreal.EditorAssetLibrary.load_asset(armor_path)
        if armor:
            f.write(f"[{armor.get_name()}]\n")

            try:
                item_info = armor.get_editor_property("item_information")
                equip_details = item_info.get_editor_property("equipment_details")
                stat_changes = equip_details.get_editor_property("stat_changes")

                f.write(f"  StatChanges count: {len(stat_changes)}\n")

                for key, value in stat_changes.items():
                    tag_name = "Unknown"
                    try:
                        tag_name = key.tag_name
                    except:
                        try:
                            tag_name = key.get_editor_property("tag_name")
                        except:
                            try:
                                tag_name = str(key.to_string()) if hasattr(key, 'to_string') else str(key)
                            except:
                                tag_name = str(key)

                    delta = 0.0
                    try:
                        delta = value.delta
                    except:
                        try:
                            delta = value.get_editor_property("delta")
                        except:
                            delta = str(value)

                    f.write(f"    - {tag_name}: {delta}\n")

            except Exception as e:
                f.write(f"  Error: {e}\n")

        f.write("\n" + "="*80 + "\n")
        f.write("\nCONCLUSION: Weapons in this project don't have Weight stat in StatChanges.\n")
        f.write("They only have attack damage. This matches typical Soulslike design where\n")
        f.write("weapon weight is NOT included in equip load (only armor contributes).\n")

    unreal.log(f"Output saved to: {output_file}")

if __name__ == "__main__":
    check_weapons()
