"""Check if AC_EquipmentManager was properly reparented."""
import unreal

# Load the Blueprint
bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"Blueprint: AC_EquipmentManager_C")

        # Try to cast CDO to C++ class
        cdo = gen_class.get_default_object()
        if cdo:
            # Get the class path
            class_path = str(type(cdo))
            unreal.log_warning(f"CDO type: {class_path}")

            # Check if it's an ActorComponent
            if isinstance(cdo, unreal.ActorComponent):
                unreal.log_warning("CDO is ActorComponent: YES")
            else:
                unreal.log_warning("CDO is ActorComponent: NO")

            # Try to access C++ properties directly
            try:
                # These are C++ snake_case versions
                val = cdo.left_hand_overlay_state
                unreal.log_warning(f"left_hand_overlay_state (attr): {val}")
            except AttributeError:
                unreal.log_warning("left_hand_overlay_state: NOT FOUND as attribute")

# Check what the C++ class looks like
unreal.log_warning("=== C++ Class Check ===")
cpp_class = unreal.load_class(None, "/Script/SLFConversion.AC_EquipmentManager")
if cpp_class:
    unreal.log_warning(f"C++ class loaded: {cpp_class.get_name()}")
    cpp_cdo = cpp_class.get_default_object()
    if cpp_cdo:
        try:
            val = cpp_cdo.get_editor_property('left_hand_overlay_state')
            unreal.log_warning(f"C++ CDO left_hand_overlay_state: {val}")
        except Exception as e:
            unreal.log_warning(f"C++ CDO property access failed: {str(e)[:50]}")
else:
    unreal.log_warning("C++ class NOT FOUND - module not loaded?")
