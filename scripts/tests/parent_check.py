import unreal

try:
    # Check B_Soulslike_Character
    char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    char_bp = unreal.EditorAssetLibrary.load_asset(char_path)
    
    if char_bp:
        unreal.log_warning(f"=== B_Soulslike_Character ===")
        
        # Check parent
        if hasattr(char_bp, 'parent_class') and char_bp.parent_class:
            parent_name = char_bp.parent_class.get_name()
            unreal.log_warning(f"Parent Class: {parent_name}")
            if 'SLF' in parent_name:
                unreal.log_warning("REPARENTED TO C++!")
            else:
                unreal.log_warning("Original Blueprint parent (B_BaseCharacter)")
        else:
            unreal.log_warning("No parent class found")
        
        # Check if EventGraph/UberGraph exists
        for attr in ['ubergraph_pages', 'function_graphs', 'macro_graphs']:
            if hasattr(char_bp, attr):
                try:
                    val = getattr(char_bp, attr)
                    if val:
                        unreal.log_warning(f"{attr}: {len(val)}")
                    else:
                        unreal.log_warning(f"{attr}: EMPTY")
                except:
                    pass
                    
    # Now check B_BaseCharacter
    base_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter"
    base_bp = unreal.EditorAssetLibrary.load_asset(base_path)
    
    if base_bp:
        unreal.log_warning(f"=== B_BaseCharacter ===")
        if hasattr(base_bp, 'parent_class') and base_bp.parent_class:
            parent_name = base_bp.parent_class.get_name()
            unreal.log_warning(f"Parent Class: {parent_name}")
            if 'SLF' in parent_name:
                unreal.log_warning("REPARENTED TO C++!")
            else:
                unreal.log_warning("Original parent")
        
except Exception as e:
    unreal.log_error(f"Script failed: {e}")
