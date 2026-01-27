# fix_b_weight_statinfo.py
# Directly set StatInfo values on B_Weight Blueprint CDO
import unreal

def fix_b_weight_statinfo():
    """Set StatInfo values directly on B_Weight Blueprint CDO."""

    output_file = "C:/scripts/SLFConversion/migration_cache/b_weight_statinfo_fix.txt"

    with open(output_file, 'w') as f:
        f.write("="*80 + "\n")
        f.write("B_WEIGHT STATINFO DIRECT FIX\n")
        f.write("="*80 + "\n\n")

        # Load B_Weight
        weight_bp_path = "/Game/SoulslikeFramework/Data/Stats/Misc/B_Weight"
        weight_bp = unreal.EditorAssetLibrary.load_asset(weight_bp_path)

        if not weight_bp:
            f.write(f"ERROR: B_Weight not found at {weight_bp_path}\n")
            return

        f.write(f"Loaded: {weight_bp.get_name()}\n")

        # Get generated class and CDO
        gen_class = weight_bp.generated_class()
        cdo = unreal.get_default_object(gen_class)
        f.write(f"CDO: {cdo.get_name()}\n")

        # Get current StatInfo
        stat_info = cdo.get_editor_property("stat_info")
        f.write(f"\nBefore fix:\n")
        try:
            tag = stat_info.get_editor_property("tag")
            tag_name = tag.get_editor_property("tag_name") if tag else "NONE"
            f.write(f"  Tag: {tag_name}\n")
            f.write(f"  CurrentValue: {stat_info.get_editor_property('current_value')}\n")
            f.write(f"  MaxValue: {stat_info.get_editor_property('max_value')}\n")
        except Exception as e:
            f.write(f"  Error reading: {e}\n")

        # Set the tag using GameplayTag
        f.write("\nSetting StatInfo values...\n")
        try:
            # Create the gameplay tag
            weight_tag = unreal.GameplayTag.request_gameplay_tag("SoulslikeFramework.Stat.Misc.Weight")
            f.write(f"Created tag: {weight_tag.get_editor_property('tag_name')}\n")

            # Set tag on StatInfo
            stat_info.set_editor_property("tag", weight_tag)
            f.write("Set tag on StatInfo\n")

            # Set other values
            stat_info.set_editor_property("display_name", unreal.Text("Equip Load"))
            stat_info.set_editor_property("current_value", 0.0)
            stat_info.set_editor_property("max_value", 50.0)
            stat_info.set_editor_property("show_max_value", True)
            stat_info.set_editor_property("display_as_percent", False)
            f.write("Set display_name, current_value, max_value, show_max_value, display_as_percent\n")

            # Apply StatInfo back to CDO
            cdo.set_editor_property("stat_info", stat_info)
            f.write("Applied StatInfo to CDO\n")

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(weight_bp_path)
            f.write("Asset saved\n")

        except Exception as e:
            f.write(f"Error setting values: {e}\n")

        # Verify
        f.write("\nAfter fix:\n")
        try:
            weight_bp2 = unreal.EditorAssetLibrary.load_asset(weight_bp_path)
            gen_class2 = weight_bp2.generated_class()
            cdo2 = unreal.get_default_object(gen_class2)
            stat_info2 = cdo2.get_editor_property("stat_info")

            tag2 = stat_info2.get_editor_property("tag")
            tag_name2 = tag2.get_editor_property("tag_name") if tag2 else "NONE"
            f.write(f"  Tag: {tag_name2}\n")
            f.write(f"  CurrentValue: {stat_info2.get_editor_property('current_value')}\n")
            f.write(f"  MaxValue: {stat_info2.get_editor_property('max_value')}\n")

            if str(tag_name2) == "SoulslikeFramework.Stat.Misc.Weight":
                f.write("\n*** SUCCESS: StatInfo now has correct values! ***\n")
            else:
                f.write("\n*** STILL FAILED - Tag not set correctly ***\n")
        except Exception as e:
            f.write(f"Verify error: {e}\n")

        f.write("\n" + "="*80 + "\n")

    unreal.log(f"Output saved to: {output_file}")

if __name__ == "__main__":
    fix_b_weight_statinfo()
