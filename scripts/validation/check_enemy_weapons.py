"""
Check enemy weapon ChildActorClass and skeleton mesh assignments
Run on bp_only project to see original values
"""
import unreal

ENEMY_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
]

def check_enemy_weapon(bp_path):
    """Check what weapon class and skeleton an enemy uses"""
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        print(f"ERROR: No generated class for {bp_path}")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        print(f"ERROR: No CDO for {bp_path}")
        return

    bp_name = bp_path.split("/")[-1]
    print(f"\n=== {bp_name} ===")

    # Get all components
    components = cdo.get_components_by_class(unreal.ActorComponent)

    for comp in components:
        comp_name = comp.get_name()
        comp_class = comp.get_class().get_name()

        # Check for ChildActorComponent (weapon)
        if isinstance(comp, unreal.ChildActorComponent):
            child_class = comp.get_editor_property("child_actor_class")
            if child_class:
                print(f"  {comp_name} (ChildActorComponent): {child_class.get_name()}")
            else:
                print(f"  {comp_name} (ChildActorComponent): None")

        # Check for SkeletalMeshComponent
        if isinstance(comp, unreal.SkeletalMeshComponent):
            mesh = comp.get_editor_property("skeletal_mesh")
            skeleton = comp.get_editor_property("skeletal_mesh_asset")
            if mesh:
                print(f"  {comp_name} (SkeletalMeshComponent): Mesh={mesh.get_name()}")
            elif hasattr(comp, 'skeletal_mesh'):
                skel_mesh = comp.skeletal_mesh
                if skel_mesh:
                    print(f"  {comp_name} (SkeletalMeshComponent): Mesh={skel_mesh.get_name()}")
                else:
                    print(f"  {comp_name} (SkeletalMeshComponent): No mesh assigned")
            else:
                print(f"  {comp_name} (SkeletalMeshComponent): Unable to get mesh")

def main():
    print("=" * 60)
    print("ENEMY WEAPON AND SKELETON CHECK")
    print("=" * 60)

    for bp_path in ENEMY_BLUEPRINTS:
        check_enemy_weapon(bp_path)

    print("\n" + "=" * 60)
    print("DONE")
    print("=" * 60)

if __name__ == "__main__":
    main()
