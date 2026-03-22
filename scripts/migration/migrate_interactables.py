# migrate_interactables.py
# Migrate B_Interactable and B_Container together in correct order

import unreal

# Map Blueprint to C++ class
# B_Interactable must be migrated BEFORE B_Container
# AB_Interactable has DefaultSceneRoot, which B_Container's SCS components need
MIGRATION_ORDER = [
    # (Blueprint path, C++ class path)
    ("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable", "/Script/SLFConversion.SLFInteractableBase"),
    ("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container", "/Script/SLFConversion.B_Container"),
]


def clear_blueprint_content(bp, keep_scs=True):
    """Clear Blueprint graphs and variables"""
    if not bp:
        return

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

    # Clear new variables
    try:
        new_vars = bp.get_editor_property('new_variables')
        if new_vars:
            new_vars.clear()
    except:
        pass

    # Optionally clear SCS
    if not keep_scs:
        try:
            scs = bp.get_editor_property('simple_construction_script')
            if scs:
                root_nodes = scs.get_editor_property('root_nodes')
                if root_nodes:
                    root_nodes.clear()
        except:
            pass

    return cleared_count


def migrate_blueprint(bp_path, cpp_class_path, keep_scs=True):
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

    # Clear content
    unreal.log_warning(f"  Clearing Blueprint content...")
    cleared = clear_blueprint_content(bp, keep_scs=keep_scs)
    unreal.log_warning(f"  Cleared {cleared} nodes")

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

    return True


def migrate_all():
    """Migrate all interactables in order"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("INTERACTABLES MIGRATION")
    unreal.log_warning("=" * 60)

    success_count = 0
    fail_count = 0

    for bp_path, cpp_class_path in MIGRATION_ORDER:
        bp_name = bp_path.split('/')[-1]
        # B_Interactable should keep SCS (has mesh components)
        # B_Container should also keep SCS (has Lid, PointLight, etc.)
        keep_scs = True

        if migrate_blueprint(bp_path, cpp_class_path, keep_scs=keep_scs):
            success_count += 1
        else:
            fail_count += 1

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning(f"MIGRATION COMPLETE: {success_count} succeeded, {fail_count} failed")
    unreal.log_warning("=" * 60)


if __name__ == "__main__":
    migrate_all()
