# check_weight_stat.py
# Check the Weight stat row in DataTable and verify loading
import unreal

def check_weight_stat():
    """Check the Weight stat in DataTable."""

    output_file = "C:/scripts/SLFConversion/migration_cache/weight_stat_check.txt"

    table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable"
    table = unreal.EditorAssetLibrary.load_asset(table_path)

    with open(output_file, 'w') as f:
        f.write("="*80 + "\n")
        f.write("WEIGHT STAT INVESTIGATION\n")
        f.write("="*80 + "\n\n")

        if not table:
            f.write(f"FAILED to load: {table_path}\n")
            return

        # Get all row names
        row_names = unreal.DataTableFunctionLibrary.get_data_table_row_names(table)
        f.write(f"Total rows: {len(row_names)}\n\n")

        # Find Weight-related row
        for row_name in row_names:
            name_str = str(row_name)
            if "Equip_Load" in name_str or "Weight" in name_str:
                f.write(f"Found Weight row: [{row_name}]\n")

        # Check the B_Weight Blueprint class
        f.write("\n--- B_Weight Class Check ---\n")
        weight_bp_path = "/Game/SoulslikeFramework/Data/Stats/_Backend/B_Weight"
        weight_bp = unreal.EditorAssetLibrary.load_asset(weight_bp_path)
        if weight_bp:
            f.write(f"B_Weight loaded: {weight_bp.get_name()}\n")
            f.write(f"B_Weight class: {weight_bp.get_class().get_name()}\n")

            # Try to get parent class
            gen_class = weight_bp.generated_class() if hasattr(weight_bp, 'generated_class') else None
            if gen_class:
                f.write(f"Generated class: {gen_class.get_name()}\n")
                parent = gen_class.get_super_class()
                f.write(f"Parent class: {parent.get_name() if parent else 'None'}\n")
        else:
            f.write(f"B_Weight NOT FOUND at: {weight_bp_path}\n")

        # Check the C++ class
        f.write("\n--- C++ USLFStatWeight Check ---\n")
        try:
            weight_cpp_class = unreal.load_class(None, "/Script/SLFConversion.SLFStatWeight")
            if weight_cpp_class:
                f.write(f"USLFStatWeight class loaded: {weight_cpp_class.get_name()}\n")
                # Try to get default object
                cdo = unreal.get_default_object(weight_cpp_class)
                if cdo:
                    f.write(f"CDO: {cdo.get_name()}\n")
                    # Try to get StatInfo
                    try:
                        stat_info = cdo.get_editor_property("stat_info")
                        if stat_info:
                            tag = stat_info.get_editor_property("tag")
                            tag_name = tag.get_editor_property("tag_name") if tag else "NONE"
                            current = stat_info.get_editor_property("current_value")
                            max_val = stat_info.get_editor_property("max_value")
                            f.write(f"StatInfo.Tag: {tag_name}\n")
                            f.write(f"StatInfo.CurrentValue: {current}\n")
                            f.write(f"StatInfo.MaxValue: {max_val}\n")
                    except Exception as e:
                        f.write(f"Error getting StatInfo: {e}\n")
            else:
                f.write("USLFStatWeight class NOT FOUND\n")
        except Exception as e:
            f.write(f"Error loading USLFStatWeight: {e}\n")

        # Check if armor StatChanges uses matching tag
        f.write("\n--- Armor StatChanges Tag Check ---\n")
        armor_path = "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor"
        armor = unreal.EditorAssetLibrary.load_asset(armor_path)
        if armor:
            try:
                item_info = armor.get_editor_property("item_information")
                equip_details = item_info.get_editor_property("equipment_details")
                stat_changes = equip_details.get_editor_property("stat_changes")

                for tag, stat_data in stat_changes.items():
                    key_tag_name = tag.get_editor_property("tag_name")
                    if "Weight" in str(key_tag_name):
                        delta = stat_data.get_editor_property("delta")
                        f.write(f"Armor Weight StatChange key: {key_tag_name}\n")
                        f.write(f"Armor Weight Delta: {delta}\n")
            except Exception as e:
                f.write(f"Error checking armor: {e}\n")

        f.write("\n" + "="*80 + "\n")
        f.write("EXPECTED: Both C++ and armor should use tag 'SoulslikeFramework.Stat.Misc.Weight'\n")
        f.write("="*80 + "\n")

    unreal.log(f"Output saved to: {output_file}")

if __name__ == "__main__":
    check_weight_stat()
