"""Complete verification of B_BossDoor after migration"""
import unreal

BOSS_DOOR_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("B_BossDoor COMPLETE VERIFICATION")
    unreal.log_warning("=" * 60)

    bp = unreal.load_asset(BOSS_DOOR_PATH)
    if not bp:
        unreal.log_error(f"Could not load {BOSS_DOOR_PATH}")
        return

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

    # Check StaticMeshComponents
    unreal.log_warning("")
    unreal.log_warning("=== STATIC MESH COMPONENTS ===")
    mesh_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
    unreal.log_warning(f"Count: {len(mesh_comps)}")
    for c in mesh_comps:
        scale = c.relative_scale3d
        loc = c.relative_location
        rot = c.relative_rotation
        mesh = c.static_mesh
        collision = c.get_collision_enabled()
        vis = c.is_visible()
        unreal.log_warning(f"  {c.get_name()}:")
        unreal.log_warning(f"    Scale: ({scale.x:.2f}, {scale.y:.2f}, {scale.z:.2f})")
        unreal.log_warning(f"    Location: ({loc.x:.2f}, {loc.y:.2f}, {loc.z:.2f})")
        unreal.log_warning(f"    Rotation: (P={rot.pitch:.1f}, Y={rot.yaw:.1f}, R={rot.roll:.1f})")
        unreal.log_warning(f"    Mesh: {mesh.get_name() if mesh else 'None'}")
        unreal.log_warning(f"    Collision: {collision}")
        unreal.log_warning(f"    Visible: {vis}")

    # Check Niagara Components
    unreal.log_warning("")
    unreal.log_warning("=== NIAGARA COMPONENTS ===")
    try:
        niagara_comps = cdo.get_components_by_class(unreal.NiagaraComponent)
        unreal.log_warning(f"Count: {len(niagara_comps)}")
        for c in niagara_comps:
            loc = c.relative_location
            rot = c.relative_rotation
            auto_act = c.auto_activate
            unreal.log_warning(f"  {c.get_name()}:")
            unreal.log_warning(f"    Location: ({loc.x:.2f}, {loc.y:.2f}, {loc.z:.2f})")
            unreal.log_warning(f"    Rotation: (P={rot.pitch:.1f}, Y={rot.yaw:.1f}, R={rot.roll:.1f})")
            unreal.log_warning(f"    AutoActivate: {auto_act}")
    except Exception as e:
        unreal.log_warning(f"Niagara check error: {e}")

    # Check Billboard Components
    unreal.log_warning("")
    unreal.log_warning("=== BILLBOARD COMPONENTS ===")
    try:
        billboard_comps = cdo.get_components_by_class(unreal.BillboardComponent)
        unreal.log_warning(f"Count: {len(billboard_comps)}")
        for c in billboard_comps:
            loc = c.relative_location
            unreal.log_warning(f"  {c.get_name()}: Location ({loc.x:.1f}, {loc.y:.1f}, {loc.z:.1f})")
    except Exception as e:
        unreal.log_warning(f"Billboard check error: {e}")

    # Check key properties
    unreal.log_warning("")
    unreal.log_warning("=== KEY PROPERTIES ===")
    try:
        unreal.log_warning(f"bCanBeTraced: {cdo.get_editor_property('b_can_be_traced')}")
    except:
        try:
            unreal.log_warning(f"bCanBeTraced: {getattr(cdo, 'b_can_be_traced', 'N/A')}")
        except:
            pass
    try:
        unreal.log_warning(f"bIsActivated: {cdo.get_editor_property('b_is_activated')}")
    except:
        try:
            unreal.log_warning(f"bIsActivated: {getattr(cdo, 'b_is_activated', 'N/A')}")
        except:
            pass
    try:
        unreal.log_warning(f"bIsFogWall: {cdo.get_editor_property('b_is_fog_wall')}")
    except:
        pass

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("VERIFICATION COMPLETE")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    main()
