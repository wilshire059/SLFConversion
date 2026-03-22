"""
List all CDO properties available on the AnimBP instance
"""

import unreal

def list_cdo_properties():
    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    asset = unreal.load_asset(animbp_path)
    if not asset:
        unreal.log_error("Could not load AnimBP")
        return

    gen_class = asset.generated_class()
    if not gen_class:
        unreal.log_error("Could not get generated class")
        return

    unreal.log(f"Asset: {asset.get_name()}")
    unreal.log(f"Generated Class: {gen_class}")
    unreal.log(f"Generated Class Name: {gen_class.get_name()}")

    # The CDO is the default object OF the generated class
    # We need to get it differently - it's a UAnimInstance subclass
    cdo = unreal.get_default_object(gen_class)
    if cdo:
        unreal.log(f"CDO: {cdo}")
        unreal.log(f"CDO Type: {type(cdo)}")
        unreal.log(f"CDO Class: {cdo.get_class()}")

        # List all properties
        all_props = [p for p in dir(cdo) if not p.startswith('_')]
        unreal.log(f"Total properties: {len(all_props)}")

        # Check for specific patterns
        ik_related = [p for p in all_props if 'ik' in p.lower() or 'weight' in p.lower()]
        falling_related = [p for p in all_props if 'fall' in p.lower() or 'falling' in p.lower()]
        overlay_related = [p for p in all_props if 'overlay' in p.lower()]
        anim_related = [p for p in all_props if 'anim' in p.lower()]

        unreal.log(f"\n[IK Related ({len(ik_related)})]: {ik_related[:10]}")
        unreal.log(f"\n[Falling Related ({len(falling_related)})]: {falling_related[:10]}")
        unreal.log(f"\n[Overlay Related ({len(overlay_related)})]: {overlay_related[:10]}")
        unreal.log(f"\n[Anim Related ({len(anim_related)})]: {anim_related[:10]}")

        # Write all to file
        with open("C:/scripts/SLFConversion/migration_cache/cdo_all_properties.txt", 'w') as f:
            f.write(f"CDO: {cdo}\n")
            f.write(f"CDO Type: {type(cdo)}\n")
            f.write(f"CDO Class: {cdo.get_class()}\n\n")

            f.write(f"IK Related ({len(ik_related)}):\n")
            for p in ik_related:
                f.write(f"  {p}\n")

            f.write(f"\nFalling Related ({len(falling_related)}):\n")
            for p in falling_related:
                f.write(f"  {p}\n")

            f.write(f"\nOverlay Related ({len(overlay_related)}):\n")
            for p in overlay_related:
                f.write(f"  {p}\n")

            f.write(f"\nAll Properties ({len(all_props)}):\n")
            for p in sorted(all_props):
                f.write(f"  {p}\n")

        unreal.log("Full property list written to migration_cache/cdo_all_properties.txt")

    else:
        unreal.log_error("Could not get CDO")

if __name__ == "__main__":
    list_cdo_properties()
