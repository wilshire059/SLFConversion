"""
Extract weapon configuration from enemy Blueprints.
Gets ChildActorClass from ChildActorComponent via CDO iteration.
"""
import unreal
import json

ENEMY_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
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
    print(f"WEAPON CONFIG EXTRACTION - {project_name}")
    print(f"{'='*70}")

    all_results = {}

    for bp_path in ENEMY_BLUEPRINTS:
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

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            print(f"  ERROR: No CDO")
            all_results[bp_name] = {"error": "No CDO"}
            continue

        # Get parent class name safely
        parent_name = "None"
        try:
            parent = unreal.SystemLibrary.get_super_class(gen_class)
            if parent:
                parent_name = parent.get_name()
        except:
            pass

        result = {
            "class": gen_class.get_name(),
            "parent": parent_name,
            "components": []
        }

        # Get ALL components
        try:
            all_components = cdo.get_components_by_class(unreal.ActorComponent)
            print(f"  Found {len(all_components)} components")

            for comp in all_components:
                comp_info = {
                    "name": comp.get_name(),
                    "class": comp.get_class().get_name()
                }

                # ChildActorComponent special handling
                if isinstance(comp, unreal.ChildActorComponent):
                    try:
                        child_class = comp.get_editor_property("child_actor_class")
                        if child_class:
                            comp_info["child_actor_class"] = child_class.get_name()
                            comp_info["child_actor_class_path"] = child_class.get_path_name()
                            print(f"  FOUND: {comp.get_name()} -> ChildActorClass: {child_class.get_name()}")
                        else:
                            comp_info["child_actor_class"] = "NONE"
                            print(f"  {comp.get_name()}: ChildActorClass is None")
                    except Exception as e:
                        comp_info["error"] = str(e)
                        print(f"  {comp.get_name()}: Error getting ChildActorClass: {e}")

                # Scene component transform
                if isinstance(comp, unreal.SceneComponent):
                    try:
                        rel_loc = comp.get_editor_property("relative_location")
                        rel_rot = comp.get_editor_property("relative_rotation")
                        comp_info["relative_location"] = f"({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})"
                        comp_info["relative_rotation"] = f"({rel_rot.pitch:.2f}, {rel_rot.yaw:.2f}, {rel_rot.roll:.2f})"

                        # Get attach info
                        attach_parent = comp.get_attach_parent()
                        if attach_parent:
                            comp_info["attach_parent"] = attach_parent.get_name()
                        attach_socket = comp.get_attach_socket_name()
                        if attach_socket and str(attach_socket) != "None":
                            comp_info["attach_socket"] = str(attach_socket)
                    except Exception as e:
                        pass

                result["components"].append(comp_info)

        except Exception as e:
            print(f"  ERROR getting components: {e}")
            result["error"] = str(e)

        all_results[bp_name] = result

        # Print summary of ChildActorComponents
        child_comps = [c for c in result["components"] if c["class"] == "ChildActorComponent"]
        print(f"\n  ChildActorComponents: {len(child_comps)}")
        for cc in child_comps:
            child_class = cc.get("child_actor_class", "UNKNOWN")
            rel_rot = cc.get("relative_rotation", "N/A")
            socket = cc.get("attach_socket", "None")
            print(f"    - {cc['name']}: class={child_class}, socket={socket}, rotation={rel_rot}")

    # Save to file
    output_file = f"C:/scripts/SLFConversion/migration_cache/weapon_config_{project_name}.json"
    with open(output_file, 'w') as f:
        json.dump(all_results, f, indent=2)

    print(f"\n{'='*70}")
    print(f"Saved to: {output_file}")
    print(f"{'='*70}")

if __name__ == "__main__":
    main()
