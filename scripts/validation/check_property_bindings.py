"""
Check AnimBP property bindings and variable names.
Compares Property Access node bindings between C++ and Blueprint.
"""

import unreal
import json

def check_animbp_bindings():
    """Check AnimBP property bindings."""
    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    unreal.log("=" * 80)
    unreal.log("PROPERTY BINDING CHECK")
    unreal.log("=" * 80)

    asset = unreal.load_asset(animbp_path)
    if not asset:
        unreal.log_error("ERROR: Could not load AnimBP")
        return

    unreal.log(f"AnimBP: {asset.get_name()}")

    # Get the generated class and CDO
    gen_class = asset.generated_class()
    if gen_class:
        unreal.log(f"Generated Class: {gen_class}")

        cdo = gen_class.get_default_object()
        if cdo:
            unreal.log("[CDO Properties - Checking IK related:]")

            # Check key properties
            props_to_check = [
                'b_is_falling', 'bIsFalling', 'bIsAccelerating', 'b_is_accelerating',
                'bIsBlocking', 'b_is_blocking', 'IsResting', 'is_resting',
                'ik_weight', 'IkWeight',
                'speed', 'Speed', 'direction', 'Direction',
                'velocity', 'Velocity',
                'left_hand_overlay_state', 'LeftHandOverlayState',
                'right_hand_overlay_state', 'RightHandOverlayState'
            ]

            for prop_name in props_to_check:
                try:
                    value = cdo.get_editor_property(prop_name)
                    unreal.log(f"  {prop_name} = {value}")
                except Exception as e:
                    unreal.log_warning(f"  {prop_name} = NOT FOUND")

    # Check parent class
    unreal.log("[Parent Class:]")
    if gen_class:
        try:
            # Use cast_to to get the class info
            unreal.log(f"  Class name: {gen_class.get_name()}")
            unreal.log(f"  Class path: {gen_class.get_path_name()}")
        except Exception as e:
            unreal.log_warning(f"  Error getting class info: {e}")

    # Write to file for easy viewing
    output_path = "C:/scripts/SLFConversion/migration_cache/property_check_output.txt"
    with open(output_path, 'w') as f:
        f.write("Property Binding Check Results\n")
        f.write("=" * 80 + "\n")
        f.write(f"AnimBP: {asset.get_name()}\n")

        if gen_class:
            f.write(f"Generated Class: {gen_class}\n\n")
            cdo = gen_class.get_default_object()
            if cdo:
                f.write("CDO Properties:\n")
                props_to_check = [
                    'b_is_falling', 'bIsFalling', 'bIsAccelerating', 'b_is_accelerating',
                    'bIsBlocking', 'b_is_blocking', 'IsResting', 'is_resting',
                    'ik_weight', 'IkWeight',
                    'speed', 'Speed', 'direction', 'Direction',
                    'velocity', 'Velocity',
                    'left_hand_overlay_state', 'LeftHandOverlayState',
                    'right_hand_overlay_state', 'RightHandOverlayState'
                ]
                for prop_name in props_to_check:
                    try:
                        value = cdo.get_editor_property(prop_name)
                        f.write(f"  {prop_name} = {value}\n")
                    except Exception as e:
                        f.write(f"  {prop_name} = NOT FOUND\n")

    unreal.log(f"Results written to: {output_path}")
    unreal.log("=" * 80)

if __name__ == "__main__":
    check_animbp_bindings()
