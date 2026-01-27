# check_b_weight.py
# Check B_Weight Blueprint at correct path
import unreal

def check_b_weight():
    """Check B_Weight Blueprint."""

    output_file = "C:/scripts/SLFConversion/migration_cache/b_weight_check.txt"

    with open(output_file, 'w') as f:
        f.write("="*80 + "\n")
        f.write("B_WEIGHT BLUEPRINT CHECK\n")
        f.write("="*80 + "\n\n")

        # Correct path
        weight_bp_path = "/Game/SoulslikeFramework/Data/Stats/Misc/B_Weight"
        weight_bp = unreal.EditorAssetLibrary.load_asset(weight_bp_path)

        if weight_bp:
            f.write(f"B_Weight loaded: YES\n")
            f.write(f"Asset path: {weight_bp_path}\n")
            f.write(f"Asset class: {weight_bp.get_class().get_name()}\n")

            # Get generated class
            try:
                gen_class = weight_bp.generated_class()
                f.write(f"Generated class: {gen_class.get_name()}\n")

                # Check parent class
                parent = gen_class.get_super_class()
                if parent:
                    f.write(f"Parent class: {parent.get_name()}\n")

                    # Check grandparent
                    grandparent = parent.get_super_class()
                    if grandparent:
                        f.write(f"Grandparent class: {grandparent.get_name()}\n")
            except Exception as e:
                f.write(f"Error getting class info: {e}\n")

            # Try to create instance and check StatInfo
            f.write(f"\n--- Instance Check ---\n")
            try:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    f.write(f"CDO name: {cdo.get_name()}\n")
                    f.write(f"CDO class: {cdo.get_class().get_name()}\n")

                    # Check if it's a USLFStatBase
                    f.write(f"Has stat_info property: ")
                    try:
                        stat_info = cdo.get_editor_property("stat_info")
                        f.write("YES\n")
                        if stat_info:
                            try:
                                tag = stat_info.get_editor_property("tag")
                                tag_name = tag.get_editor_property("tag_name") if tag else "NONE"
                                f.write(f"StatInfo.Tag: {tag_name}\n")
                            except:
                                f.write("Could not get tag\n")

                            try:
                                current = stat_info.get_editor_property("current_value")
                                f.write(f"StatInfo.CurrentValue: {current}\n")
                            except:
                                pass

                            try:
                                max_val = stat_info.get_editor_property("max_value")
                                f.write(f"StatInfo.MaxValue: {max_val}\n")
                            except:
                                pass
                    except Exception as e:
                        f.write(f"NO ({e})\n")
            except Exception as e:
                f.write(f"Error checking CDO: {e}\n")
        else:
            f.write(f"B_Weight NOT FOUND at: {weight_bp_path}\n")

        f.write("\n" + "="*80 + "\n")

    unreal.log(f"Output saved to: {output_file}")

if __name__ == "__main__":
    check_b_weight()
