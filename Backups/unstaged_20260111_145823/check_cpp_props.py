"""Check C++ AC_EquipmentManager properties."""
import unreal

# Load the C++ class directly
cpp_class = unreal.load_class(None, "/Script/SLFConversion.AC_EquipmentManager")
if cpp_class:
    unreal.log_warning(f"C++ class: {cpp_class.get_name()}")
    cdo = cpp_class.get_default_object()
    if cdo:
        unreal.log_warning(f"CDO class: {cdo.get_class().get_name()}")

        # Try various property name formats
        prop_names = [
            'LeftHandOverlayState',
            'left_hand_overlay_state',
            'leftHandOverlayState',
        ]
        for name in prop_names:
            try:
                val = cdo.get_editor_property(name)
                unreal.log_warning(f"  {name}: SUCCESS = {val}")
            except Exception as e:
                unreal.log_warning(f"  {name}: FAILED")

        # List all properties by checking what's available
        unreal.log_warning("=== Checking common property access ===")
        try:
            # Try a known property
            val = cdo.get_editor_property('component_tags')
            unreal.log_warning(f"  component_tags: SUCCESS")
        except:
            unreal.log_warning(f"  component_tags: FAILED")
else:
    unreal.log_warning("C++ class not found")

# Now check the Blueprint version
unreal.log_warning("=== Blueprint Check ===")
bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if bp:
    gen_class = bp.generated_class()
    if gen_class:
        cdo = gen_class.get_default_object()
        if cdo:
            unreal.log_warning(f"BP CDO class: {cdo.get_class().get_name()}")

            # Try to access via cast
            try:
                # Is it an AC_EquipmentManager?
                ac_em = unreal.AC_EquipmentManager.cast(cdo)
                if ac_em:
                    unreal.log_warning("Cast to AC_EquipmentManager: SUCCESS")
                    val = ac_em.get_editor_property('LeftHandOverlayState')
                    unreal.log_warning(f"  LeftHandOverlayState: {val}")
                else:
                    unreal.log_warning("Cast to AC_EquipmentManager: FAILED (returned None)")
            except Exception as e:
                unreal.log_warning(f"Cast failed: {str(e)[:80]}")
