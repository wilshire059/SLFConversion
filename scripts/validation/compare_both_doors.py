"""
Compare B_Door and B_BossDoor SCS and CDO components to understand
why B_BossDoor works but B_Door doesn't.
"""
import unreal
import json

def get_bp_info(bp_path, output_key):
    """Get comprehensive info about a Blueprint's components"""
    result = {
        "path": bp_path,
        "scs_nodes": [],
        "cdo_components": [],
        "parent_class": None
    }

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return result

    print(f"\n{'='*60}")
    print(f"BLUEPRINT: {bp_path}")
    print(f"{'='*60}")

    # Get parent class
    gen_class = bp.generated_class()
    if gen_class:
        parent = gen_class.get_super_class()
        if parent:
            result["parent_class"] = parent.get_name()
            print(f"Parent class: {parent.get_name()}")

    # Get SCS info
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            all_nodes = scs.get_editor_property('all_nodes')
            if all_nodes:
                print(f"\nSCS Nodes ({len(all_nodes)}):")
                for node in all_nodes:
                    try:
                        var_name = str(node.get_editor_property('internal_variable_name'))
                        comp_class = node.get_editor_property('component_class')
                        class_name = comp_class.get_name() if comp_class else "Unknown"

                        # Check if inherited
                        is_inherited = False
                        try:
                            is_inherited = node.get_editor_property('is_inherited_variable')
                        except:
                            pass

                        node_info = {
                            "name": var_name,
                            "class": class_name,
                            "inherited": is_inherited
                        }
                        result["scs_nodes"].append(node_info)

                        inherited_str = " (INHERITED)" if is_inherited else ""
                        print(f"  - {var_name}: {class_name}{inherited_str}")
                    except Exception as e:
                        print(f"  - (error reading node: {e})")
            else:
                print("\nSCS has no nodes")
        else:
            print("\nSCS is None")
    except Exception as e:
        print(f"\nCould not get SCS: {e}")

    # Get CDO components
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            print(f"\nCDO Components:")

            # Get all scene components
            all_comps = cdo.get_components_by_class(unreal.SceneComponent)
            print(f"  SceneComponents ({len(all_comps)}):")
            for comp in all_comps:
                comp_info = {
                    "name": comp.get_name(),
                    "class": comp.get_class().get_name(),
                    "visible": comp.is_visible()
                }

                # Check if StaticMeshComponent with mesh
                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    comp_info["mesh"] = mesh.get_name() if mesh else "NO MESH"
                    print(f"    - {comp.get_name()}: {comp.get_class().get_name()} (mesh={comp_info['mesh']}, visible={comp.is_visible()})")
                else:
                    print(f"    - {comp.get_name()}: {comp.get_class().get_name()} (visible={comp.is_visible()})")

                result["cdo_components"].append(comp_info)

    return result

# Compare both doors
b_door_info = get_bp_info("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door", "B_Door")
b_boss_door_info = get_bp_info("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor", "B_BossDoor")

# Save comparison
output = {
    "B_Door": b_door_info,
    "B_BossDoor": b_boss_door_info
}

output_path = "C:/scripts/SLFConversion/migration_cache/door_comparison.json"
with open(output_path, 'w') as f:
    json.dump(output, f, indent=2)
print(f"\n\nSaved comparison to {output_path}")

# Print key differences
print(f"\n{'='*60}")
print("KEY DIFFERENCES")
print(f"{'='*60}")

door_scs_names = {n["name"] for n in b_door_info["scs_nodes"]}
boss_scs_names = {n["name"] for n in b_boss_door_info["scs_nodes"]}

print(f"\nIn B_Door SCS but not B_BossDoor: {door_scs_names - boss_scs_names}")
print(f"In B_BossDoor SCS but not B_Door: {boss_scs_names - door_scs_names}")

door_cdo_names = {c["name"] for c in b_door_info["cdo_components"]}
boss_cdo_names = {c["name"] for c in b_boss_door_info["cdo_components"]}

print(f"\nIn B_Door CDO but not B_BossDoor: {door_cdo_names - boss_cdo_names}")
print(f"In B_BossDoor CDO but not B_Door: {boss_cdo_names - door_cdo_names}")
