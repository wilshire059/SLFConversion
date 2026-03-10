"""
Extract mesh assignments from B_Soulslike_NPC_ShowcaseVendor's SCS components.
Run on bp_only project to get original data.
"""
import unreal

def extract_vendor_meshes():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"Could not load: {bp_path}")
        return

    unreal.log_warning(f"\n=== Vendor Blueprint Mesh Data ===")

    # Get the SimpleConstructionScript
    try:
        scs = bp.get_editor_property("simple_construction_script")
        if scs:
            all_nodes = scs.get_all_nodes()
            unreal.log_warning(f"Found {len(all_nodes)} SCS nodes")

            for node in all_nodes:
                if node:
                    comp_template = node.get_editor_property("component_template")
                    if comp_template:
                        comp_name = comp_template.get_name()
                        comp_class = comp_template.get_class().get_name()
                        unreal.log_warning(f"\n  Component: {comp_name} ({comp_class})")

                        # Check if SkeletalMeshComponent
                        if "SkeletalMesh" in comp_class:
                            try:
                                mesh = comp_template.get_editor_property("skeletal_mesh_asset")
                                if mesh:
                                    mesh_path = mesh.get_path_name()
                                    unreal.log_warning(f"    SkeletalMesh: {mesh_path}")
                                else:
                                    unreal.log_warning(f"    SkeletalMesh: NONE")
                            except Exception as e:
                                unreal.log_warning(f"    Error getting mesh: {e}")

                            # Try alternate property name
                            try:
                                mesh = comp_template.get_editor_property("skeletal_mesh")
                                if mesh:
                                    mesh_path = mesh.get_path_name()
                                    unreal.log_warning(f"    SkeletalMesh (alt): {mesh_path}")
                            except:
                                pass
        else:
            unreal.log_warning("No SCS found!")
    except Exception as e:
        unreal.log_warning(f"Error accessing SCS: {e}")

    # Also try spawning to check runtime components
    unreal.log_warning("\n=== Spawning actor to check runtime ===")
    try:
        gen_class = bp.generated_class()
        if gen_class:
            world = unreal.EditorLevelLibrary.get_editor_world()
            if world:
                actor = world.spawn_actor(gen_class, unreal.Vector(0, 0, 0), unreal.Rotator())
                if actor:
                    # Get all skeletal mesh components
                    components = actor.get_components_by_class(unreal.SkeletalMeshComponent)
                    unreal.log_warning(f"Spawned actor has {len(components)} SkeletalMeshComponents:")
                    for comp in components:
                        comp_name = comp.get_name()
                        mesh = comp.get_skeletal_mesh_asset()
                        mesh_name = mesh.get_path_name() if mesh else "NONE"
                        unreal.log_warning(f"  - {comp_name}: {mesh_name}")
                    actor.destroy_actor()
    except Exception as e:
        unreal.log_warning(f"Spawn error: {e}")

if __name__ == "__main__":
    extract_vendor_meshes()
