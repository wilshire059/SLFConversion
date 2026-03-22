# check_armor_current_state.py
# Check current state of armor StatChanges in SLFConversion project
import unreal

def check_armor():
    """Check armor items' StatChanges in current project."""

    output_file = "C:/scripts/SLFConversion/migration_cache/armor_current_state.txt"

    armor_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor02",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleHat",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleBracers",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleGreaves",
    ]

    with open(output_file, 'w') as f:
        f.write("="*80 + "\n")
        f.write("ARMOR STAT CHANGES - CURRENT STATE (SLFConversion)\n")
        f.write("="*80 + "\n\n")

        for path in armor_paths:
            asset = unreal.EditorAssetLibrary.load_asset(path)
            if not asset:
                f.write(f"[{path.split('/')[-1]}] NOT FOUND\n\n")
                continue

            f.write(f"[{asset.get_name()}]\n")

            # Get ItemInformation
            try:
                item_info = asset.get_editor_property("item_information")
                f.write(f"  ItemInformation: FOUND\n")

                # Get EquipmentDetails
                try:
                    equip_details = item_info.get_editor_property("equipment_details")
                    f.write(f"  EquipmentDetails: FOUND\n")

                    # Get StatChanges
                    try:
                        stat_changes = equip_details.get_editor_property("stat_changes")
                        f.write(f"  StatChanges count: {len(stat_changes)}\n")

                        if len(stat_changes) == 0:
                            f.write(f"  *** EMPTY - Stats lost during migration! ***\n")
                        else:
                            for key, value in stat_changes.items():
                                tag_str = key.get_editor_property("tag_name") if key else "NONE"
                                stat_tag = value.get_editor_property("stat_tag") if value else None
                                stat_tag_str = stat_tag.get_editor_property("tag_name") if stat_tag else "NONE"
                                delta = value.get_editor_property("delta") if value else 0.0
                                f.write(f"    - Key: {tag_str}, StatTag: {stat_tag_str}, Delta: {delta}\n")

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
    check_armor()
