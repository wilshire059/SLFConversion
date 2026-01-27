"""
Fix B_Door_Prison instances in L_Demo level.
1. Check B_Door_Prison Blueprint (child of B_Door)
2. Load the actual level at /Game/SoulslikeFramework/Maps/L_Demo
3. Fix any door instances with null meshes
"""
import unreal
import json

results = {
    "b_door_prison": {},
    "level_doors": [],
    "fixed_count": 0
}

door_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoor.SM_PrisonDoor"
frame_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoorArch.SM_PrisonDoorArch"
level_path = "/Game/SoulslikeFramework/Maps/L_Demo"

unreal.log("=" * 70)
unreal.log("FIXING B_Door_Prison IN L_Demo")
unreal.log("=" * 70)

# Load meshes
door_mesh = unreal.EditorAssetLibrary.load_asset(door_mesh_path)
frame_mesh = unreal.EditorAssetLibrary.load_asset(frame_mesh_path)
unreal.log(f"Door mesh loaded: {door_mesh is not None}")
unreal.log(f"Frame mesh loaded: {frame_mesh is not None}")

# [1] Check B_Door_Prison Blueprint
unreal.log("\n[1] CHECKING B_Door_Prison BLUEPRINT...")

# First find B_Door_Prison path
prison_door_paths = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door_Prison",
    "/Game/SoulslikeFramework/Demo/Blueprints/B_Door_Prison",
    "/Game/SoulslikeFramework/Maps/B_Door_Prison",
]

prison_bp = None
for path in prison_door_paths:
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        prison_bp = unreal.EditorAssetLibrary.load_asset(path)
        unreal.log(f"Found B_Door_Prison at: {path}")
        results["b_door_prison"]["path"] = path
        break

# Also search for it
if not prison_bp:
    unreal.log("Searching for B_Door_Prison...")
    all_assets = unreal.EditorAssetLibrary.list_assets("/Game/", recursive=True)
    for asset in all_assets:
        if "B_Door_Prison" in str(asset) and "_C" not in str(asset):
            unreal.log(f"  Found: {asset}")
            prison_bp = unreal.EditorAssetLibrary.load_asset(asset)
            results["b_door_prison"]["path"] = str(asset)
            break

if prison_bp:
    gen_class = prison_bp.generated_class()
    if gen_class:
        unreal.log(f"B_Door_Prison class: {gen_class.get_name()}")
        results["b_door_prison"]["class_name"] = gen_class.get_name()

        cdo = unreal.get_default_object(gen_class)
        if cdo:
            results["b_door_prison"]["cdo_components"] = []
            mesh_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
            for comp in mesh_comps:
                mesh = comp.static_mesh
                visible = comp.is_visible()
                comp_info = {
                    "name": comp.get_name(),
                    "mesh": mesh.get_name() if mesh else "NONE",
                    "visible": visible
                }
                results["b_door_prison"]["cdo_components"].append(comp_info)
                unreal.log(f"  [{comp.get_name()}] Mesh: {comp_info['mesh']}, Visible: {visible}")

                # Fix CDO mesh if needed
                if not mesh:
                    if "Interactable" in comp.get_name() and door_mesh:
                        comp.set_static_mesh(door_mesh)
                        unreal.log(f"    -> FIXED CDO: Set to {door_mesh.get_name()}")
                    elif "Frame" in comp.get_name() and frame_mesh:
                        comp.set_static_mesh(frame_mesh)
                        unreal.log(f"    -> FIXED CDO: Set to {frame_mesh.get_name()}")

            # Save the Blueprint if we fixed CDO
            unreal.EditorAssetLibrary.save_asset(results["b_door_prison"]["path"], only_if_is_dirty=False)
else:
    unreal.log("B_Door_Prison Blueprint NOT FOUND")

# [2] Load the level
unreal.log(f"\n[2] LOADING LEVEL: {level_path}")
try:
    # Use level editor subsystem (newer API)
    editor_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if editor_subsystem:
        success = editor_subsystem.load_level(level_path)
        unreal.log(f"Load level via LevelEditorSubsystem: {success}")
    else:
        # Fallback to old API
        success = unreal.EditorLevelLibrary.load_level(level_path)
        unreal.log(f"Load level via EditorLevelLibrary: {success}")
except Exception as e:
    unreal.log_error(f"Failed to load level: {e}")

# [3] Find and fix door actors
unreal.log("\n[3] FINDING DOOR ACTORS IN LEVEL...")
try:
    # Get all actors using editor actor subsystem (newer API)
    editor_actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    if editor_actor_subsystem:
        all_actors = editor_actor_subsystem.get_all_level_actors()
    else:
        all_actors = unreal.EditorLevelLibrary.get_all_level_actors()

    unreal.log(f"Total actors in level: {len(all_actors)}")

    # Find door actors
    door_actors = []
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "Door" in class_name and "Boss" not in class_name:
            door_actors.append(actor)

    unreal.log(f"Door actors found: {len(door_actors)}")

    # Fix each door
    for actor in door_actors:
        actor_info = {
            "name": actor.get_name(),
            "class": actor.get_class().get_name(),
            "location": str(actor.get_actor_location()),
            "components": []
        }
        unreal.log(f"\n  Actor: {actor.get_name()} ({actor.get_class().get_name()})")
        unreal.log(f"  Location: {actor.get_actor_location()}")

        mesh_comps = actor.get_components_by_class(unreal.StaticMeshComponent)
        for comp in mesh_comps:
            mesh = comp.static_mesh
            visible = comp.is_visible()
            comp_info = {
                "name": comp.get_name(),
                "mesh": mesh.get_name() if mesh else "NONE",
                "visible": visible,
                "fixed": False
            }
            unreal.log(f"    [{comp.get_name()}] Mesh: {comp_info['mesh']}, Visible: {visible}")

            # Fix if needed
            if not mesh:
                if "Interactable" in comp.get_name() and door_mesh:
                    comp.set_static_mesh(door_mesh)
                    comp_info["fixed"] = True
                    results["fixed_count"] += 1
                    unreal.log(f"      -> FIXED: Set to {door_mesh.get_name()}")
                elif "Frame" in comp.get_name() and frame_mesh:
                    comp.set_static_mesh(frame_mesh)
                    comp_info["fixed"] = True
                    results["fixed_count"] += 1
                    unreal.log(f"      -> FIXED: Set to {frame_mesh.get_name()}")

            actor_info["components"].append(comp_info)
        results["level_doors"].append(actor_info)

except Exception as e:
    unreal.log_error(f"Error finding actors: {e}")

# [4] Save level if we fixed anything
unreal.log(f"\n[4] SUMMARY")
unreal.log(f"Doors found: {len(results['level_doors'])}")
unreal.log(f"Components fixed: {results['fixed_count']}")

if results["fixed_count"] > 0:
    unreal.log("\n[5] SAVING LEVEL...")
    try:
        if editor_subsystem:
            editor_subsystem.save_current_level()
        else:
            unreal.EditorLevelLibrary.save_current_level()
        unreal.log("Level saved successfully!")
    except Exception as e:
        unreal.log_error(f"Failed to save level: {e}")

# Save results
output_path = "C:/scripts/SLFConversion/migration_cache/door_prison_fix.json"
with open(output_path, "w") as f:
    json.dump(results, f, indent=2)
unreal.log(f"\nSaved to {output_path}")
