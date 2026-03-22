# fix_container_parent.py
# Reparent B_Container Blueprint to correct C++ class (AB_Container)

import unreal

BP_CONTAINER_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
CORRECT_CPP_CLASS = "/Script/SLFConversion.B_Container"  # AB_Container is registered as B_Container

def fix_parent():
    """Reparent B_Container to AB_Container C++ class"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("FIXING B_CONTAINER PARENT CLASS")
    unreal.log_warning("=" * 60)

    # Load the Blueprint
    bp_asset = unreal.load_asset(BP_CONTAINER_PATH)
    if not bp_asset:
        unreal.log_error(f"Could not load Blueprint: {BP_CONTAINER_PATH}")
        return False

    unreal.log_warning(f"Blueprint: {bp_asset.get_name()}")

    # Get current generated class
    gen_class = bp_asset.generated_class()
    unreal.log_warning(f"Current Generated Class: {gen_class.get_name()}")

    # Load correct C++ parent class
    cpp_class = unreal.load_class(None, CORRECT_CPP_CLASS)
    if not cpp_class:
        unreal.log_error(f"Could not load C++ class: {CORRECT_CPP_CLASS}")
        return False

    unreal.log_warning(f"Target C++ Class: {cpp_class.get_name()}")

    # Reparent the Blueprint
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp_asset, cpp_class)
        if result:
            unreal.log_warning(f"Reparented successfully!")
        else:
            unreal.log_warning(f"Reparent returned False (may already be correct parent)")
    except Exception as e:
        unreal.log_warning(f"Reparent ERROR: {e}")
        return False

    # Compile the Blueprint
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
        unreal.log_warning("Blueprint compiled")
    except Exception as e:
        unreal.log_warning(f"Compile ERROR: {e}")

    # Save the Blueprint
    saved = unreal.EditorAssetLibrary.save_asset(BP_CONTAINER_PATH, only_if_is_dirty=False)
    if saved:
        unreal.log_warning(f"Saved Blueprint")
    else:
        unreal.log_warning(f"Failed to save Blueprint")

    # Verify the change
    bp_asset2 = unreal.load_asset(BP_CONTAINER_PATH)
    gen_class2 = bp_asset2.generated_class()
    unreal.log_warning(f"New Generated Class: {gen_class2.get_name()}")

    # Spawn and check
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class2, unreal.Vector(0, 0, -10000))
        if actor:
            unreal.log_warning(f"Spawned Actor Type: {type(actor).__name__}")

            # Test properties
            test_props = ["OpenMontage", "MoveDistance", "Lid", "PointLight"]
            for prop in test_props:
                try:
                    val = actor.get_editor_property(prop)
                    unreal.log_warning(f"  {prop} = {val}")
                except Exception as e:
                    unreal.log_warning(f"  {prop} FAIL: {str(e)[:50]}")

            actor.destroy_actor()

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning("FIX COMPLETE")
    unreal.log_warning("=" * 60)
    return True

if __name__ == "__main__":
    fix_parent()
