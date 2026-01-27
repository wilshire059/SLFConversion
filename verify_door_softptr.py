"""
Verify that DefaultDoorMesh TSoftObjectPtr was saved to B_Door CDO
"""
import unreal
import json

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"

results = {"softptr_values": {}, "status": "unknown"}

bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if bp:
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # Check the TSoftObjectPtr properties
            try:
                door_mesh = cdo.get_editor_property('default_door_mesh')
                results["softptr_values"]["default_door_mesh"] = str(door_mesh) if door_mesh else "None"
                unreal.log(f"DefaultDoorMesh: {door_mesh}")
            except Exception as e:
                results["softptr_values"]["default_door_mesh_error"] = str(e)
                unreal.log_warning(f"Could not get default_door_mesh: {e}")

            try:
                frame_mesh = cdo.get_editor_property('default_door_frame_mesh')
                results["softptr_values"]["default_door_frame_mesh"] = str(frame_mesh) if frame_mesh else "None"
                unreal.log(f"DefaultDoorFrameMesh: {frame_mesh}")
            except Exception as e:
                results["softptr_values"]["default_door_frame_mesh_error"] = str(e)
                unreal.log_warning(f"Could not get default_door_frame_mesh: {e}")

            # Determine status
            door_ok = results["softptr_values"].get("default_door_mesh", "None") != "None"
            frame_ok = results["softptr_values"].get("default_door_frame_mesh", "None") != "None"

            if door_ok and frame_ok:
                results["status"] = "OK - Both TSoftObjectPtr properties set"
            elif door_ok:
                results["status"] = "PARTIAL - Only door mesh set"
            elif frame_ok:
                results["status"] = "PARTIAL - Only frame mesh set"
            else:
                results["status"] = "FAILED - No TSoftObjectPtr properties set"

            unreal.log(f"Status: {results['status']}")

with open("C:/scripts/SLFConversion/migration_cache/door_softptr_check.json", "w") as f:
    json.dump(results, f, indent=2)

unreal.log("Results saved to migration_cache/door_softptr_check.json")
