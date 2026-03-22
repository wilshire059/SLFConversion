"""
Extract B_BossDoor configuration by loading the demo level and examining instances
"""
import unreal
import json
import os

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/boss_door_bp_only_config.json"

def vector_to_dict(vec):
    return {"X": float(vec.x), "Y": float(vec.y), "Z": float(vec.z)}

def rotator_to_dict(rot):
    return {"Pitch": float(rot.pitch), "Yaw": float(rot.yaw), "Roll": float(rot.roll)}

def main():
    print("=" * 60)
    print("Extracting B_BossDoor from Demo Level")
    print("=" * 60)

    # Load the demo level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    print(f"Loading level: {level_path}")

    # Use EditorLoadingAndSavingUtils to load level
    try:
        success = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
        print(f"Level load result: {success}")
    except Exception as e:
        print(f"Error loading level: {e}")
        success = False

    # Get the editor world
    try:
        subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
        world = subsystem.get_editor_world()
        print(f"Got editor world: {world.get_name() if world else 'None'}")
    except Exception as e:
        print(f"Error getting world: {e}")
        world = None

    result = {
        "B_BossDoor": {
            "BlueprintPath": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor",
            "ClassName": "B_BossDoor_C",
            "Components": [],
            "LevelInstances": []
        },
        "B_Door": {
            "BlueprintPath": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door",
            "ClassName": "B_Door_C",
            "Components": []
        },
        "B_Interactable": {
            "BlueprintPath": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
            "ClassName": "B_Interactable_C",
            "Components": []
        }
    }

    if world:
        # Get B_BossDoor class
        bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
        bp_asset = unreal.load_asset(bp_path)

        if bp_asset:
            gen_class = bp_asset.generated_class()
            if gen_class:
                # Find all instances
                actors = unreal.GameplayStatics.get_all_actors_of_class(world, gen_class)
                print(f"\nFound {len(actors)} B_BossDoor instances")

                for actor in actors:
                    print(f"\n  Instance: {actor.get_name()}")
                    print(f"    Location: {actor.get_actor_location()}")
                    print(f"    Scale: {actor.get_actor_scale3d()}")

                    instance_info = {
                        "Name": actor.get_name(),
                        "ActorLocation": vector_to_dict(actor.get_actor_location()),
                        "ActorRotation": rotator_to_dict(actor.get_actor_rotation()),
                        "ActorScale3D": vector_to_dict(actor.get_actor_scale3d()),
                        "Components": []
                    }

                    # Get all components
                    components = actor.get_components_by_class(unreal.SceneComponent)
                    print(f"    Components ({len(components)}):")

                    for comp in components:
                        comp_info = {
                            "Name": comp.get_name(),
                            "Class": comp.get_class().get_name(),
                            "RelativeLocation": vector_to_dict(comp.relative_location),
                            "RelativeRotation": rotator_to_dict(comp.relative_rotation),
                            "RelativeScale3D": vector_to_dict(comp.relative_scale3d)
                        }

                        # For StaticMeshComponent
                        if isinstance(comp, unreal.StaticMeshComponent):
                            try:
                                mesh = comp.get_editor_property("static_mesh")
                                if mesh:
                                    comp_info["StaticMesh"] = mesh.get_path_name()
                            except:
                                pass

                        # For NiagaraComponent
                        if comp.get_class().get_name() == "NiagaraComponent":
                            try:
                                asset = comp.get_editor_property("asset")
                                if asset:
                                    comp_info["NiagaraAsset"] = asset.get_path_name()
                            except:
                                pass

                        instance_info["Components"].append(comp_info)
                        print(f"      - {comp_info['Name']} ({comp_info['Class']})")
                        print(f"        Scale: {comp_info['RelativeScale3D']}")
                        if 'StaticMesh' in comp_info:
                            print(f"        Mesh: {comp_info['StaticMesh']}")

                    result["B_BossDoor"]["LevelInstances"].append(instance_info)

    # Save results
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(result, f, indent=2)

    print(f"\n\nSaved to: {OUTPUT_PATH}")
    print("=" * 60)

if __name__ == "__main__":
    main()
