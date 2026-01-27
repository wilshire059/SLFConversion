"""
Aggressively clear B_BossDoor and reparent to SLFBossDoor.
Removes ALL Blueprint logic and SCS components that conflict with C++ parent.
"""
import unreal

def clear_and_reparent():
    boss_door_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
    target_class_path = "/Script/SLFConversion.SLFBossDoor"

    print("=" * 60)
    print("AGGRESSIVE CLEAR AND REPARENT B_BossDoor")
    print("=" * 60)

    # Load the Blueprint
    bp = unreal.load_asset(boss_door_path)
    if not bp:
        print(f"ERROR: Could not load {boss_door_path}")
        return False

    print(f"Loaded: {bp.get_name()}")

    # Step 1: Get SCS and remove ALL nodes (not just conflicting ones)
    print("\n--- Step 1: Clearing ALL SCS nodes ---")
    try:
        scs = bp.get_editor_property("simple_construction_script")
        if scs:
            # Get root nodes first
            root_nodes = scs.get_editor_property("root_nodes")
            if root_nodes:
                print(f"  Found {len(root_nodes)} root nodes")

            # Get all nodes
            all_nodes = list(scs.get_all_nodes())
            print(f"  Total SCS nodes: {len(all_nodes)}")

            # Remove each node
            for node in all_nodes:
                try:
                    var_name = str(node.get_editor_property("internal_variable_name"))
                    print(f"  Removing: {var_name}")
                    scs.remove_node(node)
                except Exception as e:
                    print(f"    Error removing {var_name}: {e}")

            # Check if cleared
            remaining = scs.get_all_nodes()
            print(f"  Remaining nodes after clear: {len(remaining)}")
    except Exception as e:
        print(f"  SCS error: {e}")

    # Step 2: Clear ALL uber graphs (EventGraph and others)
    print("\n--- Step 2: Clearing ALL uber graphs ---")
    try:
        uber_graphs = bp.get_editor_property("uber_graph_pages")
        if uber_graphs:
            print(f"  Found {len(uber_graphs)} uber graphs")
            for graph in uber_graphs:
                graph_name = graph.get_name()
                nodes = list(graph.nodes)
                print(f"  Clearing {graph_name}: {len(nodes)} nodes")
                for node in nodes:
                    try:
                        graph.remove_node(node)
                    except:
                        pass
    except Exception as e:
        print(f"  Uber graph error: {e}")

    # Step 3: Clear ALL function graphs
    print("\n--- Step 3: Clearing ALL function graphs ---")
    try:
        func_graphs = bp.get_editor_property("function_graphs")
        if func_graphs:
            print(f"  Found {len(func_graphs)} function graphs")
            for graph in func_graphs:
                graph_name = graph.get_name()
                nodes = list(graph.nodes)
                print(f"  Clearing {graph_name}: {len(nodes)} nodes")
                for node in nodes:
                    try:
                        graph.remove_node(node)
                    except:
                        pass
    except Exception as e:
        print(f"  Function graph error: {e}")

    # Step 4: Clear event dispatchers
    print("\n--- Step 4: Clearing event dispatchers ---")
    try:
        # Event dispatchers may be in new_variables with specific type
        new_vars = bp.get_editor_property("new_variables")
        if new_vars:
            print(f"  Found {len(new_vars)} variables")
    except Exception as e:
        print(f"  Variables error: {e}")

    # Step 5: Save BEFORE reparenting to lock in the cleared state
    print("\n--- Step 5: Saving cleared state ---")
    try:
        unreal.EditorAssetLibrary.save_asset(boss_door_path)
        print("  Saved")
    except Exception as e:
        print(f"  Save error: {e}")

    # Step 6: Reload and reparent
    print("\n--- Step 6: Reloading and reparenting ---")
    bp = unreal.load_asset(boss_door_path)
    if not bp:
        print("  ERROR: Could not reload")
        return False

    target_class = unreal.load_class(None, target_class_path)
    if not target_class:
        print(f"  ERROR: Could not load {target_class_path}")
        return False

    print(f"  Reparenting to: {target_class.get_name()}")
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, target_class)
        print(f"  Reparent result: {result}")
    except Exception as e:
        print(f"  Reparent error: {e}")

    # Step 7: Compile
    print("\n--- Step 7: Compiling ---")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print("  Compiled")
    except Exception as e:
        print(f"  Compile error: {e}")

    # Step 8: Final save
    print("\n--- Step 8: Final save ---")
    try:
        unreal.EditorAssetLibrary.save_asset(boss_door_path)
        print("  Saved")
    except Exception as e:
        print(f"  Save error: {e}")

    # Verify
    print("\n--- Verification ---")
    bp_check = unreal.load_asset(boss_door_path)
    if bp_check:
        gen = bp_check.generated_class()
        if gen:
            print(f"  Generated class: {gen.get_name()}")
            parent = gen.get_class()
            print(f"  Class type: {parent.get_name()}")

            cdo = unreal.get_default_object(gen)
            if cdo:
                comps = cdo.get_components_by_class(unreal.SceneComponent)
                print(f"  Total scene components: {len(comps)}")
                for c in comps:
                    print(f"    - {c.get_name()} ({c.get_class().get_name()})")
                    if hasattr(c, 'relative_scale3d'):
                        scale = c.relative_scale3d
                        print(f"      Scale: ({scale.x}, {scale.y}, {scale.z})")

    print("\n" + "=" * 60)
    print("DONE")
    print("=" * 60)

if __name__ == "__main__":
    clear_and_reparent()
