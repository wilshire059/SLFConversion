"""
Check player character visibility and component settings
"""
import unreal

print("")
print("=" * 70)
print("PLAYER CHARACTER VISIBILITY CHECK")
print("=" * 70)

# Load B_Soulslike_Character
bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if not bp:
    print(f"ERROR: Could not load {bp_path}")
else:
    print(f"Loaded: {bp.get_name()}")
    
    # Check parent class
    if hasattr(bp, 'parent_class') and bp.parent_class:
        print(f"Parent Class: {bp.parent_class.get_path_name()}")
    
    # Check for SCS components
    if hasattr(bp, 'simple_construction_script'):
        scs = bp.simple_construction_script
        if scs:
            all_nodes = scs.get_all_nodes()
            print(f"\nSCS Component Count: {len(all_nodes)}")
            
            mesh_components = []
            camera_components = []
            
            for node in all_nodes:
                comp_template = node.component_template
                if comp_template:
                    comp_class = comp_template.get_class().get_name()
                    comp_name = node.get_variable_name()
                    
                    # Check for mesh-related components
                    if "Mesh" in comp_class or "Mesh" in comp_name:
                        mesh_components.append((comp_name, comp_class, comp_template))
                    
                    # Check for camera components
                    if "Camera" in comp_class or "Camera" in comp_name or "Spring" in comp_class:
                        camera_components.append((comp_name, comp_class, comp_template))
            
            print(f"\n--- MESH COMPONENTS ({len(mesh_components)}) ---")
            for name, cls, template in mesh_components:
                print(f"  {name}: {cls}")
                # Check visibility
                if hasattr(template, 'get_editor_property'):
                    try:
                        visible = template.get_editor_property('visible')
                        print(f"    Visible: {visible}")
                    except:
                        pass
                    try:
                        hidden = template.get_editor_property('hidden_in_game')
                        print(f"    HiddenInGame: {hidden}")
                    except:
                        pass
                    try:
                        mesh = template.get_editor_property('skeletal_mesh')
                        print(f"    SkeletalMesh: {mesh.get_name() if mesh else 'None'}")
                    except:
                        pass
            
            print(f"\n--- CAMERA COMPONENTS ({len(camera_components)}) ---")
            for name, cls, template in camera_components:
                print(f"  {name}: {cls}")

# Check B_BaseCharacter too
print("")
print("=" * 70)
print("BASE CHARACTER CHECK")
print("=" * 70)

base_bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter"
base_bp = unreal.EditorAssetLibrary.load_asset(base_bp_path)

if base_bp:
    print(f"Loaded: {base_bp.get_name()}")
    if hasattr(base_bp, 'parent_class') and base_bp.parent_class:
        print(f"Parent Class: {base_bp.parent_class.get_path_name()}")
    
    # Check CDO (class default object) for mesh settings
    gen_class = base_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            print(f"\nCDO: {cdo.get_name()}")
            
            # Try to get mesh component
            if hasattr(cdo, 'mesh'):
                mesh_comp = cdo.mesh
                if mesh_comp:
                    print(f"Mesh Component: {mesh_comp.get_name()}")
                    try:
                        print(f"  Visible: {mesh_comp.get_editor_property('visible')}")
                    except:
                        pass
                    try:
                        print(f"  SkeletalMesh: {mesh_comp.get_editor_property('skeletal_mesh')}")
                    except:
                        pass

print("")
print("=" * 70)
print("CHECK COMPLETE")
print("=" * 70)
