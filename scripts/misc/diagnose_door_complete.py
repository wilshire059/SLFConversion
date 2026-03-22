"""
Complete diagnostic of B_Door - check everything that could cause invisibility.
"""
import unreal
import json

results = {"cdo": {}, "spawned": {}, "level_instances": []}

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"

unreal.log("=" * 70)
unreal.log("COMPLETE B_Door DIAGNOSTIC")
unreal.log("=" * 70)

bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if not bp:
    unreal.log_error(f"Could not load {bp_path}")
else:
    gen_class = bp.generated_class()
    cdo = unreal.get_default_object(gen_class)

    unreal.log("\n[1] CDO COMPONENT ANALYSIS")
    unreal.log("-" * 50)

    # Check ALL components, not just StaticMesh
    all_comps = cdo.get_components_by_class(unreal.ActorComponent)
    results["cdo"]["total_components"] = len(all_comps)
    results["cdo"]["components"] = []

    for comp in all_comps:
        comp_info = {
            "name": comp.get_name(),
            "class": comp.get_class().get_name(),
        }

        unreal.log(f"\n  [{comp.get_name()}] - {comp.get_class().get_name()}")

        if isinstance(comp, unreal.SceneComponent):
            # Check visibility
            comp_info["visible"] = comp.is_visible()
            comp_info["hidden_in_game"] = comp.get_editor_property('hidden_in_game') if hasattr(comp, 'hidden_in_game') else "N/A"
            unreal.log(f"    Visible: {comp_info['visible']}")

            # Check transform
            try:
                world_loc = comp.get_editor_property('relative_location')
                world_scale = comp.get_editor_property('relative_scale3d')
                comp_info["relative_location"] = f"X={world_loc.x:.1f} Y={world_loc.y:.1f} Z={world_loc.z:.1f}"
                comp_info["relative_scale"] = f"X={world_scale.x:.2f} Y={world_scale.y:.2f} Z={world_scale.z:.2f}"
                unreal.log(f"    Location: {comp_info['relative_location']}")
                unreal.log(f"    Scale: {comp_info['relative_scale']}")

                # Check for zero scale
                if world_scale.x == 0 or world_scale.y == 0 or world_scale.z == 0:
                    unreal.log_warning(f"    WARNING: Zero scale detected!")
            except Exception as e:
                comp_info["transform_error"] = str(e)

            # Check attachment
            try:
                attach_parent = comp.get_attach_parent()
                comp_info["attach_parent"] = attach_parent.get_name() if attach_parent else "None (Root)"
                unreal.log(f"    Attached to: {comp_info['attach_parent']}")
            except:
                pass

        if isinstance(comp, unreal.StaticMeshComponent):
            mesh = comp.static_mesh
            comp_info["static_mesh"] = mesh.get_name() if mesh else "NO_MESH"
            unreal.log(f"    Static Mesh: {comp_info['static_mesh']}")

            # Check collision
            try:
                collision = comp.get_collision_enabled()
                comp_info["collision_enabled"] = str(collision)
                unreal.log(f"    Collision: {collision}")
            except:
                pass

            # Check render settings
            try:
                comp_info["cast_shadow"] = comp.get_editor_property('cast_shadow')
                comp_info["visible_in_scene_capture_only"] = comp.get_editor_property('visible_in_scene_capture_only')
                comp_info["hidden_in_scene_capture"] = comp.get_editor_property('hidden_in_scene_capture')
                unreal.log(f"    Cast Shadow: {comp_info['cast_shadow']}")
                unreal.log(f"    Visible in Scene Capture Only: {comp_info['visible_in_scene_capture_only']}")
            except Exception as e:
                unreal.log(f"    Render props error: {e}")

        results["cdo"]["components"].append(comp_info)

    # [2] Spawn test
    unreal.log("\n\n[2] SPAWN TEST")
    unreal.log("-" * 50)

    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        location = unreal.Vector(50000, 50000, 0)
        rotation = unreal.Rotator(0, 0, 0)

        spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, location, rotation)
        if spawned:
            unreal.log(f"Spawned actor: {spawned.get_name()}")
            results["spawned"]["success"] = True

            spawn_comps = spawned.get_components_by_class(unreal.StaticMeshComponent)
            results["spawned"]["components"] = []

            for comp in spawn_comps:
                comp_info = {
                    "name": comp.get_name(),
                    "visible": comp.is_visible(),
                }
                mesh = comp.static_mesh
                comp_info["mesh"] = mesh.get_name() if mesh else "NO_MESH"

                # Get world transform
                try:
                    world_loc = comp.get_editor_property('relative_location')
                    world_scale = comp.get_editor_property('relative_scale3d')
                    comp_info["scale"] = f"X={world_scale.x:.2f} Y={world_scale.y:.2f} Z={world_scale.z:.2f}"
                except:
                    pass

                results["spawned"]["components"].append(comp_info)
                unreal.log(f"  [{comp.get_name()}] Mesh: {comp_info['mesh']}, Visible: {comp_info['visible']}, Scale: {comp_info.get('scale', 'N/A')}")

            spawned.destroy_actor()
        else:
            results["spawned"]["success"] = False
            unreal.log_error("Failed to spawn actor")

    # [3] Check level instances
    unreal.log("\n\n[3] LEVEL INSTANCES")
    unreal.log("-" * 50)

    all_actors = unreal.EditorLevelLibrary.get_all_level_actors()
    door_actors = [a for a in all_actors if "B_Door" in a.get_class().get_name() and "Boss" not in a.get_class().get_name()]

    unreal.log(f"Found {len(door_actors)} B_Door instances in level")

    for actor in door_actors[:3]:  # Check first 3
        actor_info = {
            "name": actor.get_name(),
            "location": str(actor.get_actor_location()),
            "components": []
        }

        mesh_comps = actor.get_components_by_class(unreal.StaticMeshComponent)
        for comp in mesh_comps:
            mesh = comp.static_mesh
            comp_data = {
                "name": comp.get_name(),
                "mesh": mesh.get_name() if mesh else "NO_MESH",
                "visible": comp.is_visible()
            }
            actor_info["components"].append(comp_data)
            unreal.log(f"  {actor.get_name()} -> [{comp.get_name()}] Mesh: {comp_data['mesh']}, Visible: {comp_data['visible']}")

        results["level_instances"].append(actor_info)

# Save results
output_path = "C:/scripts/SLFConversion/migration_cache/door_complete_diagnostic.json"
with open(output_path, "w") as f:
    json.dump(results, f, indent=2)

unreal.log(f"\n\nSaved to {output_path}")
