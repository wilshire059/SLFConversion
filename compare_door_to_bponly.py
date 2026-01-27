"""
Compare B_Door between SLFConversion and bp_only to find mesh/component differences.
Run this on SLFConversion project.
"""
import unreal
import json
import os

results = {
    "slfconversion": {},
    "comparison_notes": []
}

def extract_door_data(bp_path, label):
    """Extract comprehensive door data"""
    data = {
        "path": bp_path,
        "exists": False,
        "parent_class": None,
        "components": [],
        "properties": {}
    }

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        data["error"] = f"Could not load {bp_path}"
        return data

    data["exists"] = True

    gen_class = bp.generated_class()
    if not gen_class:
        data["error"] = "No generated class"
        return data

    # Get parent class
    parent = gen_class.get_super_class()
    if parent:
        data["parent_class"] = parent.get_name()

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        data["error"] = "No CDO"
        return data

    # Get all components
    all_comps = cdo.get_components_by_class(unreal.ActorComponent)
    for comp in all_comps:
        comp_data = {
            "name": comp.get_name(),
            "class": comp.get_class().get_name(),
        }

        # Check for mesh
        if isinstance(comp, unreal.StaticMeshComponent):
            mesh = comp.static_mesh
            comp_data["static_mesh"] = mesh.get_path_name() if mesh else "None"
            comp_data["visible"] = comp.is_visible()
            comp_data["relative_location"] = str(comp.get_relative_location())
            comp_data["relative_rotation"] = str(comp.get_relative_rotation())
        elif isinstance(comp, unreal.SkeletalMeshComponent):
            mesh = comp.skeletal_mesh
            comp_data["skeletal_mesh"] = mesh.get_path_name() if mesh else "None"

        data["components"].append(comp_data)

    # Check specific properties
    props_to_check = [
        'default_door_mesh',
        'default_door_frame_mesh',
        'move_to_distance',
        'yaw_rotation_amount',
        'play_rate'
    ]

    for prop in props_to_check:
        try:
            val = cdo.get_editor_property(prop)
            if val is not None:
                data["properties"][prop] = str(val)
            else:
                data["properties"][prop] = "None"
        except Exception as e:
            data["properties"][prop] = f"Error: {e}"

    return data

# Extract from SLFConversion
unreal.log("=" * 60)
unreal.log("Extracting B_Door data from SLFConversion")
unreal.log("=" * 60)

results["slfconversion"] = extract_door_data(
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door",
    "SLFConversion"
)

unreal.log(f"Parent class: {results['slfconversion'].get('parent_class', 'Unknown')}")
unreal.log(f"Components: {len(results['slfconversion'].get('components', []))}")

for comp in results["slfconversion"].get("components", []):
    mesh_info = comp.get("static_mesh", comp.get("skeletal_mesh", "N/A"))
    unreal.log(f"  - {comp['name']} ({comp['class']}): {mesh_info}")

# Save results
output_path = "C:/scripts/SLFConversion/migration_cache/door_slfconversion_state.json"
with open(output_path, "w") as f:
    json.dump(results, f, indent=2)

unreal.log(f"Saved to {output_path}")
unreal.log("=" * 60)
