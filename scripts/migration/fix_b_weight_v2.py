# fix_b_weight_v2.py
# Use C++ automation to set StatInfo on B_Weight
import unreal

def fix_b_weight():
    """Use C++ automation library to set StatInfo values."""

    output_file = "C:/scripts/SLFConversion/migration_cache/b_weight_fix_v2.txt"

    with open(output_file, 'w') as f:
        f.write("="*80 + "\n")
        f.write("B_WEIGHT FIX V2 (C++ Automation)\n")
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

        # Check for SetStatInfoFromParent C++ function
        f.write("\nChecking C++ automation functions...\n")

        # Try calling SetStatInfoFromParent if it exists
        try:
            success = unreal.SLFAutomationLibrary.set_stat_info_from_parent(weight_bp)
            f.write(f"SetStatInfoFromParent result: {success}\n")
        except Exception as e:
            f.write(f"SetStatInfoFromParent not available: {e}\n")

        # Alternative: Try to create a GameplayTag container struct
        f.write("\nTrying to create GameplayTag via struct...\n")
        try:
            # Create FGameplayTagContainer and try to add tag
            container = unreal.GameplayTagContainer()
            container.add_tag(unreal.Name("SoulslikeFramework.Stat.Misc.Weight"))
            f.write(f"Created container: {container}\n")
        except Exception as e:
            f.write(f"Container creation error: {e}\n")

        # Try getting StatInfo and checking its structure
        f.write("\nChecking StatInfo struct attributes...\n")
        try:
            stat_info = cdo.get_editor_property("stat_info")
            f.write(f"StatInfo type: {type(stat_info)}\n")

            # List available properties
            if hasattr(stat_info, '__dir__'):
                props = [p for p in dir(stat_info) if not p.startswith('_')]
                f.write(f"StatInfo dir (first 20): {props[:20]}\n")
        except Exception as e:
            f.write(f"Error checking StatInfo: {e}\n")

        # Get the native parent class CDO
        f.write("\nGetting C++ parent class CDO...\n")
        try:
            # Load the native USLFStatWeight class
            native_class = unreal.find_object(None, "/Script/SLFConversion.SLFStatWeight")
            if native_class:
                f.write(f"Found native class: {native_class.get_name()}\n")
                native_cdo = unreal.get_default_object(native_class)
                if native_cdo:
                    native_stat_info = native_cdo.get_editor_property("stat_info")
                    native_tag = native_stat_info.get_editor_property("tag")
                    native_tag_name = native_tag.get_editor_property("tag_name") if native_tag else "NONE"
                    f.write(f"Native CDO StatInfo.Tag: {native_tag_name}\n")
                    f.write(f"Native CDO MaxValue: {native_stat_info.get_editor_property('max_value')}\n")

                    # Try to copy the struct
                    f.write("\nCopying native StatInfo to Blueprint CDO...\n")
                    cdo.set_editor_property("stat_info", native_stat_info)
                    f.write("Set stat_info from native CDO\n")

                    # Save asset
                    unreal.EditorAssetLibrary.save_asset(weight_bp_path)
                    f.write("Asset saved\n")
            else:
                f.write("Native class not found\n")
        except Exception as e:
            f.write(f"Error with native CDO: {e}\n")

        # Verify
        f.write("\nVerifying...\n")
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
                f.write("\n*** SUCCESS: StatInfo now has correct tag! ***\n")
            else:
                f.write("\n*** STILL FAILED ***\n")
        except Exception as e:
            f.write(f"Verify error: {e}\n")

        f.write("\n" + "="*80 + "\n")

    unreal.log(f"Output saved to: {output_file}")

if __name__ == "__main__":
    fix_b_weight()
