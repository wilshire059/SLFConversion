"""
Diagnose B_Door_Demo Blueprint specifically.
"""
import unreal
import json

results = {}

door_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoor.SM_PrisonDoor"
frame_mesh_path = "/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoorArch.SM_PrisonDoorArch"

unreal.log("=" * 70)
unreal.log("DIAGNOSING B_Door_Demo")
unreal.log("=" * 70)

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door_Demo"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if not bp:
    unreal.log_error("Could not load B_Door_Demo")
else:
    unreal.log(f"Loaded B_Door_Demo")

    # Check parent class
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log(f"Generated class: {gen_class.get_name()}")
        results["class_name"] = gen_class.get_name()

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log("\nCDO Component Analysis:")
            results["cdo_all_components"] = []

            # Get ALL components, not just StaticMeshComponent
            all_comps = cdo.get_components_by_class(unreal.ActorComponent)
            unreal.log(f"  Total components: {len(all_comps)}")

            for comp in all_comps:
                comp_info = {
                    "name": comp.get_name(),
                    "class": comp.get_class().get_name()
                }
                unreal.log(f"    [{comp.get_name()}] - {comp.get_class().get_name()}")

                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    comp_info["mesh"] = mesh.get_name() if mesh else "NO_MESH"
                    comp_info["visible"] = comp.is_visible()
                    unreal.log(f"      Mesh: {comp_info['mesh']}, Visible: {comp_info['visible']}")

                results["cdo_all_components"].append(comp_info)

            # Check for default_door_mesh property
            unreal.log("\nC++ Properties:")
            try:
                door_mesh = cdo.get_editor_property('default_door_mesh')
                unreal.log(f"  default_door_mesh: {door_mesh}")
                results["default_door_mesh"] = str(door_mesh) if door_mesh else "NOT_SET"
            except Exception as e:
                unreal.log(f"  default_door_mesh not found: {e}")

            try:
                frame_mesh = cdo.get_editor_property('default_door_frame_mesh')
                unreal.log(f"  default_door_frame_mesh: {frame_mesh}")
                results["default_door_frame_mesh"] = str(frame_mesh) if frame_mesh else "NOT_SET"
            except Exception as e:
                unreal.log(f"  default_door_frame_mesh not found: {e}")

# Check the level instance
unreal.log("\n\n" + "=" * 70)
unreal.log("CHECKING LEVEL INSTANCE")
unreal.log("=" * 70)

level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
try:
    editor_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if editor_subsystem:
        editor_subsystem.load_level(level_path)
except:
    unreal.EditorLevelLibrary.load_level(level_path)

try:
    editor_actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    if editor_actor_subsystem:
        all_actors = editor_actor_subsystem.get_all_level_actors()
    else:
        all_actors = unreal.EditorLevelLibrary.get_all_level_actors()

    unreal.log(f"Total actors in level: {len(all_actors)}")

    # Find the door
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "Door" in class_name and "Boss" not in class_name:
            unreal.log(f"\nFound Door: {actor.get_name()} ({class_name})")
            unreal.log(f"  Location: {actor.get_actor_location()}")

            results["level_instance"] = {
                "name": actor.get_name(),
                "class": class_name,
                "components": []
            }

            # Get ALL components
            all_comps = actor.get_components_by_class(unreal.ActorComponent)
            unreal.log(f"  Total components: {len(all_comps)}")

            for comp in all_comps:
                comp_info = {
                    "name": comp.get_name(),
                    "class": comp.get_class().get_name()
                }
                unreal.log(f"    [{comp.get_name()}] - {comp.get_class().get_name()}")

                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    visible = comp.is_visible()
                    comp_info["mesh"] = mesh.get_name() if mesh else "NO_MESH"
                    comp_info["visible"] = visible
                    unreal.log(f"      Mesh: {comp_info['mesh']}, Visible: {visible}")

                    # Fix if needed
                    if not mesh:
                        unreal.log(f"      FIXING...")
                        if "Interactable" in comp.get_name():
                            door_mesh_obj = unreal.EditorAssetLibrary.load_asset(door_mesh_path)
                            if door_mesh_obj:
                                comp.set_static_mesh(door_mesh_obj)
                                comp_info["fixed"] = True
                                unreal.log(f"      -> Applied {door_mesh_obj.get_name()}")
                        elif "Frame" in comp.get_name():
                            frame_mesh_obj = unreal.EditorAssetLibrary.load_asset(frame_mesh_path)
                            if frame_mesh_obj:
                                comp.set_static_mesh(frame_mesh_obj)
                                comp_info["fixed"] = True
                                unreal.log(f"      -> Applied {frame_mesh_obj.get_name()}")

                results["level_instance"]["components"].append(comp_info)

            # Save level if we fixed something
            if any(c.get("fixed", False) for c in results["level_instance"]["components"]):
                try:
                    if editor_subsystem:
                        editor_subsystem.save_current_level()
                    else:
                        unreal.EditorLevelLibrary.save_current_level()
                    unreal.log("  Level SAVED!")
                except Exception as e:
                    unreal.log_error(f"  Save failed: {e}")

except Exception as e:
    unreal.log_error(f"Error: {e}")

# Save results
output_path = "C:/scripts/SLFConversion/migration_cache/door_demo_diagnosis.json"
with open(output_path, "w") as f:
    json.dump(results, f, indent=2)
unreal.log(f"\nSaved to {output_path}")
