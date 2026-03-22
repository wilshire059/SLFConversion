"""Extract B_BossDoor exact scale and location from bp_only backup"""
import unreal

BOSS_DOOR_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("EXTRACTING B_BossDoor COMPONENT DATA FROM bp_only")
    unreal.log_warning("=" * 60)

    bp = unreal.load_asset(BOSS_DOOR_PATH)
    if not bp:
        unreal.log_error(f"Could not load {BOSS_DOOR_PATH}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")

    gen = bp.generated_class()
    if not gen:
        unreal.log_error("No generated class")
        return

    unreal.log_warning(f"Generated class: {gen.get_name()}")

    cdo = unreal.get_default_object(gen)
    if not cdo:
        unreal.log_error("No CDO")
        return

    unreal.log_warning(f"CDO class: {cdo.get_class().get_name()}")

    # Get all StaticMeshComponents
    unreal.log_warning("")
    unreal.log_warning("=== STATIC MESH COMPONENTS ===")
    mesh_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
    for c in mesh_comps:
        scale = c.relative_scale3d
        loc = c.relative_location
        rot = c.relative_rotation
        mesh = c.static_mesh
        collision = c.get_collision_enabled()
        unreal.log_warning(f"  {c.get_name()}:")
        unreal.log_warning(f"    Scale: ({scale.x:.4f}, {scale.y:.4f}, {scale.z:.4f})")
        unreal.log_warning(f"    Location: ({loc.x:.2f}, {loc.y:.2f}, {loc.z:.2f})")
        unreal.log_warning(f"    Rotation: (P={rot.pitch:.1f}, Y={rot.yaw:.1f}, R={rot.roll:.1f})")
        unreal.log_warning(f"    Mesh: {mesh.get_name() if mesh else 'None'}")
        unreal.log_warning(f"    Collision: {collision}")

    # Get all SceneComponents to find the doorway/frame
    unreal.log_warning("")
    unreal.log_warning("=== ALL SCENE COMPONENTS ===")
    scene_comps = cdo.get_components_by_class(unreal.SceneComponent)
    for c in scene_comps:
        loc = c.relative_location
        scale = c.relative_scale3d
        rot = c.relative_rotation
        unreal.log_warning(f"  {c.get_class().get_name()}: {c.get_name()}")
        unreal.log_warning(f"    Location: ({loc.x:.2f}, {loc.y:.2f}, {loc.z:.2f})")
        unreal.log_warning(f"    Scale: ({scale.x:.4f}, {scale.y:.4f}, {scale.z:.4f})")
        unreal.log_warning(f"    Rotation: (P={rot.pitch:.1f}, Y={rot.yaw:.1f}, R={rot.roll:.1f})")

    # Get BoxComponent if exists (for collision)
    unreal.log_warning("")
    unreal.log_warning("=== BOX COMPONENTS ===")
    try:
        box_comps = cdo.get_components_by_class(unreal.BoxComponent)
        for c in box_comps:
            extent = c.box_extent
            loc = c.relative_location
            unreal.log_warning(f"  {c.get_name()}:")
            unreal.log_warning(f"    Extent: ({extent.x:.2f}, {extent.y:.2f}, {extent.z:.2f})")
            unreal.log_warning(f"    Location: ({loc.x:.2f}, {loc.y:.2f}, {loc.z:.2f})")
    except:
        unreal.log_warning("  No box components")

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("EXTRACTION COMPLETE")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    main()
