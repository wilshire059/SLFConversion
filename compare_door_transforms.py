"""
Compare B_Door_Demo component transforms between bp_only and SLFConversion.
Run this on BOTH projects to get the transform data.
"""
import unreal
import json
import os

results = {
    "project": "SLFConversion",  # Change to "bp_only" when running on backup
    "components": []
}

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door_Demo"

unreal.log("=" * 70)
unreal.log("B_Door_Demo TRANSFORM COMPARISON")
unreal.log("=" * 70)

bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if bp:
    gen_class = bp.generated_class()
    if gen_class:
        # Spawn to get actual component transforms
        world = unreal.EditorLevelLibrary.get_editor_world()
        if world:
            location = unreal.Vector(99999, 99999, 0)
            rotation = unreal.Rotator(0, 0, 0)
            spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, location, rotation)
            if spawned:
                unreal.log(f"Spawned: {spawned.get_name()}")

                # Get all components
                all_comps = spawned.get_components_by_class(unreal.SceneComponent)
                for comp in all_comps:
                    comp_info = {
                        "name": comp.get_name(),
                        "class": comp.get_class().get_name(),
                        "relative_location": {
                            "x": comp.relative_location.x,
                            "y": comp.relative_location.y,
                            "z": comp.relative_location.z
                        },
                        "relative_rotation": {
                            "pitch": comp.relative_rotation.pitch,
                            "yaw": comp.relative_rotation.yaw,
                            "roll": comp.relative_rotation.roll
                        },
                        "relative_scale": {
                            "x": comp.relative_scale3d.x,
                            "y": comp.relative_scale3d.y,
                            "z": comp.relative_scale3d.z
                        }
                    }

                    # Get mesh info if static mesh component
                    if isinstance(comp, unreal.StaticMeshComponent):
                        mesh = comp.static_mesh
                        comp_info["mesh"] = mesh.get_name() if mesh else "None"

                    unreal.log(f"\n  Component: {comp.get_name()} ({comp.get_class().get_name()})")
                    unreal.log(f"    Location: {comp.relative_location}")
                    unreal.log(f"    Rotation: {comp.relative_rotation}")
                    unreal.log(f"    Scale: {comp.relative_scale3d}")

                    results["components"].append(comp_info)

                # Clean up
                spawned.destroy_actor()

# Save results
output_path = "C:/scripts/SLFConversion/migration_cache/door_transforms_slfconv.json"
with open(output_path, "w") as f:
    json.dump(results, f, indent=2)
unreal.log(f"\nSaved to {output_path}")
