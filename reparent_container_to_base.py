# reparent_container_to_base.py
# Reparent B_Container from ASLFContainer to ASLFContainerBase
# ASLFContainerBase inherits from ASLFInteractableBase which implements ISLFInteractableInterface

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("REPARENTING B_Container TO ASLFContainerBase")
    unreal.log_warning("=" * 70)

    # Load the target C++ class
    target_class = unreal.load_class(None, "/Script/SLFConversion.SLFContainerBase")
    if not target_class:
        unreal.log_error("Could not load ASLFContainerBase class!")
        return

    unreal.log_warning(f"Target class: {target_class.get_name()}")

    # Load B_Container Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load Blueprint: {bp_path}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")

    # Check current parent
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log_warning(f"Current CDO Type: {type(cdo).__name__}")

    # Reparent using BlueprintEditorLibrary
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, target_class)
        if result:
            unreal.log_warning("Reparent SUCCEEDED!")
        else:
            unreal.log_error("Reparent returned False")
    except Exception as e:
        unreal.log_error(f"Reparent failed: {e}")
        return

    # Compile the Blueprint
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("Compile completed")
    except Exception as e:
        unreal.log_warning(f"Compile warning: {e}")

    # Save the Blueprint
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path)
        unreal.log_warning("Save completed")
    except Exception as e:
        unreal.log_error(f"Save failed: {e}")

    # Verify new parent
    bp_reloaded = unreal.load_asset(bp_path)
    if bp_reloaded:
        gen_class = bp_reloaded.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                unreal.log_warning(f"New CDO Type: {type(cdo).__name__}")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
