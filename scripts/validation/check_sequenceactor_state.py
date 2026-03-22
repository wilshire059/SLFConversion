#!/usr/bin/env python3
"""
Check B_SequenceActor Blueprint state - does it have EventGraph logic?
"""

import unreal

def log(msg):
    print(f"[CheckSequenceActor] {msg}")
    unreal.log_warning(f"[CheckSequenceActor] {msg}")

def main():
    log("=" * 70)
    log("CHECKING B_SequenceActor STATE")
    log("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"

    log(f"Loading: {bp_path}")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load: {bp_path}")
        return

    log(f"Blueprint: {bp.get_name()}")

    # Get parent class
    gen_class = bp.generated_class()
    if gen_class:
        log(f"Generated Class: {gen_class.get_name()}")

    # Use SLFAutomationLibrary to get parent
    try:
        parent_path = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"Parent Class Path: {parent_path}")
    except Exception as e:
        log(f"Parent class error: {e}")

    # Check CDO properties
    try:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            log("")
            log("CDO Properties:")
            try:
                seq = cdo.get_editor_property('sequence_to_play')
                log(f"  SequenceToPlay: {seq if seq else 'None'}")
            except Exception as e:
                log(f"  SequenceToPlay error: {e}")

            try:
                can_skip = cdo.get_editor_property('can_be_skipped')
                log(f"  CanBeSkipped: {can_skip}")
            except Exception as e:
                log(f"  CanBeSkipped error: {e}")
    except Exception as e:
        log(f"CDO error: {e}")

    # Check UberGraph pages (EventGraph)
    log("")
    log("Checking EventGraph...")
    try:
        graphs = bp.get_editor_property('ubergraph_pages')
        if graphs:
            log(f"  UberGraph pages: {len(graphs)}")
            for graph in graphs:
                log(f"    Graph: {graph.get_name()}")
                nodes = graph.get_editor_property('nodes')
                if nodes:
                    log(f"      Nodes: {len(nodes)}")
                    for node in nodes[:10]:  # Show first 10
                        log(f"        - {node.get_class().get_name()}: {node.get_name()}")
                    if len(nodes) > 10:
                        log(f"        ... and {len(nodes) - 10} more")
                else:
                    log(f"      Nodes: 0 (EMPTY)")
        else:
            log("  No UberGraph pages (EventGraph empty)")
    except Exception as e:
        log(f"  EventGraph error: {e}")

    # Check function graphs
    log("")
    log("Checking Function Graphs...")
    try:
        func_graphs = bp.get_editor_property('function_graphs')
        if func_graphs:
            log(f"  Function graphs: {len(func_graphs)}")
            for fg in func_graphs:
                nodes = fg.get_editor_property('nodes')
                log(f"    {fg.get_name()}: {len(nodes) if nodes else 0} nodes")
        else:
            log("  No function graphs")
    except Exception as e:
        log(f"  Function graphs error: {e}")

    # Check if has EventGraph logic
    log("")
    try:
        has_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(bp)
        log(f"HasEventGraphLogic: {has_logic}")
    except Exception as e:
        log(f"HasEventGraphLogic error: {e}")

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
