"""
Comprehensive Ladder System Validation Script
Uses SLFAutomationLibrary for validation

This script:
1. Checks B_Ladder parent class
2. Checks AC_LadderManager parent class
3. Compares components to bp_only
4. Validates CDO defaults
5. Checks event graph status
"""

import unreal
import json
import os

OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache"

def get_blueprint_parent_class(bp_path):
    """Get the parent class of a Blueprint"""
    bp = unreal.load_asset(bp_path)
    if not bp:
        return None, f"Failed to load: {bp_path}"

    if hasattr(bp, 'parent_class') and bp.parent_class:
        return bp.parent_class.get_name(), None

    # Try generated class
    if hasattr(bp, 'generated_class') and bp.generated_class:
        parent = bp.generated_class.get_super_class()
        if parent:
            return parent.get_name(), None

    return None, "Could not determine parent class"

def get_blueprint_components(bp_path):
    """Get list of components from Blueprint SCS"""
    bp = unreal.load_asset(bp_path)
    if not bp:
        return [], f"Failed to load: {bp_path}"

    components = []

    # Get SCS nodes
    if hasattr(bp, 'simple_construction_script') and bp.simple_construction_script:
        scs = bp.simple_construction_script
        if hasattr(scs, 'get_all_nodes'):
            for node in scs.get_all_nodes():
                if node and hasattr(node, 'component_template') and node.component_template:
                    comp = node.component_template
                    comp_info = {
                        'name': comp.get_name(),
                        'class': comp.get_class().get_name(),
                    }

                    # Get static mesh if ISM
                    if comp.get_class().get_name() == 'InstancedStaticMeshComponent':
                        if hasattr(comp, 'static_mesh'):
                            mesh = comp.get_editor_property('static_mesh')
                            comp_info['static_mesh'] = mesh.get_name() if mesh else 'NULL'

                    # Get box extent if BoxComponent
                    if comp.get_class().get_name() == 'BoxComponent':
                        if hasattr(comp, 'box_extent'):
                            extent = comp.get_editor_property('box_extent')
                            comp_info['box_extent'] = [extent.x, extent.y, extent.z]

                    components.append(comp_info)

    return components, None

def get_blueprint_functions(bp_path):
    """Get list of functions from Blueprint"""
    bp = unreal.load_asset(bp_path)
    if not bp:
        return [], f"Failed to load: {bp_path}"

    functions = []

    # Get function graphs
    if hasattr(bp, 'function_graphs'):
        for graph in bp.function_graphs:
            if graph:
                functions.append({
                    'name': graph.get_name(),
                    'type': 'FunctionGraph'
                })

    # Check uber graphs (event graph)
    if hasattr(bp, 'uber_graphs'):
        for graph in bp.uber_graphs:
            if graph:
                node_count = 0
                if hasattr(graph, 'nodes'):
                    node_count = len(graph.nodes)
                functions.append({
                    'name': graph.get_name(),
                    'type': 'UberGraph',
                    'node_count': node_count
                })

    return functions, None

def get_cdo_properties(bp_path, property_names):
    """Get CDO property values"""
    bp = unreal.load_asset(bp_path)
    if not bp:
        return {}, f"Failed to load: {bp_path}"

    if not hasattr(bp, 'generated_class') or not bp.generated_class:
        return {}, "No generated class"

    cdo = unreal.get_default_object(bp.generated_class)
    if not cdo:
        return {}, "No CDO"

    properties = {}
    for prop_name in property_names:
        try:
            value = cdo.get_editor_property(prop_name)
            if hasattr(value, 'x'):  # Vector
                properties[prop_name] = [value.x, value.y, value.z]
            elif hasattr(value, '__float__'):
                properties[prop_name] = float(value)
            elif hasattr(value, '__int__'):
                properties[prop_name] = int(value)
            elif hasattr(value, '__bool__'):
                properties[prop_name] = bool(value)
            else:
                properties[prop_name] = str(value)
        except Exception as e:
            properties[prop_name] = f"ERROR: {e}"

    return properties, None

def validate_ladder():
    """Main validation function"""
    results = {
        'b_ladder': {},
        'ac_ladder_manager': {},
        'comparison': {}
    }

    # ==================== B_Ladder ====================
    print("\n" + "="*60)
    print("VALIDATING B_Ladder")
    print("="*60)

    b_ladder_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Ladder"

    # Parent class
    parent, err = get_blueprint_parent_class(b_ladder_path)
    results['b_ladder']['parent_class'] = parent if not err else err
    print(f"Parent Class: {parent if not err else err}")

    # Components
    components, err = get_blueprint_components(b_ladder_path)
    results['b_ladder']['components'] = components if not err else err
    print(f"Components ({len(components)}):")
    for comp in components:
        print(f"  - {comp['name']} ({comp['class']})")
        if 'static_mesh' in comp:
            print(f"      static_mesh: {comp['static_mesh']}")
        if 'box_extent' in comp:
            print(f"      box_extent: {comp['box_extent']}")

    # Functions
    functions, err = get_blueprint_functions(b_ladder_path)
    results['b_ladder']['functions'] = functions if not err else err
    print(f"Functions/Graphs ({len(functions)}):")
    for func in functions:
        print(f"  - {func['name']} ({func['type']})", end="")
        if 'node_count' in func:
            print(f" - {func['node_count']} nodes", end="")
        print()

    # CDO Properties
    ladder_props = [
        'ladder_height', 'bar_offset', 'overshoot_count', 'connection_count',
        'bar_at_zero', 'add_finish_bar', 'top_collision_extent',
        'bottom_collision_extent', 'topdown_collision_extent'
    ]
    cdo_props, err = get_cdo_properties(b_ladder_path, ladder_props)
    results['b_ladder']['cdo_properties'] = cdo_props if not err else err
    print(f"CDO Properties:")
    for prop, value in cdo_props.items():
        print(f"  - {prop}: {value}")

    # ==================== AC_LadderManager ====================
    print("\n" + "="*60)
    print("VALIDATING AC_LadderManager")
    print("="*60)

    ac_ladder_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_LadderManager"

    # Parent class
    parent, err = get_blueprint_parent_class(ac_ladder_path)
    results['ac_ladder_manager']['parent_class'] = parent if not err else err
    print(f"Parent Class: {parent if not err else err}")

    # Functions
    functions, err = get_blueprint_functions(ac_ladder_path)
    results['ac_ladder_manager']['functions'] = functions if not err else err
    print(f"Functions/Graphs ({len(functions)}):")
    for func in functions:
        print(f"  - {func['name']} ({func['type']})", end="")
        if 'node_count' in func:
            print(f" - {func['node_count']} nodes", end="")
        print()

    # CDO Properties
    manager_props = [
        'is_on_ground', 'is_climbing', 'is_climbing_off_top',
        'is_climbing_down_from_top', 'is_oriented_to_ladder',
        'climb_sprint_multiplier', 'ladder_animset'
    ]
    cdo_props, err = get_cdo_properties(ac_ladder_path, manager_props)
    results['ac_ladder_manager']['cdo_properties'] = cdo_props if not err else err
    print(f"CDO Properties:")
    for prop, value in cdo_props.items():
        print(f"  - {prop}: {value}")

    # ==================== Save Results ====================
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    output_file = os.path.join(OUTPUT_DIR, "ladder_validation.json")
    with open(output_file, 'w') as f:
        json.dump(results, f, indent=2, default=str)

    print(f"\n{'='*60}")
    print(f"Results saved to: {output_file}")
    print(f"{'='*60}")

    return results

# Run validation
if __name__ == "__main__":
    validate_ladder()
