# check_weapon_weight.py
# Check if weapons have Weight in StatChanges
import unreal

def check_weapons():
    output_file = "C:/scripts/SLFConversion/migration_cache/weapon_weight_check.txt"

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

            try:
                item_info = asset.get_editor_property("item_information")
                f.write(f"  ItemInformation: FOUND\n")

                try:
                    equip_details = item_info.get_editor_property("equipment_details")
                    f.write(f"  EquipmentDetails: FOUND\n")

                    try:
                        stat_changes = equip_details.get_editor_property("stat_changes")
                        f.write(f"  StatChanges count: {len(stat_changes)}\n")

                        has_weight = False
                        for key, value in stat_changes.items():
                            tag_str = key.get_editor_property("tag_name") if key else "NONE"
                            delta = value.get_editor_property("delta") if value else 0.0
                            f.write(f"    - {tag_str}: {delta}\n")
                            if "Weight" in str(tag_str):
                                has_weight = True

                        if not has_weight:
                            f.write(f"  *** NO WEIGHT STAT ***\n")

                    except Exception as e:
                        f.write(f"  StatChanges error: {e}\n")
                except Exception as e:
                    f.write(f"  EquipmentDetails error: {e}\n")
            except Exception as e:
                f.write(f"  ItemInformation error: {e}\n")

            f.write("\n")

        f.write("="*80 + "\n")

    unreal.log(f"Output saved to: {output_file}")

if __name__ == "__main__":
    check_weapons()
