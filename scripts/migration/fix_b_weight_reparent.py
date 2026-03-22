# fix_b_weight_reparent.py
# Reparent B_Weight to USLFStatWeight using C++ automation
import unreal

def fix_b_weight():
    """Reparent B_Weight to USLFStatWeight using C++ function."""

    output_file = "C:/scripts/SLFConversion/migration_cache/b_weight_fix.txt"

    with open(output_file, 'w') as f:
        f.write("="*80 + "\n")
        f.write("B_WEIGHT REPARENT FIX (C++ Automation)\n")
        f.write("="*80 + "\n\n")

        # Load B_Weight
        weight_bp_path = "/Game/SoulslikeFramework/Data/Stats/Misc/B_Weight"
        weight_bp = unreal.EditorAssetLibrary.load_asset(weight_bp_path)

        if not weight_bp:
            f.write(f"ERROR: B_Weight not found at {weight_bp_path}\n")
            return

        f.write(f"Loaded: {weight_bp.get_name()}\n")

        # Use C++ automation library
        f.write("\nUsing SLFAutomationLibrary::ReparentBlueprint...\n")
        try:
            # Call the C++ function
            success = unreal.SLFAutomationLibrary.reparent_blueprint(
                weight_bp,
                "/Script/SLFConversion.SLFStatWeight"
            )
            if success:
                f.write("Reparent: SUCCESS\n")

                # Save the asset
                unreal.EditorAssetLibrary.save_asset(weight_bp_path)
                f.write("Asset saved\n")
            else:
                f.write("Reparent: FAILED (returned false)\n")
        except Exception as e:
            f.write(f"Reparent error: {e}\n")

        # Verify
        f.write("\nVerifying...\n")
        try:
            weight_bp2 = unreal.EditorAssetLibrary.load_asset(weight_bp_path)
            gen_class2 = weight_bp2.generated_class()
            cdo = unreal.get_default_object(gen_class2)
            stat_info = cdo.get_editor_property("stat_info")
            if stat_info:
                tag = stat_info.get_editor_property("tag")
                tag_name = tag.get_editor_property("tag_name") if tag else "NONE"
                current = stat_info.get_editor_property("current_value")
                max_val = stat_info.get_editor_property("max_value")
                f.write(f"StatInfo.Tag: {tag_name}\n")
                f.write(f"StatInfo.CurrentValue: {current}\n")
                f.write(f"StatInfo.MaxValue: {max_val}\n")

                if str(tag_name) == "SoulslikeFramework.Stat.Misc.Weight":
                    f.write("\n*** SUCCESS: Tag is now correct! ***\n")
                elif str(tag_name) == "None":
                    f.write("\n*** Tag still None - Blueprint CDO override issue ***\n")
        except Exception as e:
            f.write(f"Verify error: {e}\n")

        f.write("\n" + "="*80 + "\n")

    unreal.log(f"Output saved to: {output_file}")

if __name__ == "__main__":
    fix_b_weight()
