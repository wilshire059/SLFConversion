import unreal

# Debug W_Resources widgets - check if widget tree is intact
bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"

bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_error("ERROR: Could not load " + bp_path)
else:
    unreal.log_warning("=== W_Resources Widget Debug ===")
    unreal.log_warning("Loaded: " + bp.get_name())

    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning("Generated Class: " + gen_class.get_name())

    # Check if we can access C++ parent properties
    if gen_class:
        cdo = gen_class.get_default_object()
        if cdo:
            unreal.log_warning("=== Checking C++ properties ===")

            # Try properties that should exist if reparented to C++
            try:
                health_percent = cdo.get_editor_property('health_percent_cache')
                unreal.log_warning("HealthPercentCache: " + str(health_percent))
            except Exception as e:
                unreal.log_warning("HealthPercentCache NOT FOUND: " + str(e))

            try:
                base_width_hp = cdo.get_editor_property('base_width_hp')
                unreal.log_warning("BaseWidthHp: " + str(base_width_hp))
            except Exception as e:
                unreal.log_warning("BaseWidthHp NOT FOUND: " + str(e))

            try:
                base_width_fp = cdo.get_editor_property('base_width_fp')
                unreal.log_warning("BaseWidthFp: " + str(base_width_fp))
            except Exception as e:
                unreal.log_warning("BaseWidthFp NOT FOUND: " + str(e))

            try:
                base_width_stamina = cdo.get_editor_property('base_width_stamina')
                unreal.log_warning("BaseWidthStamina: " + str(base_width_stamina))
            except Exception as e:
                unreal.log_warning("BaseWidthStamina NOT FOUND: " + str(e))

    unreal.log_warning("=== Done ===")
