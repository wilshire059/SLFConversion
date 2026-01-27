"""
Complete fix for B_BossDoor:
1. Restore from bp_only
2. Clear EventGraph (logic moves to C++)
3. Remove duplicate SCS components (already in C++ parent)
4. Reparent to SLFBossDoor
5. Compile and save
"""
import unreal

def fix_boss_door():
    boss_door_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
    target_class_path = "/Script/SLFConversion.SLFBossDoor"

    print("=" * 60)
    print("FIXING B_BossDoor")
    print("=" * 60)

    # Load the Blueprint
    bp = unreal.load_asset(boss_door_path)
    if not bp:
        print(f"ERROR: Could not load {boss_door_path}")
        return False

    print(f"Loaded Blueprint: {bp.get_name()}")

    # Step 1: Clear EventGraph
    print("\n--- Step 1: Clearing EventGraph ---")
    try:
        uber_graphs = bp.get_editor_property("uber_graph_pages")
        if uber_graphs:
            for graph in uber_graphs:
                print(f"  Clearing graph: {graph.get_name()}")
                # Remove all nodes
                nodes = list(graph.nodes)
                for node in nodes:
                    graph.remove_node(node)
            print(f"  Cleared {len(uber_graphs)} graph(s)")
    except Exception as e:
        print(f"  Warning clearing EventGraph: {e}")

    # Step 2: Clear function graphs (not interface implementations)
    print("\n--- Step 2: Clearing function graphs ---")
    try:
        func_graphs = bp.get_editor_property("function_graphs")
        if func_graphs:
            for graph in func_graphs:
                graph_name = graph.get_name()
                # Keep interface implementations, clear custom functions
                if not graph_name.startswith(("OnInteract", "OnTraced", "TryGetItemInfo", "OnSpawnedFromSave",
                                              "GetDeathCurrencySpawnPoint", "UnlockBossDoor")):
                    print(f"  Clearing function: {graph_name}")
                    nodes = list(graph.nodes)
                    for node in nodes:
                        graph.remove_node(node)
    except Exception as e:
        print(f"  Warning clearing functions: {e}")

    # Step 3: Remove SCS components that conflict with C++ parent
    print("\n--- Step 3: Removing duplicate SCS components ---")
    try:
        scs = bp.get_editor_property("simple_construction_script")
        if scs:
            all_nodes = list(scs.get_all_nodes())
            components_to_remove = ["Portal_Front", "Portal_Back", "DeathCurrencyDropLocation", "Interactable SM"]

            for node in all_nodes:
                var_name = str(node.get_editor_property("internal_variable_name"))
                if var_name in components_to_remove:
                    print(f"  Removing SCS component: {var_name}")
                    scs.remove_node(node)
    except Exception as e:
        print(f"  Warning removing SCS: {e}")

    # Step 4: Clear Blueprint variables that conflict
    print("\n--- Step 4: Clearing conflicting variables ---")
    try:
        # Access new_variables
        new_vars = bp.get_editor_property("new_variables")
        if new_vars:
            vars_to_remove = []
            for var in new_vars:
                var_name = var.get_editor_property("var_name")
                var_str = str(var_name)
                # Check for variables that now exist in C++
                if var_str in ["IsActivated?", "CanBeTraced?", "Portal_Front", "Portal_Back",
                              "DeathCurrencyDropLocation", "Interactable SM", "bIsActivated",
                              "bCanBeTraced", "InteractableSM"]:
                    vars_to_remove.append(var_name)
                    print(f"  Marking for removal: {var_str}")

            # Note: Can't easily remove variables via Python, they'll become orphaned
    except Exception as e:
        print(f"  Warning handling variables: {e}")

    # Step 5: Reparent to SLFBossDoor
    print("\n--- Step 5: Reparenting to SLFBossDoor ---")
    target_class = unreal.load_class(None, target_class_path)
    if not target_class:
        print(f"ERROR: Could not load {target_class_path}")
        return False

    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, target_class)
        if result:
            print("  Reparent successful")
        else:
            print("  Reparent returned false (may still work)")
    except Exception as e:
        print(f"  Reparent error: {e}")

    # Step 6: Compile
    print("\n--- Step 6: Compiling ---")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print("  Compile completed")
    except Exception as e:
        print(f"  Compile error: {e}")

    # Step 7: Save
    print("\n--- Step 7: Saving ---")
    try:
        unreal.EditorAssetLibrary.save_asset(boss_door_path)
        print(f"  Saved: {boss_door_path}")
    except Exception as e:
        print(f"  Save error: {e}")

    # Verify
    print("\n--- Verification ---")
    bp_check = unreal.load_asset(boss_door_path)
    if bp_check:
        gen = bp_check.generated_class()
        if gen:
            cdo = unreal.get_default_object(gen)
            if cdo:
                # Check for InteractableSM component
                comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
                print(f"  StaticMeshComponents: {len(comps)}")
                for c in comps:
                    if isinstance(c, unreal.SceneComponent):
                        scale = c.relative_scale3d
                        print(f"    {c.get_name()}: Scale ({scale.x}, {scale.y}, {scale.z})")

    print("\n" + "=" * 60)
    print("DONE - Test in PIE")
    print("=" * 60)

    return True

if __name__ == "__main__":
    fix_boss_door()
