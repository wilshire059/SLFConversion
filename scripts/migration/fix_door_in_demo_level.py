"""
Fix B_Door instances in the Demo level.
Level instances have serialized null mesh values that override CDO.
We need to load the level, fix the instances, and save.
"""
import unreal

door_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoor.SM_PrisonDoor"
frame_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoorArch.SM_PrisonDoorArch"
demo_level_path = "/Game/SoulslikeFramework/Demo/_Levels/L_SoulslikeDemo"

unreal.log("=" * 70)
unreal.log("FIXING B_Door INSTANCES IN DEMO LEVEL")
unreal.log("=" * 70)

# Load meshes
door_mesh = unreal.EditorAssetLibrary.load_asset(door_mesh_path)
frame_mesh = unreal.EditorAssetLibrary.load_asset(frame_mesh_path)

if door_mesh:
    unreal.log(f"Loaded door mesh: {door_mesh.get_name()}")
else:
    unreal.log_error(f"Failed to load door mesh: {door_mesh_path}")

if frame_mesh:
    unreal.log(f"Loaded frame mesh: {frame_mesh.get_name()}")
else:
    unreal.log_error(f"Failed to load frame mesh: {frame_mesh_path}")

# Check if level exists
level_exists = unreal.EditorAssetLibrary.does_asset_exist(demo_level_path)
unreal.log(f"\nDemo level exists: {level_exists}")

if not level_exists:
    # Try to find the level
    unreal.log("\nSearching for level assets...")
    all_assets = unreal.EditorAssetLibrary.list_assets("/Game/SoulslikeFramework/", recursive=True)
    for asset in all_assets:
        if "L_Soulslike" in asset or "Demo" in asset and "Level" in asset:
            unreal.log(f"  Found: {asset}")

# Load the level into the editor
unreal.log(f"\n[1] Loading level: {demo_level_path}")
try:
    success = unreal.EditorLevelLibrary.load_level(demo_level_path)
    unreal.log(f"Load level result: {success}")
except Exception as e:
    unreal.log_error(f"Failed to load level: {e}")

# Now get all actors in the loaded level
unreal.log("\n[2] Searching for B_Door actors...")
all_actors = unreal.EditorLevelLibrary.get_all_level_actors()
unreal.log(f"Total actors in level: {len(all_actors)}")

# Find door actors (but not boss doors)
door_actors = []
for actor in all_actors:
    class_name = actor.get_class().get_name()
    if "B_Door" in class_name and "Boss" not in class_name:
        door_actors.append(actor)
    # Also check parent class
    elif "Door" in class_name:
        unreal.log(f"  Found door-like: {actor.get_name()} ({class_name})")

unreal.log(f"\nB_Door instances found: {len(door_actors)}")

# Fix each door
doors_fixed = 0
for actor in door_actors:
    unreal.log(f"\n  Actor: {actor.get_name()}")
    unreal.log(f"  Class: {actor.get_class().get_name()}")
    unreal.log(f"  Location: {actor.get_actor_location()}")

    # Get mesh components
    mesh_comps = actor.get_components_by_class(unreal.StaticMeshComponent)

    for comp in mesh_comps:
        comp_name = comp.get_name()
        current_mesh = comp.static_mesh
        mesh_name = current_mesh.get_name() if current_mesh else "NO_MESH"

        unreal.log(f"    [{comp_name}] Current: {mesh_name}")

        # Fix if needed
        if not current_mesh or mesh_name == "NO_MESH":
            if "Interactable" in comp_name and door_mesh:
                comp.set_static_mesh(door_mesh)
                unreal.log(f"      -> FIXED: Set to {door_mesh.get_name()}")
                doors_fixed += 1
            elif "Frame" in comp_name and frame_mesh:
                comp.set_static_mesh(frame_mesh)
                unreal.log(f"      -> FIXED: Set to {frame_mesh.get_name()}")
                doors_fixed += 1

# Also search for any interactable actors that might be doors
unreal.log("\n[3] Checking for interactables that might be doors...")
for actor in all_actors:
    class_name = actor.get_class().get_name()
    if "Interactable" in class_name:
        # Check if it has door meshes or prison in name
        actor_name = actor.get_name().lower()
        if "door" in actor_name or "prison" in actor_name:
            unreal.log(f"  Potential door: {actor.get_name()} ({class_name})")

# Summary
unreal.log("\n" + "=" * 70)
unreal.log("SUMMARY")
unreal.log("=" * 70)
unreal.log(f"B_Door actors found: {len(door_actors)}")
unreal.log(f"Mesh components fixed: {doors_fixed}")

if doors_fixed > 0:
    # Mark level dirty and save
    unreal.log("\n[4] Saving level...")
    try:
        # Save current level
        unreal.EditorLevelLibrary.save_current_level()
        unreal.log("Level saved successfully!")
    except Exception as e:
        unreal.log_error(f"Failed to save level: {e}")
        unreal.log("Please save the level manually in the editor.")
else:
    if len(door_actors) == 0:
        unreal.log("\nNo B_Door actors found in this level.")
        unreal.log("The door may be in a different level or sublevel.")
