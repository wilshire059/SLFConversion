# reparent_restingpoint_only.py
# Surgically reparent ONLY B_RestingPoint to SLFRestingPointBase

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("REPARENTING B_RestingPoint TO SLFRestingPointBase")
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
    try:
        current_parent = bp.get_editor_property('parent_class')
        if current_parent:
            unreal.log_warning(f"Current parent: {current_parent.get_name()}")
    except Exception as e:
        unreal.log_warning(f"Could not get current parent: {e}")

    # CRITICAL: Clear graphs but keep variables BEFORE reparenting
    # This removes function graphs like GetReplenishData that conflict with C++ signatures
    unreal.log_warning("Clearing graphs (keeping variables)...")
    try:
        unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
        unreal.log_warning("Graphs cleared successfully")
    except Exception as e:
        unreal.log_warning(f"Clear graphs error (continuing): {e}")

    # Remove implemented interfaces
    unreal.log_warning("Removing implemented interfaces...")
    try:
        unreal.SLFAutomationLibrary.remove_implemented_interfaces(bp)
        unreal.log_warning("Interfaces removed successfully")
    except Exception as e:
        unreal.log_warning(f"Remove interfaces error (continuing): {e}")

    # Reparent using BlueprintEditorLibrary
    try:
        success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if success:
            unreal.log_warning("Reparent SUCCESS")

            # Compile the Blueprint
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log_warning("Blueprint compiled")

            # Save the Blueprint
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning("Blueprint saved")

            # Verify
            bp2 = unreal.load_asset(bp_path)
            if bp2:
                try:
                    new_parent = bp2.get_editor_property('parent_class')
                    if new_parent:
                        unreal.log_warning(f"NEW parent: {new_parent.get_name()}")
                except:
                    pass

                # Check if interface is implemented
                gen_class = bp2.generated_class()
                if gen_class:
                    cdo = unreal.get_default_object(gen_class)
                    if cdo:
                        # Check interface implementation
                        interface_class = unreal.load_class(None, "/Script/SLFConversion.SLFInteractableInterface")
                        if interface_class:
                            implements = cdo.get_class().implements_interface(interface_class)
                            unreal.log_warning(f"Implements ISLFInteractableInterface: {implements}")
        else:
            unreal.log_error("Reparent FAILED")
    except Exception as e:
        unreal.log_error(f"Reparent error: {e}")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
