"""
Fix doors in all available levels and check B_Door_Prison Blueprint.
"""
import unreal
import json

results = {
    "blueprints_checked": [],
    "levels_checked": [],
    "doors_fixed": 0
}

door_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoor.SM_PrisonDoor"
frame_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoorArch.SM_PrisonDoorArch"

unreal.log("=" * 70)
unreal.log("FIXING ALL DOORS")
unreal.log("=" * 70)

# Load meshes
door_mesh = unreal.EditorAssetLibrary.load_asset(door_mesh_path)
frame_mesh = unreal.EditorAssetLibrary.load_asset(frame_mesh_path)

# [1] Check all door-related Blueprints
unreal.log("\n[1] CHECKING DOOR BLUEPRINTS...")
door_bp_paths = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door_Demo",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door_Prison",
    "/Game/SoulslikeFramework/Demo/Blueprints/B_Door_Prison",
]

# Also search for any door Blueprint we might have missed
all_assets = unreal.EditorAssetLibrary.list_assets("/Game/SoulslikeFramework/", recursive=True)
for asset in all_assets:
    asset_str = str(asset)
    if "B_Door" in asset_str and "_C" not in asset_str and "Boss" not in asset_str:
        if asset_str.replace("."+asset_str.split(".")[-1], "") not in door_bp_paths:
            door_bp_paths.append(asset_str.replace("."+asset_str.split(".")[-1], ""))

unreal.log(f"Door Blueprints to check: {len(door_bp_paths)}")

for bp_path in door_bp_paths:
    unreal.log(f"\n  Checking: {bp_path}")
    if not unreal.EditorAssetLibrary.does_asset_exist(bp_path):
        unreal.log(f"    NOT FOUND")
        continue

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log(f"    FAILED TO LOAD")
        continue

    bp_info = {"path": bp_path, "components": []}

    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            mesh_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
            for comp in mesh_comps:
                mesh = comp.static_mesh
                comp_info = {
                    "name": comp.get_name(),
                    "mesh": mesh.get_name() if mesh else "NONE",
                    "fixed": False
                }
                unreal.log(f"    [{comp.get_name()}] Mesh: {comp_info['mesh']}")

                # Fix if needed
                if not mesh:
                    if "Interactable" in comp.get_name() and door_mesh:
                        comp.set_static_mesh(door_mesh)
                        comp_info["fixed"] = True
                        results["doors_fixed"] += 1
                        unreal.log(f"      -> FIXED: Set to {door_mesh.get_name()}")
                    elif "Frame" in comp.get_name() and frame_mesh:
                        comp.set_static_mesh(frame_mesh)
                        comp_info["fixed"] = True
                        results["doors_fixed"] += 1
                        unreal.log(f"      -> FIXED: Set to {frame_mesh.get_name()}")

                bp_info["components"].append(comp_info)

            # Save if we made changes
            if any(c["fixed"] for c in bp_info["components"]):
                unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
                unreal.log(f"    SAVED")

    results["blueprints_checked"].append(bp_info)

# [2] Check levels
unreal.log("\n[2] CHECKING LEVELS...")
levels_to_check = [
    "/Game/SoulslikeFramework/Maps/L_Demo_Menu",
    "/Game/SoulslikeFramework/Maps/L_Demo_Showcase",
]

for level_path in levels_to_check:
    unreal.log(f"\n  Level: {level_path}")
    level_info = {"path": level_path, "doors": []}

    if not unreal.EditorAssetLibrary.does_asset_exist(level_path):
        unreal.log(f"    NOT FOUND")
        continue

    # Load level
    try:
        editor_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
        if editor_subsystem:
            success = editor_subsystem.load_level(level_path)
        else:
            success = unreal.EditorLevelLibrary.load_level(level_path)
        unreal.log(f"    Load result: {success}")
    except Exception as e:
        unreal.log(f"    Load failed: {e}")
        continue

    # Get actors
    try:
        editor_actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
        if editor_actor_subsystem:
            all_actors = editor_actor_subsystem.get_all_level_actors()
        else:
            all_actors = unreal.EditorLevelLibrary.get_all_level_actors()

        # Find door actors
        door_actors = []
        for actor in all_actors:
            class_name = actor.get_class().get_name()
            if "Door" in class_name and "Boss" not in class_name:
                door_actors.append(actor)

        unreal.log(f"    Door actors found: {len(door_actors)}")

        for actor in door_actors:
            actor_info = {
                "name": actor.get_name(),
                "class": actor.get_class().get_name(),
                "location": str(actor.get_actor_location()),
                "components_fixed": 0
            }
            unreal.log(f"      Actor: {actor.get_name()}")

            mesh_comps = actor.get_components_by_class(unreal.StaticMeshComponent)
            for comp in mesh_comps:
                mesh = comp.static_mesh
                unreal.log(f"        [{comp.get_name()}] Mesh: {mesh.get_name() if mesh else 'NONE'}")

                if not mesh:
                    if "Interactable" in comp.get_name() and door_mesh:
                        comp.set_static_mesh(door_mesh)
                        actor_info["components_fixed"] += 1
                        results["doors_fixed"] += 1
                        unreal.log(f"          -> FIXED")
                    elif "Frame" in comp.get_name() and frame_mesh:
                        comp.set_static_mesh(frame_mesh)
                        actor_info["components_fixed"] += 1
                        results["doors_fixed"] += 1
                        unreal.log(f"          -> FIXED")

            level_info["doors"].append(actor_info)

        # Save level if we fixed anything
        level_fixed = sum(d["components_fixed"] for d in level_info["doors"])
        if level_fixed > 0:
            try:
                if editor_subsystem:
                    editor_subsystem.save_current_level()
                else:
                    unreal.EditorLevelLibrary.save_current_level()
                unreal.log(f"    Level saved!")
            except Exception as e:
                unreal.log(f"    Save failed: {e}")

    except Exception as e:
        unreal.log(f"    Actor search failed: {e}")

    results["levels_checked"].append(level_info)

# Summary
unreal.log("\n" + "=" * 70)
unreal.log("SUMMARY")
unreal.log("=" * 70)
unreal.log(f"Total components fixed: {results['doors_fixed']}")

# Save results
output_path = "C:/scripts/SLFConversion/migration_cache/all_doors_fix.json"
with open(output_path, "w") as f:
    json.dump(results, f, indent=2)
unreal.log(f"\nSaved to {output_path}")
