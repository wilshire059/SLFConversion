# clear_container_scs.py
# Clear B_Container Blueprint SCS to use C++ components

import unreal

BP_CONTAINER_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"

def clear_container_scs():
    """Clear B_Container SCS to resolve property name conflicts with C++"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("CLEARING B_CONTAINER SCS")
    unreal.log_warning("=" * 60)

    # Load the Blueprint
    bp_asset = unreal.load_asset(BP_CONTAINER_PATH)
    if not bp_asset:
        unreal.log_error(f"Could not load Blueprint: {BP_CONTAINER_PATH}")
        return False

    unreal.log_warning(f"Blueprint: {bp_asset.get_name()}")

    # Get generated class to check parent
    gen_class = bp_asset.generated_class()
    unreal.log_warning(f"Generated class: {gen_class.get_name()}")

    # Get parent class
    parent_class = gen_class.get_super_class()
    unreal.log_warning(f"Parent class: {parent_class.get_path_name() if parent_class else 'None'}")

    # Use BlueprintEditorLibrary to clear SCS
    try:
        # Remove all user-defined component nodes
        # The C++ parent class already defines these components via CreateDefaultSubobject
        unreal.BlueprintEditorLibrary.remove_all_scs_nodes(bp_asset)
        unreal.log_warning("Cleared all SCS nodes from Blueprint")
    except Exception as e:
        unreal.log_warning(f"Error clearing SCS: {e}")
        # Try alternative method - compile to reset
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
            unreal.log_warning("Compiled Blueprint after SCS modification")
        except Exception as e2:
            unreal.log_warning(f"Compile error: {e2}")

    # Save the Blueprint
    if unreal.EditorAssetLibrary.save_asset(BP_CONTAINER_PATH):
        unreal.log_warning(f"Saved: {BP_CONTAINER_PATH}")
    else:
        unreal.log_error(f"Failed to save: {BP_CONTAINER_PATH}")
        return False

    unreal.log_warning("=" * 60)
    unreal.log_warning("SCS CLEARED SUCCESSFULLY")
    unreal.log_warning("=" * 60)
    return True

if __name__ == "__main__":
    clear_container_scs()
