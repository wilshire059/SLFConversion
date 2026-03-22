"""Spawn B_BossDoor and check its component values"""
import unreal

BOSS_DOOR_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("SPAWNING B_BossDoor TO CHECK COMPONENTS")
    unreal.log_warning("=" * 60)

    bp = unreal.load_asset(BOSS_DOOR_PATH)
    if not bp:
        unreal.log_error(f"Could not load {BOSS_DOOR_PATH}")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("No generated class")
        return

    unreal.log_warning(f"Generated class: {gen_class.get_name()}")

    # Spawn an instance
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        unreal.log_error("No editor world")
        return

    spawn_loc = unreal.Vector(0, 0, 1000)
    spawn_rot = unreal.Rotator(0, 0, 0)

    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc, spawn_rot)
    if not actor:
        unreal.log_error("Failed to spawn actor")
        return

    unreal.log_warning(f"Spawned: {actor.get_name()}")

    # Get all components
    all_comps = actor.get_components_by_class(unreal.SceneComponent)
    unreal.log_warning(f"SceneComponents: {len(all_comps)}")

    for comp in all_comps:
        comp_name = comp.get_name()
        comp_class = comp.get_class().get_name()
        rel_loc = comp.relative_location
        rel_rot = comp.relative_rotation
        rel_scale = comp.relative_scale3d

        unreal.log_warning(f"  {comp_name} ({comp_class}):")
        unreal.log_warning(f"    Location: ({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})")
        unreal.log_warning(f"    Rotation: (P={rel_rot.pitch:.1f}, Y={rel_rot.yaw:.1f}, R={rel_rot.roll:.1f})")
        unreal.log_warning(f"    Scale: ({rel_scale.x:.4f}, {rel_scale.y:.4f}, {rel_scale.z:.4f})")

        # If StaticMeshComponent, check mesh
        if isinstance(comp, unreal.StaticMeshComponent):
            mesh = comp.static_mesh
            unreal.log_warning(f"    Mesh: {mesh.get_name() if mesh else 'None'}")
            coll = comp.get_collision_enabled()
            unreal.log_warning(f"    Collision: {coll}")

    # Delete the spawned actor
    actor.destroy_actor()
    unreal.log_warning("Actor destroyed")

    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    main()
