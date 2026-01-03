import unreal

output_file = "C:/scripts/SLFConversion/tests/pawn_check_output.txt"
results = []

gm_path = "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework"
gm_bp = unreal.EditorAssetLibrary.load_asset(gm_path)

if gm_bp:
    results.append(f"GM Blueprint loaded: {gm_bp.get_name()}")
    
    if hasattr(gm_bp, 'parent_class') and gm_bp.parent_class:
        results.append(f"GM Parent Class: {gm_bp.parent_class.get_path_name()}")
    
    gen_class = gm_bp.generated_class()
    if gen_class:
        results.append(f"Generated Class: {gen_class.get_path_name()}")
        
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            results.append(f"CDO: {cdo.get_name()}")
            
            try:
                pawn_class = cdo.get_editor_property('default_pawn_class')
                if pawn_class:
                    results.append(f"DefaultPawnClass: {pawn_class.get_path_name()}")
                else:
                    results.append("DefaultPawnClass: NOT SET (None)")
            except Exception as e:
                results.append(f"DefaultPawnClass error: {e}")
else:
    results.append(f"ERROR: Could not load {gm_path}")

# Check character Blueprint
char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
char_bp = unreal.EditorAssetLibrary.load_asset(char_path)
if char_bp:
    results.append(f"\nCharacter Blueprint exists: {char_path}")
    if hasattr(char_bp, 'parent_class') and char_bp.parent_class:
        results.append(f"Character Parent Class: {char_bp.parent_class.get_path_name()}")
else:
    results.append(f"\nERROR: Could not load {char_path}")

# Write results
with open(output_file, 'w') as f:
    f.write('\n'.join(results))

print("Output written to:", output_file)
