"""
Find and fix B_Door instances in the level that have null meshes.
Also load the demo level to check doors there.
"""
import unreal
import json

door_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoor.SM_PrisonDoor"
frame_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoorArch.SM_PrisonDoorArch"

results = {"levels_checked": [], "doors_found": [], "doors_fixed": []}

# Load meshes
door_mesh = unreal.EditorAssetLibrary.load_asset(door_mesh_path)
frame_mesh = unreal.EditorAssetLibrary.load_asset(frame_mesh_path)

unreal.log("=" * 70)
unreal.log("FINDING AND FIXING LEVEL B_Door INSTANCES")
unreal.log("=" * 70)

# First check current level
unreal.log("\n[1] Checking current editor level...")
all_actors = unreal.EditorLevelLibrary.get_all_level_actors()
unreal.log(f"Total actors in level: {len(all_actors)}")

# Filter for B_Door (but not B_BossDoor)
door_actors = []
for actor in all_actors:
    class_name = actor.get_class().get_name()
    if "B_Door" in class_name and "Boss" not in class_name:
        door_actors.append(actor)

unreal.log(f"B_Door instances found: {len(door_actors)}")

for actor in door_actors:
    actor_info = {
        "name": actor.get_name(),
        "class": actor.get_class().get_name(),
        "location": str(actor.get_actor_location()),
        "fixed": False
    }

    unreal.log(f"\n  Actor: {actor.get_name()}")
    unreal.log(f"  Location: {actor.get_actor_location()}")

    # Check and fix mesh components
    mesh_comps = actor.get_components_by_class(unreal.StaticMeshComponent)
    for comp in mesh_comps:
        comp_name = comp.get_name()
        current_mesh = comp.static_mesh
        mesh_name = current_mesh.get_name() if current_mesh else "NO_MESH"

        unreal.log(f"    [{comp_name}] Current: {mesh_name}")

        # Fix if mesh is missing
        if not current_mesh:
            if "Interactable" in comp_name and door_mesh:
                comp.set_static_mesh(door_mesh)
                unreal.log(f"    -> FIXED: Set to {door_mesh.get_name()}")
                actor_info["fixed"] = True
            elif ("Frame" in comp_name or "Door Frame" in comp_name) and frame_mesh:
                comp.set_static_mesh(frame_mesh)
                unreal.log(f"    -> FIXED: Set to {frame_mesh.get_name()}")
                actor_info["fixed"] = True

    results["doors_found"].append(actor_info)
    if actor_info["fixed"]:
        results["doors_fixed"].append(actor_info["name"])

# Try to load the demo level and check
unreal.log("\n\n[2] Checking Demo level path...")
demo_level_path = "/Game/SoulslikeFramework/Demo/_Levels/L_SoulslikeDemo"

# Check if the level exists
if unreal.EditorAssetLibrary.does_asset_exist(demo_level_path):
    unreal.log(f"Demo level exists: {demo_level_path}")
    results["levels_checked"].append({"path": demo_level_path, "exists": True})
else:
    unreal.log(f"Demo level NOT found at: {demo_level_path}")
    # Try alternate paths
    alt_paths = [
        "/Game/SoulslikeFramework/Demo/L_SoulslikeDemo",
        "/Game/SoulslikeFramework/_Levels/L_SoulslikeDemo",
        "/Game/Maps/L_SoulslikeDemo"
    ]
    for alt_path in alt_paths:
        if unreal.EditorAssetLibrary.does_asset_exist(alt_path):
            unreal.log(f"Found at alternate path: {alt_path}")
            results["levels_checked"].append({"path": alt_path, "exists": True})
            break

# List all level assets
unreal.log("\n\n[3] All level assets in project:")
level_assets = unreal.EditorAssetLibrary.list_assets("/Game/", recursive=True)
level_files = [a for a in level_assets if a.endswith("_C") == False and ("L_" in a or "Level" in a or "_Levels" in a)]
for lf in level_files[:20]:
    if ".umap" in str(lf) or "World" in str(lf):
        unreal.log(f"  {lf}")

# Summary
unreal.log("\n\n" + "=" * 70)
unreal.log("SUMMARY")
unreal.log("=" * 70)
unreal.log(f"Doors found: {len(results['doors_found'])}")
unreal.log(f"Doors fixed: {len(results['doors_fixed'])}")

if len(results['doors_fixed']) > 0:
    unreal.log("\nFixed actors:")
    for name in results['doors_fixed']:
        unreal.log(f"  - {name}")
    unreal.log("\nYou should save the level to persist these fixes.")

# Save results
output_path = "C:/scripts/SLFConversion/migration_cache/level_door_fix_results.json"
with open(output_path, "w") as f:
    json.dump(results, f, indent=2)

unreal.log(f"\nSaved to {output_path}")
