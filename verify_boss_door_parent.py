"""Verify B_BossDoor parent class after migration"""
import unreal

BOSS_DOOR_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"

def main():
    print("=" * 60)
    print("VERIFYING B_BossDoor PARENT CLASS")
    print("=" * 60)

    bp = unreal.load_asset(BOSS_DOOR_PATH)
    if not bp:
        print(f"ERROR: Could not load {BOSS_DOOR_PATH}")
        return

    print(f"Blueprint loaded: {bp.get_name()}")

    # Get parent class from Blueprint
    try:
        parent = bp.get_editor_property("parent_class")
        if parent:
            print(f"Parent class: {parent.get_name()}")
            print(f"Parent class path: {parent.get_path_name()}")
    except Exception as e:
        print(f"Could not get parent_class: {e}")

    # Get generated class
    gen = bp.generated_class()
    if gen:
        print(f"Generated class: {gen.get_name()}")

        # Check if it's SLFBossDoor
        target_class = unreal.load_class(None, "/Script/SLFConversion.SLFBossDoor")
        if target_class:
            print(f"\nTarget class (SLFBossDoor): {target_class.get_name()}")
            print(f"Target class path: {target_class.get_path_name()}")

        # Get CDO and check components
        cdo = unreal.get_default_object(gen)
        if cdo:
            print("\n--- CDO Component Check ---")

            # Check for mesh components
            mesh_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
            print(f"StaticMeshComponents: {len(mesh_comps)}")
            for c in mesh_comps:
                scale = c.relative_scale3d
                loc = c.relative_location
                rot = c.relative_rotation
                mesh = c.static_mesh
                print(f"  {c.get_name()}:")
                print(f"    Scale: ({scale.x:.2f}, {scale.y:.2f}, {scale.z:.2f})")
                print(f"    Location: ({loc.x:.2f}, {loc.y:.2f}, {loc.z:.2f})")
                print(f"    Rotation: (Pitch={rot.pitch:.1f}, Yaw={rot.yaw:.1f}, Roll={rot.roll:.1f})")
                print(f"    Mesh: {mesh.get_name() if mesh else 'None'}")

            # Check for Niagara components
            try:
                niagara_comps = cdo.get_components_by_class(unreal.NiagaraComponent)
                print(f"NiagaraComponents: {len(niagara_comps)}")
                for c in niagara_comps:
                    print(f"  {c.get_name()}")
            except Exception as e:
                print(f"Could not check Niagara components: {e}")

            # Check collision settings
            if mesh_comps:
                fog_mesh = mesh_comps[0] if mesh_comps else None
                if fog_mesh:
                    collision = fog_mesh.get_collision_enabled()
                    print(f"\nCollision enabled: {collision}")
                    print(f"Visibility: {fog_mesh.is_visible()}")

    print("\n" + "=" * 60)

if __name__ == "__main__":
    main()
