# verify_weapon_cdos.py
# Focused check of weapon CDO properties in SLFConversion

import unreal

def log(msg):
    unreal.log_warning(str(msg))

WEAPON_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

def verify_weapons():
    log("=" * 70)
    log("WEAPON CDO VERIFICATION")
    log("=" * 70)

    for bp_path in WEAPON_PATHS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n=== {bp_name} ===")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load Blueprint")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  ERROR: No generated class")
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            log(f"  ERROR: No CDO")
            continue

        # Check DefaultMeshRelativeLocation
        try:
            loc = cdo.get_editor_property("default_mesh_relative_location")
            log(f"  DefaultMeshRelativeLocation: ({loc.x:.4f}, {loc.y:.4f}, {loc.z:.4f})")
        except Exception as e:
            log(f"  DefaultMeshRelativeLocation ERROR: {e}")

        # Check DefaultMeshRelativeRotation
        try:
            rot = cdo.get_editor_property("default_mesh_relative_rotation")
            log(f"  DefaultMeshRelativeRotation: (P={rot.pitch:.4f}, Y={rot.yaw:.4f}, R={rot.roll:.4f})")
        except Exception as e:
            log(f"  DefaultMeshRelativeRotation ERROR: {e}")

        # Check DefaultWeaponMesh
        try:
            mesh_ptr = cdo.get_editor_property("default_weapon_mesh")
            if mesh_ptr:
                if hasattr(mesh_ptr, 'is_null') and mesh_ptr.is_null():
                    log(f"  DefaultWeaponMesh: NULL (is_null=True)")
                elif hasattr(mesh_ptr, 'load_synchronous'):
                    loaded = mesh_ptr.load_synchronous()
                    if loaded:
                        log(f"  DefaultWeaponMesh: {loaded.get_name()}")
                    else:
                        log(f"  DefaultWeaponMesh: Could not load")
                else:
                    log(f"  DefaultWeaponMesh: {mesh_ptr}")
            else:
                log(f"  DefaultWeaponMesh: None")
        except Exception as e:
            log(f"  DefaultWeaponMesh ERROR: {e}")

        # Check DefaultAttachmentRotationOffset
        try:
            off = cdo.get_editor_property("default_attachment_rotation_offset")
            log(f"  DefaultAttachmentRotationOffset: (P={off.pitch:.4f}, Y={off.yaw:.4f}, R={off.roll:.4f})")
        except Exception as e:
            log(f"  DefaultAttachmentRotationOffset ERROR: {e}")

        # Check WeaponMesh component
        try:
            weapon_mesh = cdo.get_editor_property("weapon_mesh")
            if weapon_mesh:
                log(f"  WeaponMesh Component: {weapon_mesh.get_name()}")
                try:
                    sm = weapon_mesh.get_editor_property("static_mesh")
                    log(f"    StaticMesh: {sm.get_name() if sm else 'None'}")
                except:
                    log(f"    StaticMesh: N/A")
                try:
                    rl = weapon_mesh.get_editor_property("relative_location")
                    log(f"    RelativeLocation: ({rl.x:.4f}, {rl.y:.4f}, {rl.z:.4f})")
                except:
                    log(f"    RelativeLocation: N/A")
                try:
                    rr = weapon_mesh.get_editor_property("relative_rotation")
                    log(f"    RelativeRotation: (P={rr.pitch:.4f}, Y={rr.yaw:.4f}, R={rr.roll:.4f})")
                except:
                    log(f"    RelativeRotation: N/A")
            else:
                log(f"  WeaponMesh Component: None")
        except Exception as e:
            log(f"  WeaponMesh Component ERROR: {e}")

    log("\n" + "=" * 70)
    log("VERIFICATION COMPLETE")
    log("=" * 70)

if __name__ == "__main__":
    verify_weapons()
