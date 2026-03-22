"""
Verify both B_Door and B_BossDoor states after fix
"""
import unreal
import json

results = {}

def check_door(bp_path, name):
    data = {"path": bp_path, "status": "unknown", "cdo_components": []}

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        data["status"] = "ERROR: Could not load"
        return data

    gen_class = bp.generated_class()
    if not gen_class:
        data["status"] = "ERROR: No generated class"
        return data

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        data["status"] = "ERROR: No CDO"
        return data

    all_comps = cdo.get_components_by_class(unreal.SceneComponent)
    for comp in all_comps:
        comp_info = {
            "name": comp.get_name(),
            "class": comp.get_class().get_name(),
            "visible": comp.is_visible() if hasattr(comp, 'is_visible') else None
        }
        if isinstance(comp, unreal.StaticMeshComponent):
            mesh = comp.static_mesh
            comp_info["mesh"] = mesh.get_name() if mesh else "NO_MESH"
        data["cdo_components"].append(comp_info)

    # Check for key components
    has_door_mesh = any(c["name"] == "Interactable SM" and c.get("mesh") and c.get("mesh") != "NO_MESH" for c in data["cdo_components"])
    has_door_frame = any(c["name"] == "Door Frame" and c.get("mesh") and c.get("mesh") != "NO_MESH" for c in data["cdo_components"])

    if has_door_mesh and has_door_frame:
        data["status"] = "OK - Has door mesh and frame"
    elif has_door_frame and not has_door_mesh:
        data["status"] = "PARTIAL - Has frame but no door mesh"
    else:
        data["status"] = "BROKEN - Missing critical components"

    data["component_count"] = len(all_comps)

    return data

# Check both doors
results["B_Door"] = check_door("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door", "B_Door")
results["B_BossDoor"] = check_door("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor", "B_BossDoor")

# Log results
unreal.log("=" * 60)
unreal.log("DOOR VERIFICATION RESULTS")
unreal.log("=" * 60)
unreal.log(f"B_Door: {results['B_Door']['status']}")
unreal.log(f"  Components: {results['B_Door']['component_count']}")
for comp in results["B_Door"]["cdo_components"]:
    unreal.log(f"    - {comp['name']}: {comp['class']} (mesh={comp.get('mesh', 'N/A')}, visible={comp.get('visible', 'N/A')})")

unreal.log("")
unreal.log(f"B_BossDoor: {results['B_BossDoor']['status']}")
unreal.log(f"  Components: {results['B_BossDoor']['component_count']}")
for comp in results["B_BossDoor"]["cdo_components"]:
    unreal.log(f"    - {comp['name']}: {comp['class']} (mesh={comp.get('mesh', 'N/A')}, visible={comp.get('visible', 'N/A')})")

unreal.log("=" * 60)

# Save to file
with open("C:/scripts/SLFConversion/migration_cache/door_verification.json", "w") as f:
    json.dump(results, f, indent=2)

unreal.log("Results saved to migration_cache/door_verification.json")
