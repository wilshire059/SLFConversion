# migrate_interactables_fix.py
# Migrates interactable Blueprints to C++ while PRESERVING the Blueprint SCS
#
# The key is to:
# 1. Clear only EventGraph and function graphs (NOT SCS/variables)
# 2. Reparent to C++
# 3. The Blueprint SCS components (meshes, etc.) are preserved

import unreal

# Map Blueprint paths to their C++ parent classes
MIGRATION_MAP = [
    ("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable", "/Script/SLFConversion.B_Interactable"),
    ("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container", "/Script/SLFConversion.B_Container"),
    ("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint", "/Script/SLFConversion.B_RestingPoint"),
    ("/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible", "/Script/SLFConversion.B_Destructible"),
    ("/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField", "/Script/SLFConversion.B_Chaos_ForceField"),
]


def clear_eventgraph_only(bp):
    """Clear only EventGraph nodes, preserving SCS and variables."""
    # Access the UbergraphPages (EventGraph)
    try:
        for graph in bp.ubergraph_pages:
            graph_name = graph.get_name()
            if "EventGraph" in graph_name:
                # Get nodes and remove them
                nodes = list(graph.nodes)
                for node in nodes:
                    graph.remove_node(node)
                unreal.log_warning(f"    Cleared {len(nodes)} nodes from {graph_name}")
    except Exception as e:
        unreal.log_warning(f"    Could not clear EventGraph: {e}")


def migrate_blueprint(bp_path, cpp_class_path):
    """Migrate a single Blueprint to C++ parent while preserving SCS."""
    bp_name = bp_path.split('/')[-1]
    unreal.log_warning(f"\nMigrating {bp_name}")
    unreal.log_warning("=" * 50)

    # Load Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"  Could not load: {bp_path}")
        return False

    unreal.log_warning(f"  Target C++ class: {cpp_class_path}")

    # Use SLFAutomationLibrary to clear graphs but KEEP SCS and variables
    # ClearGraphsKeepVariablesNoCompile preserves SCS and variables
    try:
        result = unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
        if result:
            unreal.log_warning("  Cleared graphs (kept SCS and variables)")
        else:
            unreal.log_warning("  Graph clearing returned false (may still be ok)")
    except Exception as e:
        unreal.log_warning(f"  Could not use SLFAutomationLibrary: {e}")
        # Fallback - just skip clearing
        pass

    # Save cleared Blueprint
    unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.log_warning("  Saved cleared Blueprint")

    # Reparent using SLFAutomationLibrary (skips validation)
    # NOTE: ReparentBlueprint expects string path, not class object
    try:
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_class_path)
        if result:
            unreal.log_warning("  SUCCESS - Reparented!")
        else:
            unreal.log_error("  FAILED - Reparent returned false")
            return False
    except Exception as e:
        unreal.log_error(f"  FAILED - Reparent exception: {e}")
        return False

    # Compile and save
    try:
        unreal.SLFAutomationLibrary.compile_and_save(bp)
        unreal.log_warning("  Compiled and saved")
    except Exception as e:
        unreal.log_warning(f"  Compile/save exception (may be ok): {e}")

    # Verify root component
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                root = cdo.get_editor_property('root_component')
                if root:
                    unreal.log_warning(f"  Root component: {root.get_name()} ({root.get_class().get_name()})")
                else:
                    unreal.log_warning("  Root component: None (will be set by Blueprint SCS at spawn time)")
            except:
                pass

    return True


def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("INTERACTABLES FIX MIGRATION")
    unreal.log_warning("Preserves Blueprint SCS while reparenting to C++")
    unreal.log_warning("=" * 60)

    succeeded = 0
    failed = 0

    for bp_path, cpp_class_path in MIGRATION_MAP:
        if migrate_blueprint(bp_path, cpp_class_path):
            succeeded += 1
        else:
            failed += 1

    unreal.log_warning(f"\nMIGRATION COMPLETE: {succeeded} succeeded, {failed} failed")
    unreal.log_warning("=" * 60)


if __name__ == "__main__":
    main()
