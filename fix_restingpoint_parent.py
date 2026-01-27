# fix_restingpoint_parent.py
# Reparent B_RestingPoint to ASLFRestingPointBase

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("FIXING B_RestingPoint PARENT CLASS")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
    cpp_parent_path = "/Script/SLFConversion.SLFRestingPointBase"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load Blueprint: {bp_path}")
        return

    # Load the C++ parent class
    cpp_class = unreal.load_class(None, cpp_parent_path)
    if not cpp_class:
        unreal.log_error(f"Could not load C++ class: {cpp_parent_path}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")
    unreal.log_warning(f"Target parent: {cpp_class.get_name()}")

    # Get current parent
    gen_class = bp.generated_class()
    if gen_class:
        # Check current parent by checking CDO class path
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log_warning(f"Current CDO class: {cdo.get_class().get_name()}")
            unreal.log_warning(f"Current CDO path: {cdo.get_class().get_path_name()}")

    # Reparent using BlueprintEditorLibrary
    try:
        success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if success:
            unreal.log_warning("Reparent SUCCESS")

            # Save the Blueprint
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning("Blueprint saved")

            # Verify
            bp2 = unreal.load_asset(bp_path)
            if bp2:
                gen_class2 = bp2.generated_class()
                if gen_class2:
                    cdo2 = unreal.get_default_object(gen_class2)
                    if cdo2:
                        unreal.log_warning(f"NEW CDO class: {cdo2.get_class().get_name()}")
                        unreal.log_warning(f"NEW CDO path: {cdo2.get_class().get_path_name()}")
        else:
            unreal.log_error("Reparent FAILED")
    except Exception as e:
        unreal.log_error(f"Reparent error: {e}")

if __name__ == "__main__":
    main()
