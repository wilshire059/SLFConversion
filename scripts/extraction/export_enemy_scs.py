"""
Export enemy weapon and mesh info via CDO inspection
"""
import unreal
import json

ENEMY_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
]

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/enemy_scs_info.json"

def main():
    result = {}

    for bp_path in ENEMY_BLUEPRINTS:
        bp = unreal.load_asset(bp_path)
        if not bp:
            print(f"ERROR: Could not load {bp_path}")
            continue

        bp_name = bp_path.split("/")[-1]
        print(f"\n=== {bp_name} ===")

        gen_class = bp.generated_class()
        if not gen_class:
            print("  ERROR: No generated class")
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            print("  ERROR: No CDO")
            continue

        bp_info = {"components": []}

        # Get all components from CDO
        all_components = cdo.get_components_by_class(unreal.ActorComponent)

        for comp in all_components:
            comp_name = comp.get_name()
            comp_class = comp.get_class().get_name()

            comp_info = {
                "name": comp_name,
                "class": comp_class
            }

            # Check for ChildActorComponent
            if isinstance(comp, unreal.ChildActorComponent):
                try:
                    child_class = comp.get_editor_property("child_actor_class")
                    if child_class:
                        comp_info["child_actor_class"] = child_class.get_name()
                        comp_info["child_actor_class_path"] = child_class.get_path_name()
                        print(f"  {comp_name}: ChildActorClass = {child_class.get_name()}")
                except Exception as e:
                    print(f"  {comp_name}: Error getting child_actor_class: {e}")

            # Check for SkeletalMeshComponent
            if isinstance(comp, unreal.SkeletalMeshComponent):
                try:
                    mesh = comp.skeletal_mesh
                    if mesh:
                        comp_info["skeletal_mesh"] = mesh.get_name()
                        comp_info["skeletal_mesh_path"] = mesh.get_path_name()
                        print(f"  {comp_name}: SkeletalMesh = {mesh.get_name()}")
                except Exception as e:
                    print(f"  {comp_name}: Error getting skeletal_mesh: {e}")

            bp_info["components"].append(comp_info)

        # Also try to get any "Weapon" variable directly
        try:
            weapon = cdo.get_editor_property("Weapon")
            if weapon:
                print(f"  Weapon property: {weapon}")
        except:
            pass

        result[bp_name] = bp_info

    # Save to file
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(result, f, indent=2)

    print(f"\nSaved to: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
