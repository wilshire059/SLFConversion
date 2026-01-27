"""
Extract weapon mesh and component configuration from weapon Blueprints.
"""
import unreal
import json

WEAPON_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_AI_Weapon",
]

def main():
    # Determine project
    project_name = "unknown"
    try:
        project_path = unreal.Paths.get_project_file_path()
        if "bp_only" in project_path.lower():
            project_name = "bp_only"
        elif "slfconversion" in project_path.lower():
            project_name = "SLFConversion"
    except:
        pass

    print(f"\n{'='*70}")
    print(f"WEAPON MESH EXTRACTION - {project_name}")
    print(f"{'='*70}")

    all_results = {}

    for bp_path in WEAPON_BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        print(f"\n--- {bp_name} ---")

        bp = unreal.load_asset(bp_path)
        if not bp:
            print(f"  ERROR: Could not load")
            all_results[bp_name] = {"error": "Could not load"}
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            print(f"  ERROR: No generated class")
            all_results[bp_name] = {"error": "No generated class"}
            continue

        result = {
            "path": bp_path,
            "class": gen_class.get_name(),
            "components": []
        }

        # Get parent class path
        try:
            parent = unreal.SystemLibrary.get_super_class(gen_class)
            if parent:
                result["parent_class"] = parent.get_name()
                result["parent_class_path"] = parent.get_path_name()
        except:
            pass

        # Spawn a temp actor to inspect
        try:
            world = unreal.EditorLevelLibrary.get_editor_world()
            if world:
                spawn_loc = unreal.Vector(0, 0, -100000)
                spawn_rot = unreal.Rotator(0, 0, 0)
                actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc, spawn_rot)

                if actor:
                    print(f"  Spawned: {actor.get_name()}")

                    # Get all components
                    all_comps = actor.get_components_by_class(unreal.ActorComponent)
                    print(f"  Components: {len(all_comps)}")

                    for comp in all_comps:
                        comp_info = {
                            "name": comp.get_name(),
                            "class": comp.get_class().get_name()
                        }

                        # Scene component transform
                        if isinstance(comp, unreal.SceneComponent):
                            try:
                                rel_loc = comp.get_editor_property("relative_location")
                                rel_rot = comp.get_editor_property("relative_rotation")
                                rel_scale = comp.get_editor_property("relative_scale3d")
                                comp_info["relative_location"] = f"({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})"
                                comp_info["relative_rotation"] = f"({rel_rot.pitch:.2f}, {rel_rot.yaw:.2f}, {rel_rot.roll:.2f})"
                                comp_info["relative_scale"] = f"({rel_scale.x:.2f}, {rel_scale.y:.2f}, {rel_scale.z:.2f})"
                            except Exception as e:
                                pass

                        # StaticMeshComponent
                        if isinstance(comp, unreal.StaticMeshComponent):
                            try:
                                mesh = comp.get_editor_property("static_mesh")
                                if mesh:
                                    comp_info["static_mesh"] = mesh.get_name()
                                    comp_info["static_mesh_path"] = mesh.get_path_name()
                                    print(f"    {comp.get_name()}: mesh={mesh.get_name()}")
                                else:
                                    comp_info["static_mesh"] = "NONE"
                                    print(f"    {comp.get_name()}: mesh=NONE")
                            except Exception as e:
                                comp_info["mesh_error"] = str(e)
                                print(f"    {comp.get_name()}: error={e}")

                        result["components"].append(comp_info)

                    # Check for specific properties on the actor
                    try:
                        default_mesh = actor.get_editor_property("default_weapon_mesh")
                        if default_mesh:
                            result["default_weapon_mesh"] = str(default_mesh)
                            print(f"  DefaultWeaponMesh: {default_mesh}")
                    except:
                        pass

                    try:
                        rotation_offset = actor.get_editor_property("default_attachment_rotation_offset")
                        if rotation_offset:
                            result["default_attachment_rotation_offset"] = f"({rotation_offset.pitch:.2f}, {rotation_offset.yaw:.2f}, {rotation_offset.roll:.2f})"
                            print(f"  DefaultAttachmentRotationOffset: {result['default_attachment_rotation_offset']}")
                    except:
                        pass

                    # Destroy temp actor
                    actor.destroy_actor()
        except Exception as e:
            print(f"  Error spawning: {e}")
            result["spawn_error"] = str(e)

        all_results[bp_name] = result

    # Save output
    output_file = f"C:/scripts/SLFConversion/migration_cache/weapon_mesh_{project_name}.json"
    with open(output_file, 'w') as f:
        json.dump(all_results, f, indent=2)

    print(f"\n{'='*70}")
    print(f"Saved to: {output_file}")
    print(f"{'='*70}")

if __name__ == "__main__":
    main()
