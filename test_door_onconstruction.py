"""
Test that B_Door OnConstruction properly applies the DefaultDoorMesh.
This verifies the fix for editor-time mesh visibility.
"""
import unreal
import json

results = {"B_Door": {}, "B_BossDoor": {}}

def check_door_cdo(bp_path, name):
    """Check CDO mesh state and OnConstruction behavior"""
    data = {"path": bp_path, "cdo_check": {}, "spawn_check": {}}

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        data["error"] = f"Could not load {bp_path}"
        return data

    gen_class = bp.generated_class()
    if not gen_class:
        data["error"] = "No generated class"
        return data

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        data["error"] = "No CDO"
        return data

    # Check TSoftObjectPtr properties
    try:
        door_mesh_ptr = cdo.get_editor_property('default_door_mesh')
        data["cdo_check"]["default_door_mesh"] = str(door_mesh_ptr) if door_mesh_ptr else "None"
    except Exception as e:
        data["cdo_check"]["default_door_mesh_error"] = str(e)

    try:
        frame_mesh_ptr = cdo.get_editor_property('default_door_frame_mesh')
        data["cdo_check"]["default_door_frame_mesh"] = str(frame_mesh_ptr) if frame_mesh_ptr else "None"
    except Exception as e:
        data["cdo_check"]["default_door_frame_mesh_error"] = str(e)

    # Check components on CDO
    data["cdo_check"]["components"] = []
    all_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
    for comp in all_comps:
        comp_info = {"name": comp.get_name()}
        mesh = comp.static_mesh
        comp_info["mesh"] = mesh.get_name() if mesh else "NO_MESH"
        data["cdo_check"]["components"].append(comp_info)

    # Now spawn an actor to trigger OnConstruction
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        location = unreal.Vector(10000, 10000, 0)  # Far away
        rotation = unreal.Rotator(0, 0, 0)

        spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, location, rotation)
        if spawned:
            data["spawn_check"]["spawned"] = True
            data["spawn_check"]["class"] = spawned.get_class().get_name()

            # Check components after spawn (OnConstruction should have run)
            spawn_comps = spawned.get_components_by_class(unreal.StaticMeshComponent)
            data["spawn_check"]["components"] = []
            for comp in spawn_comps:
                comp_info = {"name": comp.get_name()}
                mesh = comp.static_mesh
                comp_info["mesh"] = mesh.get_name() if mesh else "NO_MESH"
                comp_info["visible"] = comp.is_visible()
                data["spawn_check"]["components"].append(comp_info)

                # Look for the door mesh component
                if "Interactable" in comp.get_name() or "Door" in comp.get_name():
                    if mesh:
                        data["spawn_check"]["door_mesh_applied"] = mesh.get_name()
                    else:
                        data["spawn_check"]["door_mesh_applied"] = "NO_MESH"

            # Destroy test actor
            spawned.destroy_actor()
        else:
            data["spawn_check"]["spawned"] = False
            data["spawn_check"]["error"] = "Failed to spawn"
    else:
        data["spawn_check"]["error"] = "No editor world"

    # Determine overall status
    door_mesh_set = "PrisonDoor" in str(data.get("cdo_check", {}).get("default_door_mesh", ""))
    spawn_mesh_ok = data.get("spawn_check", {}).get("door_mesh_applied", "") not in ["", "NO_MESH"]

    if door_mesh_set and spawn_mesh_ok:
        data["status"] = "SUCCESS - OnConstruction applied mesh correctly"
    elif door_mesh_set:
        data["status"] = "PARTIAL - TSoftObjectPtr set but mesh not applied in spawn"
    else:
        data["status"] = "FAILED - TSoftObjectPtr not set"

    return data

# Test B_Door
unreal.log("=" * 60)
unreal.log("TESTING B_Door OnConstruction")
unreal.log("=" * 60)
results["B_Door"] = check_door_cdo("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door", "B_Door")
unreal.log(f"B_Door Status: {results['B_Door'].get('status', 'UNKNOWN')}")

# Test B_BossDoor (should still work)
unreal.log("=" * 60)
unreal.log("TESTING B_BossDoor (verification)")
unreal.log("=" * 60)
results["B_BossDoor"] = check_door_cdo("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor", "B_BossDoor")
unreal.log(f"B_BossDoor Status: {results['B_BossDoor'].get('status', 'UNKNOWN')}")

# Save results
with open("C:/scripts/SLFConversion/migration_cache/door_onconstruction_test.json", "w") as f:
    json.dump(results, f, indent=2)

unreal.log("=" * 60)
unreal.log("SUMMARY")
unreal.log("=" * 60)
unreal.log(f"B_Door: {results['B_Door'].get('status', 'UNKNOWN')}")
unreal.log(f"  Spawn mesh: {results['B_Door'].get('spawn_check', {}).get('door_mesh_applied', 'N/A')}")
unreal.log(f"B_BossDoor: {results['B_BossDoor'].get('status', 'UNKNOWN')}")
unreal.log("Results saved to migration_cache/door_onconstruction_test.json")
