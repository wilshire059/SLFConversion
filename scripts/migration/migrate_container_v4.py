# migrate_container_v4.py
# Proper B_Container migration using BlueprintGeneratedClass approach

import unreal

BP_CONTAINER_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
CPP_CLASS_PATH = "/Script/SLFConversion.B_Container"

def get_blueprint_from_generated_class(asset):
    """Get BlueprintGeneratedClass if this is a Blueprint"""
    try:
        if hasattr(asset, 'generated_class'):
            return asset, asset.generated_class()
    except:
        pass
    return asset, None

def clear_blueprint_content(bp):
    """Clear all Blueprint content aggressively"""
    if not bp:
        return

    bp_name = bp.get_name()

    # Get the GeneratedClass to access more properties
    gen_class = None
    try:
        gen_class = bp.generated_class()
    except:
        pass

    # Clear uber graph pages (EventGraphs)
    cleared_nodes = 0
    try:
        uber_graphs = bp.get_editor_property('ubergraph_pages')
        if uber_graphs:
            for graph in uber_graphs:
                try:
                    nodes = graph.get_editor_property('nodes')
                    if nodes:
                        cleared_nodes += len(nodes)
                        nodes.clear()
                except Exception as e:
                    pass
        unreal.log_warning(f"  Cleared {cleared_nodes} uber graph nodes")
    except Exception as e:
        # Try alternative method
        pass

    # Clear function graphs
    func_cleared = 0
    try:
        func_graphs = bp.get_editor_property('function_graphs')
        if func_graphs:
            for graph in func_graphs:
                try:
                    nodes = graph.get_editor_property('nodes')
                    if nodes:
                        func_cleared += len(nodes)
                        nodes.clear()
                except:
                    pass
        unreal.log_warning(f"  Cleared {func_cleared} function graph nodes")
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

    # Clear inherited graphs
    try:
        inherited_graphs = bp.get_editor_property('inherited_graphs')
        if inherited_graphs:
            inherited_graphs.clear()
    except:
        pass

    # Try new_variables
    try:
        new_vars = bp.get_editor_property('new_variables')
        if new_vars:
            unreal.log_warning(f"  Clearing {len(new_vars)} new variables")
            new_vars.clear()
    except:
        pass

    # Try clearing SCS (but NOT completely - just deregister conflicts)
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            unreal.log_warning(f"  Found SCS (keeping for component hierarchy)")
    except:
        pass


def migrate_container():
    """Migrate B_Container to C++ parent class"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("B_CONTAINER MIGRATION V4")
    unreal.log_warning("=" * 60)

    # Step 1: Load Blueprint asset
    unreal.log_warning("\nStep 1: Loading Blueprint...")
    bp = unreal.load_asset(BP_CONTAINER_PATH)
    if not bp:
        unreal.log_error(f"  Could not load: {BP_CONTAINER_PATH}")
        return False

    # Check current parent
    try:
        gen_class = bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            unreal.log_warning(f"  Current parent: {parent.get_name() if parent else 'None'}")
    except Exception as e:
        unreal.log_warning(f"  Could not check parent: {e}")

    # Step 2: Clear Blueprint content
    unreal.log_warning("\nStep 2: Clearing Blueprint content...")
    clear_blueprint_content(bp)

    # Step 3: Save cleared Blueprint
    unreal.log_warning("\nStep 3: Saving cleared Blueprint...")
    try:
        result = unreal.EditorAssetLibrary.save_asset(BP_CONTAINER_PATH, only_if_is_dirty=False)
        unreal.log_warning(f"  Save result: {result}")
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # Step 4: Reload Blueprint
    unreal.log_warning("\nStep 4: Reloading Blueprint...")
    bp = unreal.load_asset(BP_CONTAINER_PATH)
    if not bp:
        unreal.log_error("  Could not reload")
        return False

    # Step 5: Load C++ class
    unreal.log_warning("\nStep 5: Loading C++ class...")
    cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
    if not cpp_class:
        unreal.log_error(f"  Could not load: {CPP_CLASS_PATH}")
        return False
    unreal.log_warning(f"  C++ class: {cpp_class.get_name()}")

    # Step 6: Reparent
    unreal.log_warning("\nStep 6: Reparenting Blueprint...")
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if result:
            unreal.log_warning("  SUCCESS - Reparented!")
        else:
            unreal.log_error("  FAILED - Reparent returned false")
            return False
    except Exception as e:
        unreal.log_error(f"  FAILED - Error: {e}")
        return False

    # Step 7: Compile
    unreal.log_warning("\nStep 7: Compiling Blueprint...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("  Compiled")
    except Exception as e:
        unreal.log_warning(f"  Compile note: {e}")

    # Step 8: Final save
    unreal.log_warning("\nStep 8: Final save...")
    try:
        result = unreal.EditorAssetLibrary.save_asset(BP_CONTAINER_PATH, only_if_is_dirty=False)
        unreal.log_warning(f"  Save result: {result}")
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # Step 9: Verify
    unreal.log_warning("\nStep 9: Verifying...")
    bp = unreal.load_asset(BP_CONTAINER_PATH)
    if bp:
        try:
            gen_class = bp.generated_class()
            if gen_class:
                parent = gen_class.get_super_class()
                unreal.log_warning(f"  Final parent: {parent.get_name() if parent else 'None'}")
        except:
            pass

    # Step 10: Check CDO and components
    unreal.log_warning("\nStep 10: Checking CDO...")
    if bp:
        try:
            gen_class = bp.generated_class()
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                unreal.log_warning(f"  CDO class: {cdo.get_class().get_name()}")

                # Check root component
                try:
                    root = cdo.get_editor_property('root_component')
                    if root:
                        unreal.log_warning(f"  Root component: {root.get_name()} ({root.get_class().get_name()})")
                    else:
                        unreal.log_warning("  Root component: None")
                except:
                    pass
        except Exception as e:
            unreal.log_warning(f"  CDO check error: {e}")

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning("MIGRATION COMPLETE")
    unreal.log_warning("=" * 60)
    return True


if __name__ == "__main__":
    migrate_container()
