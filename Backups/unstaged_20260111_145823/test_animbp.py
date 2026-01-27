# test_animbp.py - Test that ABP_SoulslikeCharacter_Additive C++ class has overlay variables
import unreal

def test():
    unreal.log_warning("=" * 60)
    unreal.log_warning("Testing ABP_SoulslikeCharacter_Additive C++ Properties")
    unreal.log_warning("=" * 60)

    # Try to load and check the C++ class directly
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive")
    if cpp_class:
        unreal.log_warning("[OK] C++ class loaded: " + str(cpp_class.get_name()))
        cdo = unreal.get_default_object(cpp_class)
        if cdo:
            unreal.log_warning("[OK] Got default object")
            # Check overlay properties exist
            try:
                left_overlay = cdo.get_editor_property('left_hand_overlay_state')
                unreal.log_warning("  [OK] LeftHandOverlayState: " + str(left_overlay))
            except Exception as e:
                unreal.log_error("  [FAIL] LeftHandOverlayState: " + str(e))

            try:
                right_overlay = cdo.get_editor_property('right_hand_overlay_state')
                unreal.log_warning("  [OK] RightHandOverlayState: " + str(right_overlay))
            except Exception as e:
                unreal.log_error("  [FAIL] RightHandOverlayState: " + str(e))

            try:
                active_overlay = cdo.get_editor_property('active_overlay_state')
                unreal.log_warning("  [OK] ActiveOverlayState: " + str(active_overlay))
            except Exception as e:
                unreal.log_error("  [FAIL] ActiveOverlayState: " + str(e))

            # Check EquipmentManager property
            try:
                equip_mgr = cdo.get_editor_property('equipment_manager')
                unreal.log_warning("  [OK] EquipmentManager: " + str(equip_mgr))
            except Exception as e:
                unreal.log_error("  [FAIL] EquipmentManager: " + str(e))
    else:
        unreal.log_error("[FAIL] Could not load C++ class!")

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("Test Complete")
    unreal.log_warning("=" * 60)

test()
