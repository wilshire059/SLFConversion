import unreal

# Extract W_Resources BaseWidth values from bp_only
bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"

bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_error("Could not load " + bp_path)
else:
    # Get the generated class and CDO
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log_warning("=== W_Resources CDO Values ===")

            # Try to get BaseWidth values
            for prop_name in ['base_width_hp', 'base_width_fp', 'base_width_stamina',
                              'BaseWidthHp', 'BaseWidthFp', 'BaseWidthStamina']:
                try:
                    val = cdo.get_editor_property(prop_name)
                    unreal.log_warning(f"  {prop_name}: {val}")
                except:
                    pass

            # Also check scaling factor
            for prop_name in ['scaling_factor', 'ScalingFactor']:
                try:
                    val = cdo.get_editor_property(prop_name)
                    unreal.log_warning(f"  {prop_name}: {val}")
                except:
                    pass

            unreal.log_warning("=== Done ===")
        else:
            unreal.log_error("Could not get CDO")
    else:
        unreal.log_error("Could not get generated class")
