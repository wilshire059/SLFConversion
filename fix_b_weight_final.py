# fix_b_weight_final.py
# Copy StatInfo from C++ parent CDO to B_Weight Blueprint CDO
import unreal

def fix_b_weight():
    """Copy StatInfo from native C++ parent to Blueprint CDO."""

    output_file = "C:/scripts/SLFConversion/migration_cache/b_weight_fix_final.txt"

    with open(output_file, 'w') as f:
        f.write("="*80 + "\n")
        f.write("B_WEIGHT FIX FINAL\n")
        f.write("="*80 + "\n\n")

        weight_bp_path = "/Game/SoulslikeFramework/Data/Stats/Misc/B_Weight"
        weight_bp = unreal.EditorAssetLibrary.load_asset(weight_bp_path)

        if not weight_bp:
            f.write(f"ERROR: B_Weight not found\n")
            return

        # Get Blueprint CDO
        gen_class = weight_bp.generated_class()
        bp_cdo = unreal.get_default_object(gen_class)

        # Get native C++ parent CDO
        native_class = unreal.find_object(None, "/Script/SLFConversion.SLFStatWeight")
        if not native_class:
            f.write("ERROR: Native SLFStatWeight class not found\n")
            return

        native_cdo = unreal.get_default_object(native_class)
        native_stat_info = native_cdo.get_editor_property("stat_info")

        f.write(f"Native StatInfo.Tag: {native_stat_info.get_editor_property('tag').get_editor_property('tag_name')}\n")
        f.write(f"Native StatInfo.MaxValue: {native_stat_info.get_editor_property('max_value')}\n")

        # Copy StatInfo to Blueprint CDO
        bp_cdo.set_editor_property("stat_info", native_stat_info)
        f.write("\nCopied StatInfo to Blueprint CDO\n")

        # Save
        saved = unreal.EditorAssetLibrary.save_asset(weight_bp_path)
        f.write(f"Save result: {saved}\n")

        # Verify
        weight_bp2 = unreal.EditorAssetLibrary.load_asset(weight_bp_path)
        gen_class2 = weight_bp2.generated_class()
        cdo2 = unreal.get_default_object(gen_class2)
        stat_info2 = cdo2.get_editor_property("stat_info")
        tag2 = stat_info2.get_editor_property("tag")
        tag_name2 = tag2.get_editor_property("tag_name") if tag2 else "NONE"

        f.write(f"\nVerify - Tag: {tag_name2}\n")
        f.write(f"Verify - MaxValue: {stat_info2.get_editor_property('max_value')}\n")

        if str(tag_name2) == "SoulslikeFramework.Stat.Misc.Weight":
            f.write("\n*** SUCCESS ***\n")
        else:
            f.write("\n*** FAILED ***\n")

        f.write("\n" + "="*80 + "\n")

    unreal.log(f"Output: {output_file}")

if __name__ == "__main__":
    fix_b_weight()
