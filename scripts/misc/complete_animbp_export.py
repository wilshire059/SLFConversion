#!/usr/bin/env python3
"""
Complete AnimBP Export - Exports ALL graphs, nodes, variables, and connections
to JSON for comprehensive comparison between bp_only and current projects.

Run this on BOTH projects:
1. Run on bp_only project first -> outputs to animbp_export_bp_only.json
2. Run on SLFConversion project -> outputs to animbp_export_current.json
3. Diff the two JSON files to find exact differences
"""

import unreal
import json
import os

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/animbp_comparison"

def log(msg):
    print(f"[Export] {msg}")
    unreal.log_warning(f"[Export] {msg}")

def export_pin_info(pin):
    """Export detailed pin information."""
    pin_data = {
        "name": str(pin.pin_name) if hasattr(pin, 'pin_name') else str(pin.get_name()),
        "type": "",
        "direction": "",
        "connected_to": [],
        "default_value": "",
    }

    try:
        if hasattr(pin, 'pin_type'):
            pin_type = pin.pin_type
            if hasattr(pin_type, 'pin_category'):
                pin_data["type"] = str(pin_type.pin_category)

        if hasattr(pin, 'direction'):
            pin_data["direction"] = str(pin.direction)

        if hasattr(pin, 'default_value'):
            pin_data["default_value"] = str(pin.default_value)

        # Get connected pins
        if hasattr(pin, 'linked_to'):
            for linked in pin.linked_to:
                try:
                    owner = linked.get_owning_node()
                    pin_data["connected_to"].append({
                        "node": owner.get_name() if owner else "Unknown",
                        "pin": str(linked.pin_name) if hasattr(linked, 'pin_name') else str(linked.get_name()),
                    })
                except:
                    pass
    except Exception as e:
        pass

    return pin_data

def export_node_full(node, graph_name):
    """Export comprehensive node information including all pins."""
    node_class = node.get_class().get_name()

    node_data = {
        "class": node_class,
        "name": node.get_name(),
        "graph": graph_name,
        "position": {"x": 0, "y": 0},
        "pins": [],
        "properties": {},
        "special_info": {},
    }

    # Get position
    try:
        if hasattr(node, 'node_pos_x'):
            node_data["position"]["x"] = node.get_editor_property('node_pos_x')
        if hasattr(node, 'node_pos_y'):
            node_data["position"]["y"] = node.get_editor_property('node_pos_y')
    except:
        pass

    # Get GUID
    try:
        if hasattr(node, 'node_guid'):
            node_data["guid"] = str(node.get_editor_property('node_guid'))
    except:
        pass

    # Export all pins
    try:
        if hasattr(node, 'pins'):
            for pin in node.pins:
                node_data["pins"].append(export_pin_info(pin))
    except:
        pass

    # Special handling for specific node types
    if "VariableGet" in node_class or "VariableSet" in node_class:
        try:
            if hasattr(node, 'variable_reference'):
                var_ref = node.get_editor_property('variable_reference')
                if var_ref:
                    node_data["special_info"]["variable_name"] = str(var_ref.member_name) if hasattr(var_ref, 'member_name') else str(var_ref)
        except:
            pass

    if "PropertyAccess" in node_class:
        try:
            # Try multiple ways to get the path
            for prop_name in ['path', 'property_access_path', 'resolved_pin_type']:
                try:
                    val = node.get_editor_property(prop_name)
                    if val:
                        node_data["special_info"][prop_name] = str(val)
                except:
                    pass
        except:
            pass

    if "BlendListByEnum" in node_class:
        try:
            node_data["special_info"]["enum_type"] = ""
            node_data["special_info"]["active_value_binding"] = ""
            # Try to get enum binding info
            for prop_name in ['active_enum_value', 'enum_to_blend', 'bound_enum']:
                try:
                    val = node.get_editor_property(prop_name)
                    if val is not None:
                        node_data["special_info"][prop_name] = str(val)
                except:
                    pass
        except:
            pass

    if "LinkedAnimLayer" in node_class:
        try:
            for prop_name in ['interface', 'layer', 'interface_guid']:
                try:
                    val = node.get_editor_property(prop_name)
                    if val is not None:
                        node_data["special_info"][prop_name] = str(val)
                except:
                    pass
        except:
            pass

    if "StateMachine" in node_class or "StateNode" in node_class:
        try:
            for prop_name in ['state_name', 'bound_graph', 'state_machine_name']:
                try:
                    val = node.get_editor_property(prop_name)
                    if val is not None:
                        node_data["special_info"][prop_name] = str(val)
                except:
                    pass
        except:
            pass

    if "TransitionNode" in node_class:
        try:
            for prop_name in ['logic_type', 'previous_state', 'next_state']:
                try:
                    val = node.get_editor_property(prop_name)
                    if val is not None:
                        node_data["special_info"][prop_name] = str(val)
                except:
                    pass
        except:
            pass

    return node_data

def export_graph_full(graph):
    """Export complete graph information."""
    graph_name = graph.get_name()

    graph_data = {
        "name": graph_name,
        "class": graph.get_class().get_name(),
        "schema": str(graph.schema.get_name()) if graph.schema else "None",
        "node_count": 0,
        "nodes": [],
    }

    # Export all nodes
    try:
        nodes = list(graph.nodes)
        graph_data["node_count"] = len(nodes)

        for node in nodes:
            node_data = export_node_full(node, graph_name)
            graph_data["nodes"].append(node_data)
    except Exception as e:
        log(f"    Error exporting graph {graph_name}: {e}")

    return graph_data

def main():
    log("=" * 80)
    log("COMPLETE ANIMBP EXPORT - ALL GRAPHS AND NODES")
    log("=" * 80)

    # Create output directory
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    # Detect which project we're running on
    # Check if this is bp_only or SLFConversion by looking at the project name
    project_name = "current"  # Default
    try:
        # Try to detect based on loaded classes
        test_class = unreal.find_class("UABP_SoulslikeCharacter_Additive")
        if test_class:
            project_name = "current"  # Has C++ class = SLFConversion
        else:
            project_name = "bp_only"
    except:
        pass

    log(f"Detected project: {project_name}")

    # Load AnimBP
    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log(f"[ERROR] Could not load AnimBP at {ANIMBP_PATH}")
        return

    log(f"Loaded: {bp.get_name()}")

    # Get parent class info
    parent_class = "Unknown"
    try:
        if hasattr(bp, 'parent_class') and bp.parent_class:
            parent_class = bp.parent_class.get_path_name()
    except:
        pass

    export_data = {
        "project": project_name,
        "asset_path": ANIMBP_PATH,
        "asset_name": bp.get_name(),
        "parent_class": parent_class,
        "variables": [],
        "graphs": [],
        "summary": {
            "total_graphs": 0,
            "total_nodes": 0,
            "total_variables": 0,
        }
    }

    # Export Blueprint variables
    log("\n--- Exporting Blueprint Variables ---")
    try:
        if hasattr(unreal, 'SLFAutomationLibrary'):
            vars_list = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
            for var_name in vars_list:
                export_data["variables"].append({
                    "name": var_name,
                    "has_question_mark": "?" in var_name,
                    "has_underscore_suffix": "_0" in var_name or "_1" in var_name,
                })
                log(f"  Variable: {var_name}")
            export_data["summary"]["total_variables"] = len(vars_list)
    except Exception as e:
        log(f"  Error exporting variables: {e}")

    # Export ALL graphs
    log("\n--- Exporting ALL Graphs ---")
    try:
        all_graphs = unreal.BlueprintEditorLibrary.get_graphs(bp)
        log(f"Found {len(all_graphs)} graphs")

        for graph in all_graphs:
            graph_name = graph.get_name()
            log(f"\n  Graph: {graph_name}")

            graph_data = export_graph_full(graph)
            export_data["graphs"].append(graph_data)
            export_data["summary"]["total_nodes"] += graph_data["node_count"]

            log(f"    Nodes: {graph_data['node_count']}")

            # Log some node types for reference
            node_types = {}
            for node in graph_data["nodes"]:
                node_class = node["class"]
                node_types[node_class] = node_types.get(node_class, 0) + 1

            for node_type, count in sorted(node_types.items()):
                if count > 0:
                    log(f"      {node_type}: {count}")

        export_data["summary"]["total_graphs"] = len(all_graphs)

    except Exception as e:
        log(f"  Error exporting graphs: {e}")
        import traceback
        traceback.print_exc()

    # Also use C++ diagnostic functions if available
    if hasattr(unreal, 'SLFAutomationLibrary'):
        log("\n--- Running C++ Diagnostics ---")
        try:
            diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)
            export_data["cpp_diagnosis"] = diagnosis

            # Save diagnosis separately
            diag_file = os.path.join(OUTPUT_DIR, f"diagnosis_{project_name}.txt")
            with open(diag_file, 'w') as f:
                f.write(diagnosis)
            log(f"  Saved diagnosis to: {diag_file}")
        except Exception as e:
            log(f"  Error running C++ diagnosis: {e}")

    # Save full export to JSON
    output_file = os.path.join(OUTPUT_DIR, f"animbp_export_{project_name}.json")
    with open(output_file, 'w') as f:
        json.dump(export_data, f, indent=2, default=str)

    log("\n" + "=" * 80)
    log("EXPORT SUMMARY")
    log("=" * 80)
    log(f"  Project: {project_name}")
    log(f"  Parent Class: {parent_class}")
    log(f"  Total Graphs: {export_data['summary']['total_graphs']}")
    log(f"  Total Nodes: {export_data['summary']['total_nodes']}")
    log(f"  Total Variables: {export_data['summary']['total_variables']}")
    log(f"  Output: {output_file}")
    log("=" * 80)

if __name__ == "__main__":
    main()
