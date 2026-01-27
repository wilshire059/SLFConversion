import unreal

# Compare W_Resources widget dimensions between projects
# This extracts the actual SizeBox and ProgressBar properties from the Blueprint

bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"

bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_error("ERROR: Could not load " + bp_path)
else:
    unreal.log_warning("=== W_Resources Widget Comparison ===")
    unreal.log_warning("Asset: " + bp.get_name())

    # Get generated class
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning("Parent class: " + str(gen_class.get_super_class().get_name() if gen_class.get_super_class() else "None"))

    # Try to access widget tree directly
    try:
        widget_tree = bp.get_editor_property('widget_tree')
        if widget_tree:
            root = widget_tree.get_editor_property('root_widget')
            if root:
                unreal.log_warning("Root widget: " + root.get_name())

                # Get all widgets
                all_widgets = widget_tree.get_editor_property('all_widgets')
                if all_widgets:
                    unreal.log_warning(f"Total widgets in tree: {len(all_widgets)}")

                    for w in all_widgets:
                        name = w.get_name()
                        classname = w.get_class().get_name()

                        # Only show SizeBox and ProgressBar widgets
                        if 'SizeBox' in classname:
                            unreal.log_warning(f"--- SizeBox: {name} ---")

                            # Get SizeBox specific properties
                            try:
                                wo = w.get_editor_property('width_override')
                                unreal.log_warning(f"  WidthOverride: {wo}")
                            except:
                                unreal.log_warning(f"  WidthOverride: (not accessible)")

                            try:
                                ho = w.get_editor_property('height_override')
                                unreal.log_warning(f"  HeightOverride: {ho}")
                            except:
                                unreal.log_warning(f"  HeightOverride: (not accessible)")

                            try:
                                mdw = w.get_editor_property('min_desired_width')
                                unreal.log_warning(f"  MinDesiredWidth: {mdw}")
                            except:
                                pass

                            try:
                                mdh = w.get_editor_property('min_desired_height')
                                unreal.log_warning(f"  MinDesiredHeight: {mdh}")
                            except:
                                pass

                            # Check slot
                            try:
                                slot = w.get_editor_property('slot')
                                if slot:
                                    slot_class = slot.get_class().get_name()
                                    unreal.log_warning(f"  Slot type: {slot_class}")
                            except:
                                pass

                        elif 'ProgressBar' in classname:
                            unreal.log_warning(f"--- ProgressBar: {name} ---")

                            try:
                                percent = w.get_editor_property('percent')
                                unreal.log_warning(f"  Percent: {percent}")
                            except:
                                pass

                            try:
                                style = w.get_editor_property('widget_style')
                                if style:
                                    unreal.log_warning(f"  HasStyle: True")
                                    # Try to get bar fill brush
                                    try:
                                        fill = style.get_editor_property('fill_image')
                                        if fill:
                                            try:
                                                size = fill.get_editor_property('image_size')
                                                unreal.log_warning(f"  FillImage Size: {size}")
                                            except:
                                                pass
                                    except:
                                        pass
                            except:
                                pass
                else:
                    unreal.log_warning("all_widgets is None")
    except Exception as e:
        unreal.log_error(f"Error accessing widget_tree: {e}")

    # Also try CDO properties
    if gen_class:
        cdo = gen_class.get_default_object()
        if cdo:
            unreal.log_warning("=== CDO Properties ===")
            try:
                hp = cdo.get_editor_property('base_width_hp')
                unreal.log_warning(f"BaseWidthHp: {hp}")
            except:
                unreal.log_warning("BaseWidthHp: (not accessible)")

            try:
                fp = cdo.get_editor_property('base_width_fp')
                unreal.log_warning(f"BaseWidthFp: {fp}")
            except:
                unreal.log_warning("BaseWidthFp: (not accessible)")

            try:
                st = cdo.get_editor_property('base_width_stamina')
                unreal.log_warning(f"BaseWidthStamina: {st}")
            except:
                unreal.log_warning("BaseWidthStamina: (not accessible)")

    unreal.log_warning("=== Done ===")
