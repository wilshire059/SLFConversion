"""
Quick check of W_Settings_Entry parent class.
"""
import unreal

bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry")
if bp:
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"CLASS: {gen_class.get_name()}")

        # Try to get entry_type to check if reparented
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # Check C++ properties
            try:
                et = cdo.get_editor_property('entry_type')
                unreal.log_warning(f"ENTRY_TYPE: {et}")
                unreal.log_warning("REPARENT_STATUS: OK (C++ property accessible)")
            except Exception as e:
                unreal.log_warning(f"ENTRY_TYPE: ERROR - {e}")
                unreal.log_warning("REPARENT_STATUS: FAIL (NOT reparented to C++)")

            # Check view panel properties
            try:
                dv = cdo.get_editor_property('double_button_view')
                unreal.log_warning(f"DOUBLE_BUTTON_VIEW: {dv}")
            except Exception as e:
                unreal.log_warning(f"DOUBLE_BUTTON_VIEW: ERROR - {e}")

            try:
                db = cdo.get_editor_property('decrease_button')
                unreal.log_warning(f"DECREASE_BUTTON: {db}")
            except Exception as e:
                unreal.log_warning(f"DECREASE_BUTTON: ERROR - {e}")
