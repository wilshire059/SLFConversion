import unreal

def reparent_interactables():
    """Re-reparent B_Interactable to force CDO regeneration with new C++ components"""
    
    # B_Interactable needs to be reparented to AB_Interactable
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"
    cpp_class_path = "/Script/SLFConversion.B_Interactable"
    
    unreal.log(f"Loading Blueprint: {bp_path}")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log("ERROR: Could not load Blueprint")
        return
    
    unreal.log(f"Blueprint loaded: {bp.get_name()}")
    
    unreal.log(f"Loading C++ class: {cpp_class_path}")
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        unreal.log("ERROR: Could not load C++ class")
        return
    
    unreal.log(f"C++ class loaded: {cpp_class.get_name()}")
    
    # Reparent
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        unreal.log(f"Reparent result: {result}")
    except Exception as e:
        unreal.log(f"Reparent error: {e}")
    
    # Compile
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log("Compiled B_Interactable")
    except Exception as e:
        unreal.log(f"Compile error: {e}")
    
    # Save
    try:
        result = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        unreal.log(f"Save result: {result}")
    except Exception as e:
        unreal.log(f"Save error: {e}")
    
    # Now do B_RestingPoint
    bp_path2 = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
    cpp_class_path2 = "/Script/SLFConversion.B_RestingPoint"
    
    unreal.log(f"Loading Blueprint: {bp_path2}")
    bp2 = unreal.EditorAssetLibrary.load_asset(bp_path2)
    if bp2:
        cpp_class2 = unreal.load_class(None, cpp_class_path2)
        if cpp_class2:
            try:
                result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp2, cpp_class2)
                unreal.log(f"Reparent result: {result}")
            except Exception as e:
                unreal.log(f"Reparent error: {e}")
            
            try:
                unreal.BlueprintEditorLibrary.compile_blueprint(bp2)
                unreal.log("Compiled B_RestingPoint")
            except Exception as e:
                unreal.log(f"Compile error: {e}")
            
            try:
                result = unreal.EditorAssetLibrary.save_asset(bp_path2, only_if_is_dirty=False)
                unreal.log(f"Save result: {result}")
            except Exception as e:
                unreal.log(f"Save error: {e}")
    
    unreal.log("Done reparenting")

if __name__ == "__main__":
    reparent_interactables()
