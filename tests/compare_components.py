import unreal

results = []

def check_blueprint(bp_path, bp_name):
    results.append("")
    results.append("=== " + bp_name + " (Current State) ===")
    
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        results.append("FAILED to load " + bp_name)
        return
    
    # Check parent
    if hasattr(bp, 'parent_class') and bp.parent_class:
        results.append("Parent: " + bp.parent_class.get_name())
    else:
        results.append("Parent: UNKNOWN")
    
    gen_class = bp.generated_class()
    if not gen_class:
        results.append("No generated class!")
        return
    
    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        results.append("No CDO!")
        return
    
    # Check SCS components
    results.append("")
    results.append("SCS Components:")
    if hasattr(bp, 'simple_construction_script'):
        scs = bp.simple_construction_script
        if scs:
            try:
                nodes = scs.get_all_nodes()
                if nodes:
                    for node in nodes:
                        if hasattr(node, 'component_template') and node.component_template:
                            comp = node.component_template
                            comp_class = comp.get_class().get_name()
                            comp_name = str(node.get_variable_name()) if hasattr(node, 'get_variable_name') else comp.get_name()
                            results.append("  - " + comp_name + " (" + comp_class + ")")
                else:
                    results.append("  NO SCS NODES!")
            except Exception as e:
                results.append("  Error getting nodes: " + str(e))
        else:
            results.append("  SCS is None")
    else:
        results.append("  No simple_construction_script attribute")
    
    # Check for key mesh components on CDO
    results.append("")
    results.append("Key Components on CDO:")
    mesh_comps = ["head", "upperbody", "lowerbody", "arms", "cameraboom", "followcamera"]
    for comp_name in mesh_comps:
        if hasattr(cdo, comp_name):
            comp = getattr(cdo, comp_name)
            if comp:
                results.append("  " + comp_name + ": EXISTS (" + comp.get_class().get_name() + ")")
            else:
                results.append("  " + comp_name + ": None")
        else:
            results.append("  " + comp_name + ": NOT FOUND on CDO")
    
    # Check main mesh
    if hasattr(cdo, 'mesh'):
        mesh = cdo.mesh
        if mesh:
            results.append("")
            results.append("CharacterMesh0: " + mesh.get_class().get_name())
            try:
                skel_mesh = mesh.get_skinned_asset() if hasattr(mesh, 'get_skinned_asset') else None
                if skel_mesh:
                    results.append("  SkeletalMesh: " + skel_mesh.get_name())
                else:
                    results.append("  SkeletalMesh: NONE")
                visible = mesh.get_editor_property("visible")
                results.append("  Visible: " + str(visible))
            except Exception as e:
                results.append("  Error: " + str(e))

# Check both blueprints
check_blueprint("/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character", "B_Soulslike_Character")
check_blueprint("/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter", "B_BaseCharacter")

# Output results
for line in results:
    unreal.log_warning(line)
