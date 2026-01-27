# reparent_container.py
# Reparent B_Container to C++ parent class with proper SCS clearing

import unreal

BP_CONTAINER_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
CPP_CLASS_PATH = "/Script/SLFConversion.B_Container"

def reparent_container():
    """Reparent B_Container Blueprint to C++ class"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("REPARENTING B_CONTAINER TO C++")
    unreal.log_warning("=" * 60)

    # Load the Blueprint
    bp_asset = unreal.load_asset(BP_CONTAINER_PATH)
    if not bp_asset:
        unreal.log_error(f"Could not load Blueprint: {BP_CONTAINER_PATH}")
        return False

    unreal.log_warning(f"Blueprint: {bp_asset.get_name()}")

    # Load the C++ parent class
    cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
    if not cpp_class:
        unreal.log_error(f"Could not load C++ class: {CPP_CLASS_PATH}")
        return False

    unreal.log_warning(f"C++ parent class: {cpp_class.get_name()}")

    # Clear the Blueprint's SCS (removes Blueprint component definitions)
    # This prevents property conflicts with C++ components
    try:
        unreal.BlueprintEditorLibrary.remove_all_scs_nodes(bp_asset)
        unreal.log_warning("Cleared SCS nodes from Blueprint")
    except Exception as e:
        unreal.log_warning(f"Note: SCS clearing: {e}")

    # Reparent the Blueprint to C++ class
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp_asset, cpp_class)
        if result:
            unreal.log_warning("Successfully reparented Blueprint")
        else:
            unreal.log_error("Failed to reparent Blueprint")
            return False
    except Exception as e:
        unreal.log_error(f"Reparent error: {e}")
        return False

    # Compile the Blueprint
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
        unreal.log_warning("Compiled Blueprint")
    except Exception as e:
        unreal.log_warning(f"Compile note: {e}")

    # Save the Blueprint
    if unreal.EditorAssetLibrary.save_asset(BP_CONTAINER_PATH):
        unreal.log_warning(f"Saved: {BP_CONTAINER_PATH}")
    else:
        unreal.log_error(f"Failed to save: {BP_CONTAINER_PATH}")
        return False

    unreal.log_warning("=" * 60)
    unreal.log_warning("REPARENTING COMPLETE")
    unreal.log_warning("=" * 60)
    return True

if __name__ == "__main__":
    reparent_container()
