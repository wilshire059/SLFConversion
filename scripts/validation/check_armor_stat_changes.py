# check_armor_stat_changes.py
# Check what StatChanges armor items have and if Weight stat exists
import unreal

def tag_to_string(tag):
    """Convert a GameplayTag to string."""
    if tag and hasattr(tag, 'tag_name'):
        return str(tag.tag_name)
    # Try to get as string
    try:
        return tag.get_editor_property("tag_name").to_string()
    except:
        pass
    try:
        # Last resort - use repr and extract
        tag_repr = str(tag)
        return tag_repr
    except:
        return "UNKNOWN"

def check_armor_stat_changes():
    """Check StatChanges for all armor items."""

    armor_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor02",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleHat",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleBracers",
        "/Game/SoulslikeFramework/Data/Items/DA_ExampleGreaves",
    ]

    output_file = "C:/scripts/SLFConversion/migration_cache/armor_stat_changes_check.txt"

    with open(output_file, 'w') as f:
        f.write("="*80 + "\n")
        f.write("ARMOR STAT CHANGES ANALYSIS\n")
        f.write("="*80 + "\n\n")

        weight_tag_found = False

        for path in armor_paths:
            asset = unreal.EditorAssetLibrary.load_asset(path)
            if not asset:
                f.write(f"[MISSING] {path}\n")
                continue

            item_name = path.split("/")[-1]
            f.write(f"[{item_name}]\n")

            # Try to access ItemInformation struct
            try:
                item_info = asset.get_editor_property("item_information")
                if item_info:
                    equip_details = item_info.get_editor_property("equipment_details")
                    if equip_details:
                        stat_changes = equip_details.get_editor_property("stat_changes")

                        if stat_changes and len(stat_changes) > 0:
                            f.write(f"  StatChanges count: {len(stat_changes)}\n")
                            for tag, stat_data in stat_changes.items():
                                # Get the key tag name
                                key_tag_name = "UNKNOWN"
                                try:
                                    key_tag_name = tag.get_editor_property("tag_name")
                                    if hasattr(key_tag_name, 'to_string'):
                                        key_tag_name = key_tag_name.to_string()
                                    else:
                                        key_tag_name = str(key_tag_name)
                                except:
                                    key_tag_name = str(tag)

                                # Get the stat_tag from FSLFEquipmentStat
                                stat_tag_name = "UNKNOWN"
                                try:
                                    stat_tag = stat_data.get_editor_property("stat_tag")
                                    stat_tag_name = stat_tag.get_editor_property("tag_name")
                                    if hasattr(stat_tag_name, 'to_string'):
                                        stat_tag_name = stat_tag_name.to_string()
                                    else:
                                        stat_tag_name = str(stat_tag_name)
                                except:
                                    pass

                                delta = 0.0
                                try:
                                    delta = stat_data.get_editor_property("delta")
                                except:
                                    pass

                                # Check for Weight tag
                                if "Weight" in key_tag_name or "Weight" in stat_tag_name:
                                    weight_tag_found = True
                                    f.write(f"    *** WEIGHT FOUND ***\n")

                                f.write(f"    - Key: {key_tag_name}, StatTag: {stat_tag_name}, Delta: {delta}\n")
                        else:
                            f.write("  StatChanges: EMPTY\n")

                    else:
                        f.write("  EquipmentDetails: NULL\n")
                else:
                    f.write("  ItemInformation: NULL\n")
            except Exception as e:
                f.write(f"  ERROR: {e}\n")

            f.write("\n")

        f.write("="*80 + "\n")
        if weight_tag_found:
            f.write("WEIGHT TAG: FOUND in at least one item\n")
        else:
            f.write("WEIGHT TAG: NOT FOUND - This is why Equip Load stays at 0!\n")
        f.write("="*80 + "\n")

    unreal.log(f"Output saved to: {output_file}")

if __name__ == "__main__":
    check_armor_stat_changes()
