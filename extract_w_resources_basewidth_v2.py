import unreal

# Extract BaseWidth values from W_Resources in bp_only
bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"

bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_warning("ERROR: Could not load " + bp_path)
else:
    unreal.log_warning("Loaded: " + bp.get_name())

    gen_class = bp.generated_class()
    if gen_class:
        cdo = gen_class.get_default_object()
        if cdo:
            unreal.log_warning("=== CDO Properties ===")

            # Try to read BaseWidth properties
            try:
                base_width_hp = cdo.get_editor_property('base_width_hp')
                unreal.log_warning("BaseWidthHp: " + str(base_width_hp))
            except Exception as e:
                unreal.log_warning("Error base_width_hp: " + str(e))

            try:
                base_width_fp = cdo.get_editor_property('base_width_fp')
                unreal.log_warning("BaseWidthFp: " + str(base_width_fp))
            except Exception as e:
                unreal.log_warning("Error base_width_fp: " + str(e))

            try:
                base_width_stamina = cdo.get_editor_property('base_width_stamina')
                unreal.log_warning("BaseWidthStamina: " + str(base_width_stamina))
            except Exception as e:
                unreal.log_warning("Error base_width_stamina: " + str(e))

    unreal.log_warning("=== Done ===")
