#!/usr/bin/env python3
"""
Fix Level Blueprint stale nodes by force-reconstructing them.

The issue: Level Blueprint has cached pins referencing old Blueprint FNames.
C++ properties use different FNames, causing "pin no longer exists" errors.

This script finds and reconstructs problematic nodes in the Level Blueprint.
"""

import unreal

def log(msg):
    print(f"[FixLevelBP] {msg}")
    unreal.log_warning(f"[FixLevelBP] {msg}")

def get_blueprint_graphs(blueprint):
    """Get all graphs in a Blueprint including Level Blueprints."""
    graphs = []

    # Try to get function graphs
    if hasattr(blueprint, 'function_graphs'):
        for graph in blueprint.function_graphs:
            graphs.append(graph)

    # Try to get uber graphs (includes EventGraph)
    if hasattr(blueprint, 'uber_graphs'):
        for graph in blueprint.uber_graphs:
            graphs.append(graph)

    return graphs

def reconstruct_node(node):
    """Force reconstruct a Blueprint node to refresh its pins."""
    if node is None:
        return False

    try:
        # Get the node class name for logging
        node_name = node.get_name() if hasattr(node, 'get_name') else str(node)
        node_class = node.get_class().get_name() if hasattr(node, 'get_class') else "Unknown"

        log(f"  Attempting to reconstruct: {node_name} ({node_class})")

        # Use BlueprintEditorLibrary to reconstruct if available
        if hasattr(unreal, 'BlueprintEditorLibrary') and hasattr(unreal.BlueprintEditorLibrary, 'reconstruct_node'):
            unreal.BlueprintEditorLibrary.reconstruct_node(node)
            log(f"  Reconstructed via BlueprintEditorLibrary")
            return True

        # Alternative: Mark node as dirty
        if hasattr(node, 'mark_dirty'):
            node.mark_dirty()
            log(f"  Marked dirty")
            return True

        return False
    except Exception as e:
        log(f"  Error reconstructing node: {e}")
        return False

def find_problematic_nodes(graph):
    """Find nodes that might have stale pins."""
    problematic = []

    if not hasattr(graph, 'nodes'):
        return problematic

    for node in graph.nodes:
        if node is None:
            continue

        node_name = ""
        try:
            node_name = node.get_name() if hasattr(node, 'get_name') else str(node)
            node_class = node.get_class().get_name() if hasattr(node, 'get_class') else ""
        except:
            continue

        # SpawnActor nodes
        if 'SpawnActor' in node_class or 'SpawnActor' in node_name:
            problematic.append(node)
            log(f"  Found SpawnActor node: {node_name}")

        # Get nodes for GameInstance
        if 'K2Node_VariableGet' in node_class or 'VariableGet' in node_name:
            problematic.append(node)
            log(f"  Found Variable Get node: {node_name}")

        # EventToggleCinematicMode
        if 'CinematicMode' in node_name or 'CinematicMode' in node_class:
            problematic.append(node)
            log(f"  Found CinematicMode node: {node_name}")

    return problematic

def fix_level_blueprint(level_path):
    """Fix a level blueprint's stale nodes."""
    log(f"Processing: {level_path}")

    # Load the level
    level_asset = unreal.EditorAssetLibrary.load_asset(level_path)
    if not level_asset:
        log(f"  ERROR: Could not load level: {level_path}")
        return False

    log(f"  Loaded level: {level_asset.get_name()}")

    # Get the world from the level
    world = None
    if hasattr(level_asset, 'get_world'):
        world = level_asset.get_world()

    # Try to get level blueprint various ways
    level_bp = None

    # Method 1: Direct level script blueprint access
    if hasattr(level_asset, 'level_script_blueprint'):
        level_bp = level_asset.level_script_blueprint
        log(f"  Found level_script_blueprint attribute")

    # Method 2: Via PersistentLevel
    if level_bp is None and hasattr(level_asset, 'persistent_level'):
        persistent = level_asset.persistent_level
        if persistent and hasattr(persistent, 'level_script_blueprint'):
            level_bp = persistent.level_script_blueprint
            log(f"  Found via persistent_level")

    # Method 3: Try loading the _C class
    if level_bp is None:
        level_bp_path = f"{level_path}_C"
        try:
            level_bp = unreal.load_object(None, level_bp_path)
            log(f"  Loaded _C class")
        except:
            pass

    if level_bp is None:
        log(f"  Could not find Level Script Blueprint")

        # Try alternative: Load via asset registry
        asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

        # Search for level blueprint assets
        bp_path = level_path.replace('/Game/', '/Game/').rstrip('.umap')
        log(f"  Searching for Blueprint at: {bp_path}")

        return False

    log(f"  Level Blueprint: {level_bp.get_name() if hasattr(level_bp, 'get_name') else level_bp}")

    # Get graphs
    graphs = get_blueprint_graphs(level_bp)
    log(f"  Found {len(graphs)} graphs")

    nodes_fixed = 0
    for graph in graphs:
        graph_name = graph.get_name() if hasattr(graph, 'get_name') else str(graph)
        log(f"  Processing graph: {graph_name}")

        problematic = find_problematic_nodes(graph)
        for node in problematic:
            if reconstruct_node(node):
                nodes_fixed += 1

    # If we found the blueprint, try to compile it
    if hasattr(unreal, 'BlueprintEditorLibrary') and level_bp:
        try:
            # Mark modified
            if hasattr(unreal.EditorAssetLibrary, 'mark_dirty'):
                unreal.EditorAssetLibrary.mark_dirty(level_path)

            # Reconstruct all nodes
            if hasattr(unreal.BlueprintEditorLibrary, 'reconstruct_all_nodes'):
                unreal.BlueprintEditorLibrary.reconstruct_all_nodes(level_bp)
                log(f"  Called reconstruct_all_nodes")

            # Compile
            if hasattr(unreal.BlueprintEditorLibrary, 'compile_blueprint'):
                result = unreal.BlueprintEditorLibrary.compile_blueprint(level_bp)
                log(f"  Compile result: {result}")
        except Exception as e:
            log(f"  Error during compile: {e}")

    log(f"  Fixed {nodes_fixed} nodes")
    return nodes_fixed > 0 or level_bp is not None

def main():
    log("=" * 70)
    log("FIXING LEVEL BLUEPRINT STALE NODES")
    log("=" * 70)

    # List of levels to process
    levels = [
        "/Game/SoulslikeFramework/Maps/L_Demo_Showcase",
    ]

    for level_path in levels:
        fix_level_blueprint(level_path)

    log("")
    log("=" * 70)
    log("DONE")
    log("")
    log("If errors persist, these nodes may need manual refresh in editor:")
    log("  - SpawnActor B_SequenceActor (Sequence to Play, Settings pins)")
    log("  - Get node for FirstTimeOnDemoLevel")
    log("  - EventToggleCinematicMode (Active? pin)")
    log("=" * 70)

if __name__ == "__main__":
    main()
