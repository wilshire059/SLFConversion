import unreal

output_file = "C:/scripts/SLFConversion/tests/scs_detail_output.txt"
results = []

char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
char_bp = unreal.EditorAssetLibrary.load_asset(char_path)

if char_bp:
    results.append(f"Blueprint: {char_bp.get_name()}")
    
    if hasattr(char_bp, 'simple_construction_script'):
        scs = char_bp.simple_construction_script
        if scs:
            all_nodes = scs.get_all_nodes()
            results.append(f"SCS Node Count: {len(all_nodes)}")
            
            for i, node in enumerate(all_nodes):
                comp_template = node.component_template
                if comp_template:
                    comp_class = comp_template.get_class().get_name()
                    comp_name = node.get_variable_name()
                    results.append(f"\n[{i}] {comp_name}: {comp_class}")
                    
                    # For SkeletalMeshComponent, check the mesh
                    if "SkeletalMesh" in comp_class:
                        try:
                            mesh = comp_template.get_skinned_asset() if hasattr(comp_template, 'get_skinned_asset') else None
                            if mesh:
                                results.append(f"     Mesh: {mesh.get_name()}")
                            else:
                                results.append(f"     Mesh: NOT SET")
                        except Exception as e:
                            results.append(f"     Mesh Error: {e}")
                            
                        try:
                            visible = comp_template.get_editor_property('visible')
                            results.append(f"     Visible: {visible}")
                        except:
                            pass
                else:
                    results.append(f"\n[{i}] Node with no template")
        else:
            results.append("SCS is None")
    else:
        results.append("No simple_construction_script attribute")
else:
    results.append(f"ERROR: Could not load {char_path}")

with open(output_file, 'w') as f:
    f.write('\n'.join(results))
