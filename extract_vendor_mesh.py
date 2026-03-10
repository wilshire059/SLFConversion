"""
Extract vendor NPC mesh configuration from bp_only
"""
import unreal
import json

def extract_vendor_mesh():
    print("=" * 60)
    print("EXTRACTING VENDOR NPC MESH CONFIGURATION")
    print("=" * 60)

    # Load the vendor Blueprint from bp_only
    vendor_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor"
    vendor_bp = unreal.load_asset(vendor_path)

    if not vendor_bp:
        print(f"ERROR: Could not load vendor Blueprint at {vendor_path}")
        return

    print(f"\nVendor Blueprint: {vendor_bp.get_name()}")

    # Get generated class
    gen_class = vendor_bp.generated_class()
    print(f"Generated Class: {gen_class.get_name() if gen_class else 'NULL'}")

    if not gen_class:
        print("ERROR: No generated class!")
        return

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    print(f"CDO: {cdo.get_name()}")

    mesh_data = {}

    # Check main character mesh
    print("\n=== MAIN CHARACTER MESH ===")
    try:
        mesh_comp = cdo.get_editor_property("mesh")
        if mesh_comp:
            print(f"Mesh Component: {mesh_comp.get_name()}")
            skel_mesh = mesh_comp.get_editor_property("skeletal_mesh_asset")
            if skel_mesh:
                print(f"  Skeletal Mesh: {skel_mesh.get_name()}")
                print(f"  Mesh Path: {skel_mesh.get_path_name()}")
                mesh_data["MainMesh"] = skel_mesh.get_path_name()
            else:
                skel_mesh = mesh_comp.get_editor_property("skeletal_mesh")
                if skel_mesh:
                    print(f"  Skeletal Mesh: {skel_mesh.get_name()}")
                    print(f"  Mesh Path: {skel_mesh.get_path_name()}")
                    mesh_data["MainMesh"] = skel_mesh.get_path_name()
                else:
                    print("  WARNING: No skeletal mesh set!")
    except Exception as e:
        print(f"Error accessing mesh: {e}")

    # Check the child components (Head, Body, Arms, Legs)
    print("\n=== BODY PART COMPONENTS ===")
    component_names = ["Head", "Body", "Arms", "Legs"]

    # Get all components from the actor
    try:
        # Try to iterate through all SkeletalMeshComponents
        all_components = cdo.get_components_by_class(unreal.SkeletalMeshComponent)
        print(f"Found {len(all_components)} SkeletalMeshComponents")

        for comp in all_components:
            comp_name = comp.get_name()
            print(f"\nComponent: {comp_name}")
            try:
                skel_mesh = comp.get_editor_property("skeletal_mesh_asset")
                if not skel_mesh:
                    skel_mesh = comp.get_editor_property("skeletal_mesh")
                if skel_mesh:
                    print(f"  Skeletal Mesh: {skel_mesh.get_name()}")
                    print(f"  Mesh Path: {skel_mesh.get_path_name()}")
                    mesh_data[comp_name] = skel_mesh.get_path_name()
                else:
                    print("  No mesh assigned")
            except Exception as e:
                print(f"  Error: {e}")
    except Exception as e:
        print(f"Error getting components: {e}")

    # Also check SCS (SimpleConstructionScript) for component defaults
    print("\n=== SIMPLE CONSTRUCTION SCRIPT ===")
    try:
        scs = vendor_bp.get_editor_property("simple_construction_script")
        if scs:
            print("SCS found")
            # Get all nodes
            all_nodes = scs.get_all_nodes()
            print(f"Found {len(all_nodes)} SCS nodes")

            for node in all_nodes:
                try:
                    comp_template = node.component_template
                    if comp_template:
                        comp_class = comp_template.get_class().get_name()
                        comp_name = comp_template.get_name()
                        print(f"\nSCS Node: {comp_name} ({comp_class})")

                        if "SkeletalMesh" in comp_class:
                            try:
                                skel_mesh = comp_template.get_editor_property("skeletal_mesh_asset")
                                if not skel_mesh:
                                    skel_mesh = comp_template.get_editor_property("skeletal_mesh")
                                if skel_mesh:
                                    print(f"  Skeletal Mesh: {skel_mesh.get_name()}")
                                    print(f"  Mesh Path: {skel_mesh.get_path_name()}")
                                    mesh_data[f"SCS_{comp_name}"] = skel_mesh.get_path_name()
                                else:
                                    print("  No mesh assigned in SCS")
                            except Exception as e:
                                print(f"  Error getting mesh: {e}")
                except Exception as e:
                    print(f"  Error processing node: {e}")
    except Exception as e:
        print(f"Error accessing SCS: {e}")

    # Save to JSON
    output_path = "C:/scripts/SLFConversion/migration_cache/vendor_mesh_config.json"
    with open(output_path, 'w') as f:
        json.dump(mesh_data, f, indent=2)
    print(f"\n\nSaved mesh config to: {output_path}")
    print(json.dumps(mesh_data, indent=2))

    print("\n" + "=" * 60)
    print("EXTRACTION COMPLETE")
    print("=" * 60)

if __name__ == "__main__":
    extract_vendor_mesh()
