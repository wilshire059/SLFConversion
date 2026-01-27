# reparent_container_to_interactable.py
# Reparent B_Container to B_Interactable (not directly to C++)
# B_Interactable is parented to ASLFInteractableBase, so B_Container will inherit the interface

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("REPARENTING B_Container TO B_Interactable")
    unreal.log_warning("=" * 70)

    # Load B_Interactable Blueprint (which is parented to ASLFInteractableBase)
    interactable_bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"
    interactable_bp = unreal.load_asset(interactable_bp_path)
    if not interactable_bp:
        unreal.log_error(f"Could not load: {interactable_bp_path}")
        return

    # Get the generated class to use as parent
    target_class = interactable_bp.generated_class()
    if not target_class:
        unreal.log_error("Could not get B_Interactable generated class")
        return

    unreal.log_warning(f"Target class: {target_class.get_name()}")

    # Load B_Container Blueprint
    container_bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    container_bp = unreal.load_asset(container_bp_path)
    if not container_bp:
        unreal.log_error(f"Could not load: {container_bp_path}")
        return

    unreal.log_warning(f"Blueprint: {container_bp.get_name()}")

    # Check current parent
    gen_class = container_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log_warning(f"Current CDO Type: {type(cdo).__name__}")

    # Reparent
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(container_bp, target_class)
        unreal.log_warning(f"Reparent result: {result}")
    except Exception as e:
        unreal.log_error(f"Reparent failed: {e}")
        return

    # Compile
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(container_bp)
        unreal.log_warning("Compile completed")
    except Exception as e:
        unreal.log_warning(f"Compile: {e}")

    # Save
    try:
        unreal.EditorAssetLibrary.save_asset(container_bp_path)
        unreal.log_warning("Save completed")
    except Exception as e:
        unreal.log_error(f"Save failed: {e}")

    # Verify
    container_bp_reloaded = unreal.load_asset(container_bp_path)
    if container_bp_reloaded:
        gen_class = container_bp_reloaded.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                unreal.log_warning(f"New CDO Type: {type(cdo).__name__}")

                # Check components
                components = cdo.get_components_by_class(unreal.ActorComponent)
                unreal.log_warning(f"Components: {len(components) if components else 0}")
                if components:
                    for comp in components:
                        unreal.log_warning(f"  - {comp.get_name()} ({type(comp).__name__})")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
