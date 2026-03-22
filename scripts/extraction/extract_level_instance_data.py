"""
Extract B_BossDoor instance data from level maps to see scale/transform values
"""
import unreal
import json
import os

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/boss_door_level_instance.json"

def vector_to_dict(vec):
    return {"X": float(vec.x), "Y": float(vec.y), "Z": float(vec.z)}

def rotator_to_dict(rot):
    return {"Pitch": float(rot.pitch), "Yaw": float(rot.yaw), "Roll": float(rot.roll)}

def main():
    print("=" * 60)
    print("Extracting B_BossDoor level instance data")
    print("=" * 60)

    # Try to find boss door instances in the demo level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    # Load the level - we use EditorAssetLibrary
    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)

    # Actually let's search the asset registry for all instances
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Find all B_BossDoor assets in the project
    print("\nSearching for B_BossDoor assets...")

    result = {
        "BlueprintPath": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor",
        "DefaultComponentScales": {},
        "LevelInstances": []
    }

    # Load the B_BossDoor blueprint and inspect CDO's components
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
    bp_asset = unreal.load_asset(bp_path)

    if bp_asset:
        gen_class = bp_asset.generated_class()
        if gen_class:
            # Spawn a temporary actor to see the component hierarchy
            world = unreal.EditorLevelLibrary.get_editor_world()
            if world:
                print(f"\nWorld: {world.get_name()}")

                # Get all actors of class B_BossDoor_C
                boss_doors = unreal.GameplayStatics.get_all_actors_of_class(world, gen_class)
                print(f"Found {len(boss_doors)} B_BossDoor actors in world")

                for actor in boss_doors:
                    actor_info = {
                        "Name": actor.get_name(),
                        "Location": vector_to_dict(actor.get_actor_location()),
                        "Rotation": rotator_to_dict(actor.get_actor_rotation()),
                        "Scale": vector_to_dict(actor.get_actor_scale3d()),
                        "Components": []
                    }

                    # Get all components
                    components = actor.get_components_by_class(unreal.SceneComponent)
                    for comp in components:
                        comp_info = {
                            "Name": comp.get_name(),
                            "Class": comp.get_class().get_name(),
                            "RelativeLocation": vector_to_dict(comp.relative_location),
                            "RelativeRotation": rotator_to_dict(comp.relative_rotation),
                            "RelativeScale3D": vector_to_dict(comp.relative_scale3d)
                        }

                        # For StaticMeshComponent, get mesh
                        if isinstance(comp, unreal.StaticMeshComponent):
                            mesh = comp.get_editor_property("static_mesh")
                            if mesh:
                                comp_info["StaticMesh"] = mesh.get_path_name()
                            comp_info["CollisionEnabled"] = str(comp.get_collision_enabled())

                        # For NiagaraComponent, get asset
                        if comp.get_class().get_name() == "NiagaraComponent":
                            try:
                                asset = comp.get_editor_property("asset")
                                if asset:
                                    comp_info["NiagaraAsset"] = asset.get_path_name()
                            except:
                                pass

                        actor_info["Components"].append(comp_info)
                        print(f"  Component: {comp_info['Name']} ({comp_info['Class']})")
                        print(f"    Scale: {comp_info['RelativeScale3D']}")

                    result["LevelInstances"].append(actor_info)

    # Save results
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(result, f, indent=2)

    print(f"\nSaved to: {OUTPUT_PATH}")
    print("=" * 60)

if __name__ == "__main__":
    main()
