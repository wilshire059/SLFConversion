import unreal

output_file = "C:/scripts/SLFConversion/tests/char_final_check.txt"
results = []

# Check B_Soulslike_Character
char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
char_bp = unreal.EditorAssetLibrary.load_asset(char_path)

if char_bp:
    results.append("=== B_Soulslike_Character ===")
    
    gen_class = char_bp.generated_class()
    if gen_class:
        parent = gen_class.get_super_class()
        results.append(f"Parent: {parent.get_name() if parent else 'None'}")
        
        # If parent is the C++ class, something went wrong
        if parent and 'SLF' in parent.get_name():
            results.append("WARNING: Reparented to C++ class!")
        
    # Check EventGraph
    try:
        uber_pages = char_bp.get_editor_property('ubergraph_pages')
        if uber_pages:
            results.append(f"UberGraph Pages: {len(uber_pages)} (EventGraph should exist)")
        else:
            results.append("NO UberGraph - EventGraph was CLEARED!")
    except Exception as e:
        results.append(f"UberGraph check failed: {e}")
    
    # Check function graphs
    try:
        func_graphs = char_bp.get_editor_property('function_graphs')
        if func_graphs:
            results.append(f"Function Graphs: {len(func_graphs)}")
        else:
            results.append("No function graphs")
    except Exception as e:
        results.append(f"Function graphs check failed: {e}")
        
    # Check CDO mesh
    cdo = unreal.get_default_object(gen_class)
    if cdo and hasattr(cdo, 'mesh'):
        main_mesh = cdo.mesh
        if main_mesh:
            try:
                mesh_asset = main_mesh.get_skinned_asset() if hasattr(main_mesh, 'get_skinned_asset') else None
                results.append(f"CDO SkeletalMesh: {mesh_asset.get_name() if mesh_asset else 'NONE'}")
            except Exception as e:
                results.append(f"CDO mesh check error: {e}")
else:
    results.append(f"FAILED to load B_Soulslike_Character")

# Check B_BaseCharacter 
base_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter"
base_bp = unreal.EditorAssetLibrary.load_asset(base_path)

if base_bp:
    results.append("
=== B_BaseCharacter ===")
    
    gen_class = base_bp.generated_class()
    if gen_class:
        parent = gen_class.get_super_class()
        results.append(f"Parent: {parent.get_name() if parent else 'None'}")
    
    # Check EventGraph
    try:
        uber_pages = base_bp.get_editor_property('ubergraph_pages')
        if uber_pages:
            results.append(f"UberGraph Pages: {len(uber_pages)} (EventGraph should exist)")
        else:
            results.append("NO UberGraph - EventGraph was CLEARED!")
    except Exception as e:
        results.append(f"UberGraph check failed: {e}")
else:
    results.append("FAILED to load B_BaseCharacter")

with open(output_file, 'w') as f:
    f.write('
'.join(results))
print("Done - results written to " + output_file)
