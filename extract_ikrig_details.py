"""
Extract detailed IK Rig node configuration from AnimBP assets.
Run this on both SLFConversion and bp_only to compare.
"""

import unreal
import json

def get_animgraph_ik_details():
    """Extract IK Rig node details from the AnimBP using Python API."""
    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    asset = unreal.load_asset(animbp_path)
    if not asset:
        print("ERROR: Could not load AnimBP")
        return

    print("=" * 80)
    print("IK RIG NODE DETAILS EXTRACTION")
    print("=" * 80)

    print(f"\nAnimBP: {asset.get_name()}")
    print(f"Class: {asset.get_class()}")

    # Get the generated class
    gen_class = asset.generated_class()
    if gen_class:
        print(f"Generated Class: {gen_class}")

        # Get the CDO to examine AnimGraph node properties
        cdo = gen_class.get_default_object()
        if cdo:
            print(f"\nCDO Class: {cdo.get_class()}")

            # List all properties that might relate to IK
            all_props = dir(cdo)
            ik_props = [p for p in all_props if 'ik' in p.lower() or 'alpha' in p.lower() or 'weight' in p.lower()]
            print(f"\nIK-related properties found: {len(ik_props)}")
            for prop in ik_props[:30]:
                try:
                    value = getattr(cdo, prop, None)
                    if value is not None and not callable(value):
                        print(f"  {prop} = {value}")
                except:
                    pass

    # Try to get AnimBlueprint-specific info
    if hasattr(asset, 'get_all_variables'):
        print("\n[Variables]")
        try:
            # This may not be exposed
            pass
        except:
            pass

    # Try to access the AnimGraph
    print("\n[Checking for AnimGraph nodes via reflection...]")
    try:
        # Get the UObject's class
        obj_class = asset.get_class()
        print(f"Object class: {obj_class}")

        # List properties
        props = [p for p in dir(asset) if not p.startswith('_')]
        print(f"Total properties: {len(props)}")

        # Look for relevant ones
        anim_props = [p for p in props if 'anim' in p.lower() or 'graph' in p.lower() or 'node' in p.lower()]
        print(f"Anim-related properties: {anim_props[:20]}")

    except Exception as e:
        print(f"Error accessing properties: {e}")

    print("\n" + "=" * 80)
    print("DONE")
    print("=" * 80)

if __name__ == "__main__":
    get_animgraph_ik_details()
