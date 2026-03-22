import unreal

output_file = "C:/scripts/SLFConversion/tests/char_check.txt"
results = []

try:
    char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    char_bp = unreal.EditorAssetLibrary.load_asset(char_path)
    
    if char_bp:
        results.append(f"Loaded: {char_bp.get_name()}")
        
        # Check parent class
        gen_class = char_bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            if parent:
                results.append(f"Parent: {parent.get_name()}")
                
        # Check if has ubergraph (EventGraph nodes)
        if hasattr(char_bp, 'ubergraph_pages'):
            pages = char_bp.ubergraph_pages
            if pages:
                results.append(f"Has UberGraph pages: {len(pages)}")
            else:
                results.append("NO UberGraph pages (CLEARED!)")
        
        # Check function graphs
        if hasattr(char_bp, 'function_graphs'):
            fg = char_bp.function_graphs
            if fg:
                results.append(f"Has function graphs: {len(fg)}")
            else:
                results.append("NO function graphs")
    else:
        results.append("FAILED to load character BP")
except Exception as e:
    results.append(f"Error: {e}")

with open(output_file, 'w') as f:
    f.write('
'.join(results))
print("Done")
