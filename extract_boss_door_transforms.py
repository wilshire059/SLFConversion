"""
Extract exact component transforms from B_BossDoor in bp_only
to compare with SLFConversion
"""
import unreal
import json
import os

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/boss_door_transforms_bp_only.json"

def vector_to_dict(vec):
    return {"X": float(vec.x), "Y": float(vec.y), "Z": float(vec.z)}

def rotator_to_dict(rot):
    return {"Pitch": float(rot.pitch), "Yaw": float(rot.yaw), "Roll": float(rot.roll)}

def main():
    print("=" * 60)
    print("Extracting B_BossDoor component transforms from bp_only")
    print("=" * 60)

    # Load the blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
    bp_asset = unreal.load_asset(bp_path)

    if not bp_asset:
        print(f"ERROR: Could not load {bp_path}")
        return

    gen_class = bp_asset.generated_class()
    if not gen_class:
        print("ERROR: No generated class")
        return

    print(f"Blueprint: {bp_path}")
    print(f"Generated class: {gen_class.get_name()}")

    # Spawn a temporary instance to get actual runtime transforms
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        print("ERROR: Could not get editor world")
        return

    # Spawn at origin
    spawn_transform = unreal.Transform()
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, 1000))

    if not actor:
        print("ERROR: Could not spawn actor")
        return

    print(f"Spawned actor: {actor.get_name()}")

    result = {
        "BlueprintPath": bp_path,
        "Components": {}
    }

    # Get all components
    all_components = actor.get_components_by_class(unreal.SceneComponent)
    print(f"Found {len(all_components)} scene components")

    for comp in all_components:
        comp_name = comp.get_name()
        comp_class = comp.get_class().get_name()

        comp_data = {
            "Class": comp_class,
            "RelativeLocation": vector_to_dict(comp.relative_location),
            "RelativeRotation": rotator_to_dict(comp.relative_rotation),
            "RelativeScale": vector_to_dict(comp.relative_scale3d),
        }

        # Get parent
        parent = comp.get_attach_parent()
        if parent:
            comp_data["AttachParent"] = parent.get_name()

        # For StaticMeshComponent, get mesh
        if comp_class == "StaticMeshComponent":
            smc = unreal.StaticMeshComponent.cast(comp)
            if smc:
                mesh = smc.get_editor_property("static_mesh")
                if mesh:
                    comp_data["StaticMesh"] = mesh.get_path_name()

        # For NiagaraComponent, get asset
        if comp_class == "NiagaraComponent":
            try:
                nc = unreal.NiagaraComponent.cast(comp)
                if nc:
                    asset = nc.get_editor_property("asset")
                    if asset:
                        comp_data["NiagaraAsset"] = asset.get_path_name()
            except:
                pass

        result["Components"][comp_name] = comp_data

        print(f"\n  {comp_name} ({comp_class})")
        print(f"    Location: {comp_data['RelativeLocation']}")
        print(f"    Rotation: {comp_data['RelativeRotation']}")
        print(f"    Scale: {comp_data['RelativeScale']}")
        if "AttachParent" in comp_data:
            print(f"    Parent: {comp_data['AttachParent']}")
        if "StaticMesh" in comp_data:
            print(f"    Mesh: {comp_data['StaticMesh']}")

    # Clean up - destroy the spawned actor
    actor.destroy_actor()
    print("\nSpawned actor destroyed")

    # Save results
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(result, f, indent=2)

    print(f"\nSaved to: {OUTPUT_PATH}")
    print("=" * 60)

if __name__ == "__main__":
    main()
