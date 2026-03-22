# migrate_interactables_v2.py
# Migrate B_Interactable and B_Container - clear SCS DefaultSceneRoot conflicts

import unreal

# Map Blueprint to C++ class
MIGRATION_ORDER = [
    ("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable", "/Script/SLFConversion.SLFInteractableBase"),
    ("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container", "/Script/SLFConversion.B_Container"),
]


def clear_all_blueprint_content(bp):
    """Clear ALL Blueprint content including SCS"""
    if not bp:
        return 0

    cleared_count = 0

    # Clear uber graph pages (EventGraphs)
    try:
        uber_graphs = bp.get_editor_property('ubergraph_pages')
        if uber_graphs:
            for graph in uber_graphs:
                try:
                    nodes = graph.get_editor_property('nodes')
                    if nodes:
                        cleared_count += len(nodes)
                        nodes.clear()
                except:
                    pass
    except:
        pass

    # Clear function graphs
    try:
        func_graphs = bp.get_editor_property('function_graphs')
        if func_graphs:
            for graph in func_graphs:
                try:
                    nodes = graph.get_editor_property('nodes')
                    if nodes:
                        cleared_count += len(nodes)
                        nodes.clear()
                except:
                    pass
    except:
        pass

    # Clear macro graphs
    try:
        macro_graphs = bp.get_editor_property('macro_graphs')
        if macro_graphs:
            for graph in macro_graphs:
                try:
                    nodes = graph.get_editor_property('nodes')
                    if nodes:
                        nodes.clear()
                except:
                    pass
    except:
        pass

    # Clear delegate graphs
    try:
        delegate_graphs = bp.get_editor_property('delegate_signature_graphs')
        if delegate_graphs:
            for graph in delegate_graphs:
                try:
                    nodes = graph.get_editor_property('nodes')
                    if nodes:
                        nodes.clear()
                except:
                    pass
    except:
        pass

    # Clear new variables (important for property conflicts!)
    try:
        new_vars = bp.get_editor_property('new_variables')
        if new_vars:
            var_count = len(new_vars)
            new_vars.clear()
            unreal.log_warning(f"    Cleared {var_count} variables")
    except:
        pass

    # Clear SCS completely (to remove DefaultSceneRoot conflict)
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            # Clear root nodes
            root_nodes = scs.get_editor_property('root_nodes')
            if root_nodes:
                root_count = len(root_nodes)
                root_nodes.clear()
                unreal.log_warning(f"    Cleared {root_count} SCS root nodes")

            # Clear default scene root
            try:
                scs.set_editor_property('default_scene_root_node', None)
                unreal.log_warning(f"    Cleared SCS default scene root")
            except:
                pass
    except Exception as e:
        unreal.log_warning(f"    SCS clear error: {e}")

    return cleared_count


def migrate_blueprint(bp_path, cpp_class_path):
    """Migrate a single Blueprint to C++ parent"""
    bp_name = bp_path.split('/')[-1]
    unreal.log_warning(f"\n{'='*50}")
    unreal.log_warning(f"Migrating {bp_name}")
    unreal.log_warning(f"{'='*50}")

    # Load Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"  Could not load: {bp_path}")
        return False

    # Clear ALL content including SCS
    unreal.log_warning(f"  Clearing Blueprint content (including SCS)...")
    cleared = clear_all_blueprint_content(bp)
    unreal.log_warning(f"  Cleared {cleared} nodes total")

    # Save cleared Blueprint
    unreal.log_warning(f"  Saving cleared Blueprint...")
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # Reload
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"  Could not reload Blueprint")
        return False

    # Load C++ class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        unreal.log_error(f"  Could not load C++ class: {cpp_class_path}")
        return False
    unreal.log_warning(f"  C++ class: {cpp_class.get_name()}")

    # Reparent
    unreal.log_warning(f"  Reparenting...")
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if result:
            unreal.log_warning(f"  SUCCESS - Reparented!")
        else:
            unreal.log_error(f"  FAILED - Reparent returned false")
            return False
    except Exception as e:
        unreal.log_error(f"  FAILED - Error: {e}")
        return False

    # Compile
    unreal.log_warning(f"  Compiling...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    except Exception as e:
        unreal.log_warning(f"  Compile note: {e}")

    # Final save
    unreal.log_warning(f"  Final save...")
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # Verify
    bp = unreal.load_asset(bp_path)
    if bp:
        try:
            gen_class = bp.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    try:
                        root = cdo.get_editor_property('root_component')
                        root_name = root.get_name() if root else "None"
                        unreal.log_warning(f"  Root component: {root_name}")
                    except:
                        pass
        except:
            pass

    return True


def migrate_all():
    """Migrate all interactables in order"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("INTERACTABLES MIGRATION V2")
    unreal.log_warning("(With SCS clearing for DefaultSceneRoot conflicts)")
    unreal.log_warning("=" * 60)

    success_count = 0
    fail_count = 0

    for bp_path, cpp_class_path in MIGRATION_ORDER:
        if migrate_blueprint(bp_path, cpp_class_path):
            success_count += 1
        else:
            fail_count += 1

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning(f"MIGRATION COMPLETE: {success_count} succeeded, {fail_count} failed")
    unreal.log_warning("=" * 60)


if __name__ == "__main__":
    migrate_all()
