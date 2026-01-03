import unreal

output_file = "C:/scripts/SLFConversion/tests/char_mesh_output.txt"
results = []

char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
char_bp = unreal.EditorAssetLibrary.load_asset(char_path)

if char_bp:
    results.append(f"Character Blueprint: {char_bp.get_name()}")
    
    if hasattr(char_bp, 'parent_class') and char_bp.parent_class:
        results.append(f"Parent Class: {char_bp.parent_class.get_path_name()}")
    
    gen_class = char_bp.generated_class()
    if gen_class:
        results.append(f"Generated Class: {gen_class.get_path_name()}")
        
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            results.append(f"\nCDO: {cdo.get_name()}")
            
            # Check main mesh (inherited from ACharacter)
            if hasattr(cdo, 'mesh'):
                main_mesh = cdo.mesh
                if main_mesh:
                    results.append(f"\n-- Main CharacterMesh0 --")
                    results.append(f"  Component Name: {main_mesh.get_name()}")
                    try:
                        visible = main_mesh.get_editor_property('visible')
                        results.append(f"  Visible: {visible}")
                    except:
                        pass
                    try:
                        hidden_game = main_mesh.get_editor_property('hidden_in_game')
                        results.append(f"  HiddenInGame: {hidden_game}")
                    except:
                        pass
                    try:
                        mesh_asset = main_mesh.get_skinned_asset() if hasattr(main_mesh, 'get_skinned_asset') else main_mesh.skeletal_mesh
                        results.append(f"  SkeletalMesh: {mesh_asset.get_name() if mesh_asset else 'NONE'}")
                    except Exception as e:
                        results.append(f"  SkeletalMesh: Error - {e}")
                else:
                    results.append("Main mesh component is None")
            else:
                results.append("CDO has no 'mesh' attribute")
    
    # Check SCS components
    results.append(f"\n-- SCS Components --")
    if hasattr(char_bp, 'simple_construction_script'):
        scs = char_bp.simple_construction_script
        if scs:
            all_nodes = scs.get_all_nodes()
            results.append(f"Total SCS Nodes: {len(all_nodes)}")
            
            for node in all_nodes:
                comp_template = node.component_template
                if comp_template:
                    comp_class = comp_template.get_class().get_name()
                    comp_name = node.get_variable_name()
                    
                    # Check mesh-related components
                    if "Mesh" in comp_class or "Skeletal" in comp_class:
                        results.append(f"\n  {comp_name} ({comp_class}):")
                        try:
                            visible = comp_template.get_editor_property('visible')
                            results.append(f"    Visible: {visible}")
                        except:
                            pass
                        try:
                            hidden_game = comp_template.get_editor_property('hidden_in_game')
                            results.append(f"    HiddenInGame: {hidden_game}")
                        except:
                            pass
                        try:
                            mesh_asset = comp_template.get_skinned_asset() if hasattr(comp_template, 'get_skinned_asset') else None
                            if mesh_asset:
                                results.append(f"    SkeletalMesh: {mesh_asset.get_name()}")
                            else:
                                results.append(f"    SkeletalMesh: NONE (not set)")
                        except Exception as e:
                            results.append(f"    SkeletalMesh Error: {e}")
else:
    results.append(f"ERROR: Could not load {char_path}")

# Write results
with open(output_file, 'w') as f:
    f.write('\n'.join(results))

print("Done")
