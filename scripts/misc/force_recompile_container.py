# force_recompile_container.py
# Force recompile and save B_Container

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("FORCE RECOMPILING B_Container")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    # Mark it dirty
    try:
        unreal.EditorAssetLibrary.checkout_loaded_asset(bp)
        unreal.log_warning("Checked out")
    except:
        pass

    # Force compile
    try:
        # Try multiple compile approaches
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("Compile 1 completed")
    except Exception as e:
        unreal.log_warning(f"Compile 1: {e}")

    # Request full refresh
    try:
        # This might help refresh the SCS
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        unreal.log_warning("Save 1 completed")
    except Exception as e:
        unreal.log_error(f"Save 1: {e}")

    # Reload and recompile
    bp2 = unreal.load_asset(bp_path)
    if bp2:
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp2)
            unreal.log_warning("Compile 2 completed")
        except Exception as e:
            unreal.log_warning(f"Compile 2: {e}")

        # Check components after
        gen_class = bp2.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                unreal.log_warning(f"CDO Type: {type(cdo).__name__}")

                # Spawn and check
                world = unreal.EditorLevelLibrary.get_editor_world()
                if world:
                    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, -5000))
                    if actor:
                        components = actor.get_components_by_class(unreal.ActorComponent)
                        unreal.log_warning(f"Spawned instance components: {len(components)}")
                        for comp in components:
                            unreal.log_warning(f"  - {comp.get_name()} ({type(comp).__name__})")
                        actor.destroy_actor()

        # Final save
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            unreal.log_warning("Final save completed")
        except Exception as e:
            unreal.log_error(f"Final save: {e}")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
