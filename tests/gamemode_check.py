"""
Check GameMode DefaultPawnClass setting
"""
import unreal

print("")
print("=" * 70)
print("GAMEMODE CHECK")
print("=" * 70)

# Load GM_SoulslikeFramework
gm_path = "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework"
gm_bp = unreal.EditorAssetLibrary.load_asset(gm_path)

if not gm_bp:
    unreal.log_error(f"Could not load {gm_path}")
else:
    unreal.log(f"Loaded: {gm_bp.get_name()}")
    
    if hasattr(gm_bp, 'parent_class') and gm_bp.parent_class:
        unreal.log(f"Parent Class: {gm_bp.parent_class.get_path_name()}")
    
    # Get generated class and CDO
    gen_class = gm_bp.generated_class()
    if gen_class:
        unreal.log(f"Generated Class: {gen_class.get_path_name()}")
        
        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log(f"CDO: {cdo.get_name()}")
            
            # Check default pawn class
            if hasattr(cdo, 'default_pawn_class'):
                pawn_class = cdo.default_pawn_class
                if pawn_class:
                    unreal.log(f"DefaultPawnClass: {pawn_class.get_path_name()}")
                else:
                    unreal.log_warning("DefaultPawnClass: NOT SET (None)")
            else:
                unreal.log_warning("CDO has no default_pawn_class attribute")
                
            # List all available properties
            unreal.log("CDO properties containing 'pawn' or 'character':")
            for prop in dir(cdo):
                if 'pawn' in prop.lower() or 'character' in prop.lower():
                    try:
                        val = getattr(cdo, prop)
                        unreal.log(f"  {prop}: {val}")
                    except:
                        pass

# Also check B_Soulslike_Character path
char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
char_bp = unreal.EditorAssetLibrary.load_asset(char_path)
if char_bp:
    unreal.log(f"\nB_Soulslike_Character exists at: {char_path}")
    if hasattr(char_bp, 'parent_class') and char_bp.parent_class:
        unreal.log(f"  Parent Class: {char_bp.parent_class.get_path_name()}")
else:
    unreal.log_warning(f"Could not load B_Soulslike_Character at {char_path}")

print("=" * 70)
print("CHECK COMPLETE")
print("=" * 70)
