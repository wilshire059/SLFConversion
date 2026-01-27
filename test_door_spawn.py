"""
Test spawning B_Door and verifying mesh is applied in BeginPlay
"""
import unreal
import json

results = {"B_Door": {}, "B_BossDoor": {}}

def test_spawn_door(bp_path, name):
    data = {"path": bp_path, "spawn_success": False, "mesh_after_spawn": None}

    # Get the world
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        data["error"] = "No editor world"
        return data

    # Load Blueprint class
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        data["error"] = f"Could not load {bp_path}"
        return data

    gen_class = bp.generated_class()
    if not gen_class:
        data["error"] = "No generated class"
        return data

    # Spawn at origin
    spawn_params = unreal.ActorSpawnParameters()
    spawn_params.name = unreal.Name(f"Test_{name}")
    location = unreal.Vector(0, 0, 0)
    rotation = unreal.Rotator(0, 0, 0)

    # Spawn the actor
    spawned = world.spawn_actor(gen_class, location, rotation, spawn_params)
    if not spawned:
        data["error"] = "Failed to spawn actor"
        return data

    data["spawn_success"] = True
    data["spawned_class"] = spawned.get_class().get_name()

    # Check components after spawn (BeginPlay should have run)
    all_comps = spawned.get_components_by_class(unreal.StaticMeshComponent)
    data["components"] = []
    for comp in all_comps:
        comp_info = {"name": comp.get_name()}
        mesh = comp.static_mesh
        comp_info["mesh"] = mesh.get_name() if mesh else "NO_MESH"
        comp_info["visible"] = comp.is_visible()
        data["components"].append(comp_info)

        if comp.get_name() == "Interactable SM":
            data["mesh_after_spawn"] = comp_info["mesh"]

    # Destroy the test actor
    spawned.destroy_actor()

    # Determine success
    if data["mesh_after_spawn"] and data["mesh_after_spawn"] != "NO_MESH":
        data["status"] = "SUCCESS - Door mesh is visible"
    else:
        data["status"] = "FAILED - Door mesh missing after spawn"

    return data

# Test both doors
results["B_Door"] = test_spawn_door("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door", "B_Door")
results["B_BossDoor"] = test_spawn_door("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor", "B_BossDoor")

# Log results
unreal.log("=" * 60)
unreal.log("DOOR SPAWN TEST RESULTS")
unreal.log("=" * 60)
unreal.log(f"B_Door: {results['B_Door'].get('status', 'UNKNOWN')}")
unreal.log(f"  Mesh after spawn: {results['B_Door'].get('mesh_after_spawn', 'N/A')}")
unreal.log(f"B_BossDoor: {results['B_BossDoor'].get('status', 'UNKNOWN')}")
unreal.log(f"  Mesh after spawn: {results['B_BossDoor'].get('mesh_after_spawn', 'N/A')}")
unreal.log("=" * 60)

# Save results
with open("C:/scripts/SLFConversion/migration_cache/door_spawn_test.json", "w") as f:
    json.dump(results, f, indent=2)

unreal.log("Results saved to migration_cache/door_spawn_test.json")
