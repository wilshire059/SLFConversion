"""
Deep comparison of B_Door vs B_BossDoor to understand why B_BossDoor inherits C++ components but B_Door doesn't
"""
import unreal

def analyze_blueprint(bp_path, name):
    unreal.log(f"\n{'='*60}")
    unreal.log(f"ANALYZING: {name}")
    unreal.log(f"{'='*60}")

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load {bp_path}")
        return

    unreal.log(f"Blueprint: {bp.get_name()}")
    unreal.log(f"Blueprint class: {bp.get_class().get_name()}")

    # Check compile status
    unreal.log(f"\nCompile Status:")
    try:
        status = bp.get_editor_property('status')
        unreal.log(f"  Status: {status}")
    except Exception as e:
        unreal.log(f"  Could not get status: {e}")

    # Check UbergraphPages (EventGraph)
    try:
        ubergraph_pages = bp.get_editor_property('ubergraph_pages')
        unreal.log(f"  UbergraphPages: {len(ubergraph_pages) if ubergraph_pages else 0}")
    except Exception as e:
        unreal.log(f"  Could not get ubergraph_pages: {e}")

    # Check FunctionGraphs
    try:
        func_graphs = bp.get_editor_property('function_graphs')
        unreal.log(f"  FunctionGraphs: {len(func_graphs) if func_graphs else 0}")
    except Exception as e:
        unreal.log(f"  Could not get function_graphs: {e}")

    # Check SCS
    unreal.log(f"\nSimpleConstructionScript:")
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            all_nodes = scs.get_editor_property('all_nodes')
            unreal.log(f"  AllNodes: {len(all_nodes) if all_nodes else 0}")
            root_nodes = scs.get_editor_property('root_nodes')
            unreal.log(f"  RootNodes: {len(root_nodes) if root_nodes else 0}")
            default_root = scs.get_editor_property('default_scene_root_node')
            unreal.log(f"  DefaultSceneRootNode: {default_root.get_name() if default_root else 'None'}")
        else:
            unreal.log("  SCS is None")
    except Exception as e:
        unreal.log(f"  Could not get SCS: {e}")

    # Check generated class
    unreal.log(f"\nGenerated Class:")
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log(f"  Name: {gen_class.get_name()}")

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log(f"  CDO: {cdo.get_name()}")

            # List ALL components (not just scene)
            all_comps = cdo.get_components_by_class(unreal.ActorComponent)
            unreal.log(f"  ActorComponents ({len(all_comps)}):")
            for comp in all_comps:
                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    mesh_name = mesh.get_name() if mesh else "NO_MESH"
                    unreal.log(f"    - {comp.get_name()}: StaticMesh (mesh={mesh_name})")
                else:
                    unreal.log(f"    - {comp.get_name()}: {comp.get_class().get_name()}")

    # Also check C++ parent CDO for comparison
    unreal.log(f"\nC++ Parent Class CDO:")
    if name == "B_Door":
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.B_Door")
    else:
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.B_BossDoor")

    if cpp_class:
        unreal.log(f"  C++ Class: {cpp_class.get_name()}")
        cpp_cdo = unreal.get_default_object(cpp_class)
        if cpp_cdo:
            all_comps = cpp_cdo.get_components_by_class(unreal.ActorComponent)
            unreal.log(f"  C++ CDO ActorComponents ({len(all_comps)}):")
            for comp in all_comps:
                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    mesh_name = mesh.get_name() if mesh else "NO_MESH"
                    unreal.log(f"    - {comp.get_name()}: StaticMesh (mesh={mesh_name})")
                else:
                    unreal.log(f"    - {comp.get_name()}: {comp.get_class().get_name()}")

# Analyze both
analyze_blueprint("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door", "B_Door")
analyze_blueprint("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor", "B_BossDoor")

unreal.log("\n" + "="*60)
unreal.log("DONE")
unreal.log("="*60)
