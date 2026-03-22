"""
Extract vendor NPC mesh configuration from bp_only - v2
Print everything to stdout
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

    print(f"Vendor Blueprint: {vendor_bp.get_name()}")

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

    # Check SCS (SimpleConstructionScript) for component defaults - this is where the meshes are
    print("\n" + "=" * 60)
    print("SIMPLE CONSTRUCTION SCRIPT (SCS) COMPONENTS")
    print("=" * 60)
    try:
        scs = vendor_bp.get_editor_property("simple_construction_script")
        if scs:
            print("SCS found!")
            # Get all nodes
            all_nodes = scs.get_all_nodes()
            print(f"Total SCS nodes: {len(all_nodes)}")

            for node in all_nodes:
                try:
                    comp_template = node.component_template
                    if comp_template:
                        comp_class = comp_template.get_class().get_name()
                        comp_name = comp_template.get_name()
                        print(f"\n  Component: {comp_name}")
                        print(f"    Class: {comp_class}")

                        if "SkeletalMesh" in comp_class:
                            # Try both property names
                            skel_mesh = None
                            try:
                                skel_mesh = comp_template.get_editor_property("skeletal_mesh_asset")
                            except:
                                pass
                            if not skel_mesh:
                                try:
                                    skel_mesh = comp_template.skeletal_mesh
                                except:
                                    pass
                            if not skel_mesh:
                                try:
                                    skel_mesh = comp_template.get_skinned_asset()
                                except:
                                    pass

                            if skel_mesh:
                                mesh_name = skel_mesh.get_name()
                                mesh_path = skel_mesh.get_path_name()
                                print(f"    SKELETAL MESH: {mesh_name}")
                                print(f"    PATH: {mesh_path}")
                                mesh_data[comp_name] = mesh_path
                            else:
                                print(f"    No mesh assigned")
                except Exception as e:
                    print(f"    Error processing node: {e}")
        else:
            print("No SCS found!")
    except Exception as e:
        print(f"Error accessing SCS: {e}")

    # Also check the Guide NPC for comparison
    print("\n" + "=" * 60)
    print("COMPARING WITH GUIDE NPC")
    print("=" * 60)

    guide_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide"
    guide_bp = unreal.load_asset(guide_path)

    if guide_bp:
        print(f"Guide Blueprint: {guide_bp.get_name()}")
        guide_class = guide_bp.generated_class()
        if guide_class:
            guide_cdo = unreal.get_default_object(guide_class)

            # Check main mesh
            try:
                mesh_comp = guide_cdo.get_editor_property("mesh")
                if mesh_comp:
                    skel_mesh = None
                    try:
                        skel_mesh = mesh_comp.get_editor_property("skeletal_mesh_asset")
                    except:
                        pass
                    if not skel_mesh:
                        try:
                            skel_mesh = mesh_comp.skeletal_mesh
                        except:
                            pass
                    if skel_mesh:
                        print(f"  Guide Main Mesh: {skel_mesh.get_name()}")
                        print(f"  Guide Mesh Path: {skel_mesh.get_path_name()}")
            except Exception as e:
                print(f"  Error: {e}")

    # Save results
    print("\n" + "=" * 60)
    print("MESH DATA SUMMARY")
    print("=" * 60)
    print(json.dumps(mesh_data, indent=2))

    output_path = "C:/scripts/SLFConversion/migration_cache/vendor_mesh_config.json"
    with open(output_path, 'w') as f:
        json.dump(mesh_data, f, indent=2)
    print(f"\nSaved to: {output_path}")

    print("\n" + "=" * 60)
    print("EXTRACTION COMPLETE")
    print("=" * 60)

if __name__ == "__main__":
    extract_vendor_mesh()
