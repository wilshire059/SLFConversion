import unreal

def resave_interactables():
    """Force resave B_Interactable and B_RestingPoint to pick up new C++ components"""
    
    # List of blueprints to resave
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
        
        # Mark dirty and save
        unreal.EditorAssetLibrary.checkout_asset(bp_path)
        
        # Get the blueprint and compile
        if hasattr(bp, 'compile_blueprint'):
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log(f"  Compiled")
        
        # Save
        if unreal.EditorAssetLibrary.save_asset(bp_path):
            unreal.log(f"  Saved successfully")
        else:
            unreal.log(f"  WARNING: Save returned false")
    
    unreal.log("Done resaving interactables")

if __name__ == "__main__":
    resave_interactables()
