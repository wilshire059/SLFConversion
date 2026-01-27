"""
Check B_Ladder ISM component mesh assignments
"""
import unreal

def check_ladder_meshes():
    """Check mesh assignments on B_Ladder ISM components"""
    print("\n" + "="*60)
    print("B_LADDER MESH DIAGNOSTIC")
    print("="*60)

    b_ladder_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Ladder"

    bp = unreal.load_asset(b_ladder_path)
    if not bp:
        print("ERROR: Could not load B_Ladder")
        return

    print(f"Loaded: {b_ladder_path}")

    # Get generated class
    gen_class = bp.generated_class()
    if not gen_class:
        print("ERROR: No generated class")
        return

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        print("ERROR: No CDO")
        return

    print(f"CDO: {cdo.get_name()}")

    # Get parent class
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    print(f"Parent Class: {parent}")

    # Get SCS components
    components = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
    print(f"\nSCS Components ({len(components)}):")
    for comp_name in components:
        print(f"  - {comp_name}")

    # Check if we can access components on CDO
    print("\n--- CDO Component Analysis ---")

    # Try to get components from CDO
    cdo_components = cdo.get_components_by_class(unreal.InstancedStaticMeshComponent)
    print(f"ISM Components on CDO: {len(cdo_components)}")

    for ism in cdo_components:
        name = ism.get_name()
        mesh = ism.get_editor_property('static_mesh')
        mesh_name = mesh.get_name() if mesh else "NULL"
        num_instances = ism.get_instance_count()
        visibility = ism.is_visible()
        print(f"  {name}: Mesh={mesh_name}, Instances={num_instances}, Visible={visibility}")

    print("\n" + "="*60)

if __name__ == "__main__":
    check_ladder_meshes()
