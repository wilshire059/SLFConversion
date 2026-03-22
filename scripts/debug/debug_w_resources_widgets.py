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

        # Check parent class
        parent = gen_class.get_super_class()
        if parent:
            unreal.log_warning("Parent Class: " + parent.get_name())
        else:
            unreal.log_warning("Parent Class: None")

    # Try to get widget tree info
    try:
        # Get the WidgetTree from the WidgetBlueprint
        # The Python API might not expose this directly, but let's try
        widget_tree = getattr(bp, 'widget_tree', None)
        if widget_tree:
            unreal.log_warning("Has WidgetTree object")
        else:
            unreal.log_warning("No widget_tree attribute")
    except Exception as e:
        unreal.log_warning("Error: " + str(e))

    # Check if we can access any UPROPERTY from C++ parent
    if gen_class:
        cdo = gen_class.get_default_object()
        if cdo:
            # Try properties that should exist if reparented to C++
            try:
                health_percent = cdo.get_editor_property('health_percent_cache')
                unreal.log_warning("HealthPercentCache exists: " + str(health_percent))
            except:
                unreal.log_warning("HealthPercentCache: NOT FOUND")

            try:
                base_width_hp = cdo.get_editor_property('base_width_hp')
                unreal.log_warning("BaseWidthHp: " + str(base_width_hp))
            except:
                unreal.log_warning("BaseWidthHp: NOT FOUND (might be BP variable)")

    unreal.log_warning("=== Done ===")
