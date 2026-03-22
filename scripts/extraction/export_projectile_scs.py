"""
Export SCS components from B_BaseProjectile and B_Projectile_ThrowingKnife
Run this in both bp_only and SLFConversion to compare
"""

import unreal
import json
import os
import sys

def get_scs_components(blueprint):
    """Extract SCS components from a Blueprint"""
    components = []

    if not blueprint:
        return components

    # Get the SimpleConstructionScript
    try:
        scs = blueprint.get_editor_property('simple_construction_script')
    except Exception as e:
        print(f"  Could not get simple_construction_script: {e}")
        return components

    if not scs:
        print(f"  No SCS found (is None)")
        return components

    # Get all nodes
    try:
        all_nodes = scs.get_all_nodes()
        print(f"  Found {len(all_nodes)} SCS nodes")
    except Exception as e:
        print(f"  Could not get SCS nodes: {e}")
        return components

    for node in all_nodes:
        if node:
            try:
                comp_template = node.component_template
            except:
                continue

            if comp_template:
                comp_info = {
                    'name': comp_template.get_name(),
                    'class': comp_template.get_class().get_name(),
                    'class_path': str(comp_template.get_class().get_path_name()),
                }

                # Get additional properties based on component type
                class_name = comp_template.get_class().get_name()

                if 'ProjectileMovementComponent' in class_name:
                    try:
                        comp_info['initial_speed'] = comp_template.get_editor_property('initial_speed')
                    except:
                        pass
                    try:
                        comp_info['max_speed'] = comp_template.get_editor_property('max_speed')
                    except:
                        pass
                    try:
                        comp_info['projectile_gravity_scale'] = comp_template.get_editor_property('projectile_gravity_scale')
                    except:
                        pass
                    try:
                        comp_info['rotation_follows_velocity'] = comp_template.get_editor_property('rotation_follows_velocity')
                    except:
                        pass

                if 'SphereComponent' in class_name:
                    try:
                        comp_info['sphere_radius'] = comp_template.get_editor_property('sphere_radius')
                    except:
                        pass

                if 'NiagaraComponent' in class_name:
                    try:
                        asset = comp_template.get_editor_property('asset')
                        comp_info['niagara_asset'] = str(asset.get_path_name()) if asset else None
                    except:
                        pass
                    try:
                        comp_info['auto_activate'] = comp_template.get_editor_property('auto_activate')
                    except:
                        pass

                if 'StaticMeshComponent' in class_name:
                    try:
                        mesh = comp_template.get_editor_property('static_mesh')
                        comp_info['static_mesh'] = str(mesh.get_path_name()) if mesh else None
                    except:
                        pass

                components.append(comp_info)

    return components

def export_blueprint_info(bp_path, output_name):
    """Export Blueprint info including parent class and SCS components"""
    result = {
        'path': bp_path,
        'parent_class': None,
        'parent_class_path': None,
        'generated_class': None,
        'scs_components': []
    }

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return result

    print(f"\nBlueprint: {bp_path}")
    print(f"  Type: {type(bp).__name__}")

    # Get parent class
    try:
        parent = bp.get_editor_property('parent_class')
        if parent:
            result['parent_class'] = parent.get_name()
            result['parent_class_path'] = str(parent.get_path_name())
            print(f"  Parent class: {result['parent_class']}")
            print(f"  Parent path: {result['parent_class_path']}")
        else:
            print(f"  Parent class: None")
    except Exception as e:
        print(f"  Error getting parent_class: {e}")

    # Get generated class name
    try:
        gen_class = bp.generated_class()
        if gen_class:
            result['generated_class'] = gen_class.get_name()
            print(f"  Generated class: {result['generated_class']}")
    except Exception as e:
        print(f"  Error getting generated_class: {e}")

    # Get SCS components
    result['scs_components'] = get_scs_components(bp)
    print(f"  SCS Components ({len(result['scs_components'])}):")
    for comp in result['scs_components']:
        print(f"    - {comp['name']}: {comp['class']}")
        for key, value in comp.items():
            if key not in ['name', 'class', 'class_path']:
                print(f"        {key}: {value}")

    if len(result['scs_components']) == 0:
        print("  WARNING: No SCS components found!")

    return result

def main():
    # Determine project name from project path
    project_path = unreal.Paths.get_project_file_path()
    print(f"Project path: {project_path}")

    if 'bp_only' in project_path.lower():
        project_name = 'bp_only'
    elif 'slfconversion' in project_path.lower():
        project_name = 'slfconversion'
    else:
        project_name = 'unknown'

    print(f"Detected project: {project_name}")

    # Blueprints to check
    blueprints = [
        '/Game/SoulslikeFramework/Blueprints/_WorldActors/Projectiles/B_BaseProjectile',
        '/Game/SoulslikeFramework/Blueprints/_WorldActors/Projectiles/B_Projectile_ThrowingKnife',
    ]

    results = {}

    for bp_path in blueprints:
        bp_name = bp_path.split('/')[-1]
        results[bp_name] = export_blueprint_info(bp_path, bp_name)

    # Save to file
    output_path = f'C:/scripts/slfconversion/migration_cache/projectile_scs_{project_name}.json'
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2, default=str)

    print(f"\n========================================")
    print(f"Saved to: {output_path}")
    print(f"========================================")

if __name__ == '__main__':
    main()
