# migrate_container_v3.py
# Aggressive B_Container migration: clear variables, clear graphs, clear SCS roots, reparent

import unreal

BP_CONTAINER_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
CPP_CLASS_PATH = "/Script/SLFConversion.B_Container"

def clear_all_blueprint_content(bp):
    """Clear variables, graphs, and SCS from Blueprint"""
    if not bp:
        return

    bp_name = bp.get_name()

    # Step 1: Clear new variables (like OpenMontage, OpenVFX that conflict with C++)
    try:
        new_vars = bp.get_editor_property('new_variables')
        if new_vars:
            var_count = len(new_vars)
            new_vars.clear()
            unreal.log_warning(f"  Cleared {var_count} Blueprint variables")
    except Exception as e:
        unreal.log_warning(f"  Could not clear variables: {e}")

    # Step 2: Clear uber graph pages (EventGraphs)
    try:
        uber_graphs = bp.get_editor_property('ubergraph_pages')
        for graph in uber_graphs:
            try:
                nodes = graph.get_editor_property('nodes')
                if nodes:
                    nodes.clear()
            except:
                pass
        unreal.log_warning(f"  Cleared uber graph pages")
    except Exception as e:
        unreal.log_warning(f"  Uber graphs: {e}")

    # Step 3: Clear function graphs
    try:
        func_graphs = bp.get_editor_property('function_graphs')
        for graph in func_graphs:
            try:
                nodes = graph.get_editor_property('nodes')
                if nodes:
                    nodes.clear()
            except:
                pass
        unreal.log_warning(f"  Cleared function graphs")
    except Exception as e:
        unreal.log_warning(f"  Function graphs: {e}")

    # Step 4: Clear macro graphs
    try:
        macro_graphs = bp.get_editor_property('macro_graphs')
        for graph in macro_graphs:
            try:
                nodes = graph.get_editor_property('nodes')
                if nodes:
                    nodes.clear()
            except:
                pass
        unreal.log_warning(f"  Cleared macro graphs")
    except Exception as e:
        pass

    # Step 5: Clear delegate signature graphs
    try:
        delegate_graphs = bp.get_editor_property('delegate_signature_graphs')
        for graph in delegate_graphs:
            try:
                nodes = graph.get_editor_property('nodes')
                if nodes:
                    nodes.clear()
            except:
                pass
    except:
        pass

    # Step 6: Clear SCS root nodes (but keep the SCS structure)
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            root_nodes = scs.get_editor_property('root_nodes')
            if root_nodes:
                root_count = len(root_nodes)
                root_nodes.clear()
                unreal.log_warning(f"  Cleared {root_count} SCS root nodes")

            # Clear default scene root
            try:
                scs.set_editor_property('default_scene_root_node', None)
            except:
                pass
    except Exception as e:
        unreal.log_warning(f"  SCS: {e}")


def migrate_container():
    """Full B_Container migration with aggressive clearing"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("B_CONTAINER MIGRATION V3 - AGGRESSIVE CLEAR")
    unreal.log_warning("=" * 60)

    # Load Blueprint
    bp = unreal.load_asset(BP_CONTAINER_PATH)
    if not bp:
        unreal.log_error(f"Could not load: {BP_CONTAINER_PATH}")
        return False

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    # Check current parent
    try:
        gen_class = bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            if parent:
                unreal.log_warning(f"Current parent: {parent.get_name()}")
    except:
        pass

    # Phase 1: Clear ALL Blueprint content
    unreal.log_warning("\nPhase 1: Clear ALL Blueprint content")
    clear_all_blueprint_content(bp)

    # Phase 2: Save after clearing
    unreal.log_warning("\nPhase 2: Save cleared Blueprint")
    try:
        unreal.EditorAssetLibrary.save_asset(BP_CONTAINER_PATH, only_if_is_dirty=False)
        unreal.log_warning("  Saved")
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # Phase 3: Reload
    unreal.log_warning("\nPhase 3: Reload Blueprint")
    bp = unreal.load_asset(BP_CONTAINER_PATH)
    if not bp:
        unreal.log_error("Could not reload Blueprint")
        return False

    # Phase 4: Load C++ class
    unreal.log_warning("\nPhase 4: Load C++ class")
    cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
    if not cpp_class:
        unreal.log_error(f"Could not load: {CPP_CLASS_PATH}")
        return False
    unreal.log_warning(f"  C++ class: {cpp_class.get_name()}")

    # Phase 5: Reparent
    unreal.log_warning("\nPhase 5: Reparent Blueprint")
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if result:
            unreal.log_warning("  Reparent SUCCESSFUL!")
        else:
            unreal.log_error("  Reparent FAILED")
            return False
    except Exception as e:
        unreal.log_error(f"  Reparent error: {e}")
        return False

    # Phase 6: Compile
    unreal.log_warning("\nPhase 6: Compile")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("  Compiled")
    except Exception as e:
        unreal.log_warning(f"  Compile note: {e}")

    # Phase 7: Final save
    unreal.log_warning("\nPhase 7: Final save")
    try:
        unreal.EditorAssetLibrary.save_asset(BP_CONTAINER_PATH, only_if_is_dirty=False)
        unreal.log_warning("  Saved")
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # Phase 8: Verify parent
    unreal.log_warning("\nPhase 8: Verify parent class")
    bp = unreal.load_asset(BP_CONTAINER_PATH)
    if bp:
        try:
            gen_class = bp.generated_class()
            if gen_class:
                parent = gen_class.get_super_class()
                if parent:
                    unreal.log_warning(f"  New parent: {parent.get_name()}")
                    if parent.get_name() == "B_Container":
                        unreal.log_warning("  SUCCESS - Now parented to C++ B_Container!")
                    else:
                        unreal.log_warning(f"  Parent is {parent.get_name()}, expected B_Container")
        except Exception as e:
            unreal.log_warning(f"  Verify error: {e}")

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning("MIGRATION COMPLETE")
    unreal.log_warning("=" * 60)
    return True


if __name__ == "__main__":
    migrate_container()
