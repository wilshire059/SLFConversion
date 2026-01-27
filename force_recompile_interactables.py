import unreal

def force_recompile():
    """Force recompile and resave interactable Blueprints to pick up new C++ components"""
    
    bp_paths = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
    ]
    
    for bp_path in bp_paths:
        unreal.log(f"Processing: {bp_path}")
        
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log(f"  ERROR: Could not load")
            continue
        
        # Force compile the Blueprint
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
    
    unreal.log("Done")

if __name__ == "__main__":
    force_recompile()
