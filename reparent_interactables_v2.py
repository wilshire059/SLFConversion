import unreal

def reparent_interactables():
    """Reparent B_Interactable and B_RestingPoint to their C++ parent classes"""

    reparent_list = [
        {
            "bp_path": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
            "cpp_class": "/Script/SLFConversion.B_Interactable"
        },
        {
            "bp_path": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
            "cpp_class": "/Script/SLFConversion.B_RestingPoint"
        },
    ]

    for item in reparent_list:
        bp_path = item["bp_path"]
        cpp_class_path = item["cpp_class"]

        unreal.log(f"Processing: {bp_path}")

        # Load Blueprint
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log(f"  ERROR: Could not load Blueprint")
            continue

        # Load C++ class
        cpp_class = unreal.load_class(None, cpp_class_path)
        if not cpp_class:
            unreal.log(f"  ERROR: Could not load C++ class: {cpp_class_path}")
            continue

        unreal.log(f"  Blueprint: {bp.get_name()}, C++ Class: {cpp_class.get_name()}")

        # Reparent
        try:
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            unreal.log(f"  Reparent result: {result}")
        except Exception as e:
            unreal.log(f"  Reparent error: {e}")
            continue

        # Compile
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log(f"  Compiled")
        except Exception as e:
            unreal.log(f"  Compile error: {e}")

        # Save
        try:
            result = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            unreal.log(f"  Save result: {result}")
        except Exception as e:
            unreal.log(f"  Save error: {e}")

    unreal.log("Done reparenting interactables")

if __name__ == "__main__":
    reparent_interactables()
