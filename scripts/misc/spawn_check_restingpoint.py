import unreal
import json
import os

def spawn_and_check_restingpoint():
    """Spawn B_RestingPoint and check its components at runtime"""

    output_path = "C:/scripts/slfconversion/migration_cache/restingpoint_spawned_data.json"

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"

    # Load the Blueprint class
    bp_class = unreal.load_class(None, bp_path + ".B_RestingPoint_C")
    if not bp_class:
        bp = unreal.load_asset(bp_path)
        if bp:
            bp_class = bp.generated_class()

    if not bp_class:
        print(f"ERROR: Could not load class from {bp_path}")
        return

    print(f"Loaded class: {bp_class.get_name()}")

    # Get editor world
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        print("ERROR: No editor world available")
        return

    # Spawn the actor
    spawn_location = unreal.Vector(0, 0, 1000)
    spawn_rotation = unreal.Rotator(0, 0, 0)

    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        bp_class,
        spawn_location,
        spawn_rotation
    )

    if not actor:
        print("ERROR: Failed to spawn actor")
        return

    print(f"Spawned actor: {actor.get_name()}")

    data = {
        "actor_name": actor.get_name(),
        "actor_class": actor.get_class().get_name(),
        "components": []
    }

    # Get all components
    components = actor.get_components_by_class(unreal.ActorComponent)
    print(f"\nFound {len(components)} components:")

    for comp in components:
        comp_info = {
            "name": comp.get_name(),
            "class": comp.get_class().get_name(),
            "class_path": comp.get_class().get_path_name(),
            "properties": {}
        }

        class_name = comp.get_class().get_name()
        print(f"\n  {comp.get_name()} ({class_name})")

        # Check if it's a NiagaraComponent
        if "NiagaraComponent" in class_name:
            niagara_comp = unreal.NiagaraComponent.cast(comp)
            if niagara_comp:
                try:
                    asset = niagara_comp.get_asset()
                    if asset:
                        comp_info["properties"]["niagara_system"] = asset.get_path_name()
                        print(f"      NiagaraSystem: {asset.get_path_name()}")
                    else:
                        comp_info["properties"]["niagara_system"] = None
                        print(f"      NiagaraSystem: NONE")
                except Exception as e:
                    comp_info["properties"]["niagara_system_error"] = str(e)
                    print(f"      NiagaraSystem ERROR: {e}")

                try:
                    auto_activate = niagara_comp.get_editor_property('auto_activate')
                    comp_info["properties"]["auto_activate"] = auto_activate
                    print(f"      AutoActivate: {auto_activate}")
                except:
                    pass

        # Check scene component properties
        if hasattr(comp, 'get_relative_location'):
            try:
                rel_loc = comp.get_relative_location() if hasattr(comp, 'get_relative_location') else None
                if rel_loc:
                    comp_info["properties"]["relative_location"] = f"{rel_loc.x:.1f},{rel_loc.y:.1f},{rel_loc.z:.1f}"
            except:
                pass

        # Check PointLightComponent
        if "PointLightComponent" in class_name:
            try:
                intensity = comp.get_editor_property('intensity')
                comp_info["properties"]["intensity"] = intensity
                print(f"      Intensity: {intensity}")
            except:
                pass

        # Check SpringArmComponent
        if "SpringArmComponent" in class_name:
            try:
                arm_length = comp.get_editor_property('target_arm_length')
                comp_info["properties"]["target_arm_length"] = arm_length
                print(f"      TargetArmLength: {arm_length}")
            except:
                pass

        # Check StaticMeshComponent
        if "StaticMeshComponent" in class_name:
            try:
                mesh = comp.get_editor_property('static_mesh')
                if mesh:
                    comp_info["properties"]["static_mesh"] = mesh.get_path_name()
                    print(f"      StaticMesh: {mesh.get_path_name()}")
                else:
                    comp_info["properties"]["static_mesh"] = None
                    print(f"      StaticMesh: NONE")
            except:
                pass

        data["components"].append(comp_info)

    # Delete the spawned actor
    unreal.EditorLevelLibrary.destroy_actor(actor)
    print(f"\nDestroyed spawned actor")

    # Save to JSON
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=2)

    print(f"\n\nSaved to: {output_path}")

    # Summary
    print("\n" + "="*80)
    print("NIAGARA COMPONENTS SUMMARY")
    print("="*80)
    for comp in data["components"]:
        if "Niagara" in comp["class"]:
            ns = comp["properties"].get("niagara_system", "N/A")
            print(f"  {comp['name']}: {ns}")

if __name__ == "__main__":
    spawn_and_check_restingpoint()
