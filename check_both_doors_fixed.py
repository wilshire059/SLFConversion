"""
Check both B_Door and B_BossDoor - after restoring B_Door from git
"""
import unreal
import json

def get_bp_info(bp_path, name):
    """Get comprehensive info about a Blueprint's components"""
    result = {
        "name": name,
        "path": bp_path,
        "scs_nodes": [],
        "cdo_components": []
    }

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return result

    print(f"\n{'='*60}")
    print(f"{name}: {bp_path}")
    print(f"{'='*60}")

    gen_class = bp.generated_class()
    if not gen_class:
        print("ERROR: No generated class")
        return result

    # Get SCS info
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            all_nodes = scs.get_editor_property('all_nodes')
            if all_nodes and len(all_nodes) > 0:
                print(f"\nSCS Nodes ({len(all_nodes)}):")
                for node in all_nodes:
                    try:
                        var_name = str(node.get_editor_property('internal_variable_name'))
                        comp_class = node.get_editor_property('component_class')
                        class_name = comp_class.get_name() if comp_class else "Unknown"
                        result["scs_nodes"].append({"name": var_name, "class": class_name})
                        print(f"  - {var_name}: {class_name}")
                    except:
                        pass
            else:
                print("\n*** SCS HAS NO NODES (uses C++ components!) ***")
        else:
            print("\n*** SCS is None (uses C++ components!) ***")
    except Exception as e:
        print(f"\nCould not get SCS: {e}")

    # Get CDO components
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
                print(f"    - {comp.get_name()}: StaticMeshComponent (mesh={comp_info['mesh']}, visible={comp.is_visible()})")
            else:
                print(f"    - {comp.get_name()}: {comp.get_class().get_name()} (visible={comp.is_visible()})")

            result["cdo_components"].append(comp_info)

    return result

# Check both doors
b_door_info = get_bp_info("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door", "B_Door")
b_boss_door_info = get_bp_info("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor", "B_BossDoor")

# Save info
output = {
    "B_Door": b_door_info,
    "B_BossDoor": b_boss_door_info
}

output_path = "C:/scripts/SLFConversion/migration_cache/both_doors_current.json"
with open(output_path, 'w') as f:
    json.dump(output, f, indent=2)
print(f"\n\n{'='*60}")
print(f"Saved to {output_path}")

# Summary
print(f"\n{'='*60}")
print("SUMMARY")
print(f"{'='*60}")
print(f"B_Door SCS nodes: {len(b_door_info['scs_nodes'])}")
print(f"B_Door CDO components: {len(b_door_info['cdo_components'])}")
print(f"B_BossDoor SCS nodes: {len(b_boss_door_info['scs_nodes'])}")
print(f"B_BossDoor CDO components: {len(b_boss_door_info['cdo_components'])}")
