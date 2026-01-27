# check_bp_parent_v3.py
# Check what parent class B_RestingPoint actually has using SLFAutomationLibrary

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING BLUEPRINT PARENT CLASSES")
    unreal.log_warning("=" * 70)

    blueprints = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
    ]

    for bp_path in blueprints:
        bp_name = bp_path.split('/')[-1]
        unreal.log_warning(f"\n--- {bp_name} ---")

        try:
            result = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp_path)
            unreal.log_warning(f"  Parent class: {result}")
        except Exception as e:
            # Fallback - try loading and inspecting
            unreal.log_warning(f"  SLFAutomationLibrary error: {e}")

            bp = unreal.load_asset(bp_path)
            if bp:
                gen_class = bp.generated_class()
                if gen_class:
                    # Use CDO to check the class
                    cdo = unreal.get_default_object(gen_class)
                    if cdo:
                        cdo_class = cdo.get_class()
                        unreal.log_warning(f"  CDO class: {cdo_class.get_name()}")
                        unreal.log_warning(f"  CDO path: {cdo_class.get_path_name()}")

                        # Check outer
                        outer = gen_class.get_outer()
                        if outer:
                            unreal.log_warning(f"  Outer: {outer.get_name()}")

if __name__ == "__main__":
    main()
