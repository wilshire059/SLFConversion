"""
Reparent B_Destructible to the correct C++ parent class AB_Destructible.
"""

import unreal

def reparent_destructible():
    """Reparent B_Destructible to C++ AB_Destructible class."""

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible"
    cpp_class_path = "/Script/SLFConversion.B_Destructible"

    print("\n" + "="*80)
    print("Reparenting B_Destructible to C++ class")
    print("="*80)

    # Load the C++ class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        print(f"ERROR: Could not load C++ class: {cpp_class_path}")
        return

    print(f"C++ class loaded: {cpp_class.get_name()}")

    # Load the Blueprint
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        print(f"ERROR: Could not load Blueprint: {bp_path}")
        return

    print(f"Blueprint loaded: {bp_asset.get_name()}")

    # Reparent the Blueprint
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp_asset, cpp_class)
        if result:
            print(f"SUCCESS: Reparented Blueprint to {cpp_class.get_name()}")
        else:
            print(f"FAILED: Could not reparent Blueprint")
            return
    except Exception as e:
        print(f"ERROR during reparent: {e}")
        return

    # Compile the Blueprint
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
        print("Blueprint compiled")
    except Exception as e:
        print(f"Compile error: {e}")

    # Save the Blueprint
    try:
        result = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        print(f"Save result: {result}")
    except Exception as e:
        print(f"Save error: {e}")

    # Verify the reparent by spawning
    print("\n--- Verification ---")
    gen_class = bp_asset.generated_class()
    if gen_class:
        spawn_location = unreal.Vector(0.0, 0.0, 2000.0)
        spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_location)
        if spawned:
            print(f"Spawned: {spawned.get_name()}")

            # Check for GeometryCollection property (from C++ parent)
            try:
                gc = spawned.get_editor_property('geometry_collection')
                print(f"GeometryCollection: {gc.get_path_name() if gc else 'None'}")
            except Exception as e:
                print(f"GeometryCollection error: {e}")

            # Check for GeometryCollectionComponent
            gc_comps = spawned.get_components_by_class(unreal.GeometryCollectionComponent)
            print(f"GeometryCollectionComponents: {len(gc_comps)}")

            # Cleanup
            unreal.EditorLevelLibrary.destroy_actor(spawned)

    print("\n" + "="*80)
    print("Reparent Complete")
    print("="*80 + "\n")

if __name__ == "__main__":
    reparent_destructible()
