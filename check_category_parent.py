"""
Check parent class of W_Settings_CategoryEntry
"""
import unreal

bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"
bp = unreal.load_asset(bp_path)
if bp:
    gen_class = bp.generated_class()
    if gen_class:
        # Use static_class or native parent
        parent_class_name = str(gen_class)
        unreal.log_warning(f"W_Settings_CategoryEntry generated class: {parent_class_name}")

        # Check if it's a subclass of our C++ class
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.W_Settings_CategoryEntry")
        if cpp_class:
            is_child = gen_class.is_child_of(cpp_class)
            unreal.log_warning(f"Is child of C++ UW_Settings_CategoryEntry: {is_child}")
        else:
            unreal.log_warning("Could not load C++ W_Settings_CategoryEntry class")

        # Also check UserWidget
        userwidget_class = unreal.load_class(None, "/Script/UMG.UserWidget")
        if userwidget_class:
            is_child_uw = gen_class.is_child_of(userwidget_class)
            unreal.log_warning(f"Is child of UserWidget: {is_child_uw}")
else:
    unreal.log_error("Could not load W_Settings_CategoryEntry")
