import unreal

output_file = "C:/scripts/SLFConversion/tests/pawn_check_output.txt"
results = []

# Check if B_Soulslike_Character has EventGraph nodes
char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
char_bp = unreal.EditorAssetLibrary.load_asset(char_path)

if char_bp:
    results.append(f"Character BP: {char_bp.get_name()}")
    
    # Check parent class
    gen_class = char_bp.generated_class()
    if gen_class:
        parent = gen_class.get_super_class()
        results.append(f"Parent: {parent.get_path_name() if parent else 'None'}")
    
    # Try to access EventGraph to see if it has nodes
    if hasattr(char_bp, 'ubergraph_pages'):
        pages = char_bp.ubergraph_pages
        if pages:
            results.append(f"UberGraph Pages: {len(pages)}")
            for i, page in enumerate(pages):
                graph_name = page.get_name() if hasattr(page, 'get_name') else str(page)
                results.append(f"  [{i}] {graph_name}")
    else:
        results.append("No ubergraph_pages attribute")
        
    # Check function graphs
    if hasattr(char_bp, 'function_graphs'):
        fg = char_bp.function_graphs
        results.append(f"Function Graphs: {len(fg) if fg else 0}")
        if fg:
            for i, f in enumerate(fg[:5]):
                results.append(f"  [{i}] {f.get_name() if hasattr(f, 'get_name') else str(f)}")
    
    # Check CDO mesh
    cdo = unreal.get_default_object(gen_class)
    if cdo and hasattr(cdo, 'mesh'):
        main_mesh = cdo.mesh
        if main_mesh:
            try:
                mesh_asset = main_mesh.get_skinned_asset() if hasattr(main_mesh, 'get_skinned_asset') else None
                results.append(f"Main Mesh Asset: {mesh_asset.get_name() if mesh_asset else 'NONE'}")
            except:
                results.append("Main Mesh Asset: Error getting")
else:
    results.append(f"ERROR: Could not load {char_path}")

with open(output_file, 'w') as f:
    f.write('
'.join(results))

print('Done')
