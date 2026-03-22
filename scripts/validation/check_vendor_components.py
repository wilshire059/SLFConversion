"""
Check Vendor Blueprint components - look for Head, Body, Arms, Legs mesh parts.
"""
import unreal

def check_vendor():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"Could not load: {bp_path}")
        return

    unreal.log_warning(f"\n=== Checking Vendor Components ===")
    unreal.log_warning(f"Blueprint: {bp.get_name()}")

    # Check SCS
    scs = bp.simple_construction_script
    if scs:
        all_nodes = scs.get_all_nodes()
        unreal.log_warning(f"SCS Node Count: {len(all_nodes)}")
        for node in all_nodes:
            if node:
                comp = node.component_template
                if comp:
                    unreal.log_warning(f"  SCS Component: {comp.get_name()} ({comp.get_class().get_name()})")
                    if "SkeletalMesh" in comp.get_class().get_name():
                        try:
                            mesh = comp.get_editor_property("skeletal_mesh")
                            if mesh:
                                unreal.log_warning(f"    Mesh: {mesh.get_name()}")
                            else:
                                unreal.log_warning(f"    Mesh: NONE")
                        except:
                            pass
    else:
        unreal.log_warning("No SCS found!")

    # Try spawning to check runtime components
    unreal.log_warning("\n=== Spawning to check runtime ===")
    try:
        gen_class = bp.generated_class()
        if gen_class:
            # Spawn in temp world
            world = unreal.EditorLevelLibrary.get_editor_world()
            if world:
                actor = world.spawn_actor(gen_class, unreal.Vector(0, 0, 0), unreal.Rotator())
                if actor:
                    components = actor.get_components_by_class(unreal.SkeletalMeshComponent)
                    unreal.log_warning(f"Spawned actor has {len(components)} SkeletalMeshComponents:")
                    for comp in components:
                        mesh = comp.get_skeletal_mesh_asset() if hasattr(comp, 'get_skeletal_mesh_asset') else None
                        unreal.log_warning(f"  - {comp.get_name()}: mesh={mesh.get_name() if mesh else 'NONE'}")
                    actor.destroy_actor()
    except Exception as e:
        unreal.log_warning(f"Spawn error: {e}")

if __name__ == "__main__":
    check_vendor()
