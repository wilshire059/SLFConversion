# debug_player_weapon_cdo.py
# Check if transforms are set on player weapon CDOs

import unreal

def log(msg):
    unreal.log_warning(str(msg))

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

def debug_cdos():
    log("=" * 70)
    log("PLAYER WEAPON CDO VALUES")
    log("=" * 70)

    for bp_path in WEAPONS:
        bp_name = bp_path.split("/")[-1]
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            continue
        gen_class = bp.generated_class()
        if not gen_class:
            continue
        try:
            cdo = unreal.get_default_object(gen_class)
        except:
            continue

        log(f"[{bp_name}]")
        try:
            loc = cdo.get_editor_property("default_mesh_relative_location")
            log(f"  Location: ({loc.x:.2f}, {loc.y:.2f}, {loc.z:.2f})")
        except Exception as e:
            log(f"  Location ERROR: {e}")
        try:
            rot = cdo.get_editor_property("default_mesh_relative_rotation")
            log(f"  Rotation: (P={rot.pitch:.2f}, Y={rot.yaw:.2f}, R={rot.roll:.2f})")
        except Exception as e:
            log(f"  Rotation ERROR: {e}")
        try:
            mesh = cdo.get_editor_property("default_weapon_mesh")
            log(f"  Mesh: {mesh.get_name() if mesh else 'None'}")
        except:
            pass

if __name__ == "__main__":
    debug_cdos()
