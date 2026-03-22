# verify_ai_vs_player_weapon.py
# Compare AI weapon setup (which works) with player weapon setup

import unreal

def log(msg):
    unreal.log_warning(str(msg))

def compare():
    log("=" * 70)
    log("COMPARING AI vs PLAYER WEAPON SETUP")
    log("=" * 70)

    weapons = [
        ("/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword", "AI Weapon"),
        ("/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01", "Player Weapon"),
    ]

    for bp_path, label in weapons:
        bp_name = bp_path.split("/")[-1]
        log(f"\n[{label}] - {bp_name}")
        log("-" * 50)

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  ERROR: No generated class")
            continue

        # CDO values
        try:
            cdo = unreal.get_default_object(gen_class)
            log(f"  CDO Properties:")

            # Check all relevant properties
            try:
                loc = cdo.get_editor_property("default_mesh_relative_location")
                log(f"    default_mesh_relative_location: ({loc.x:.2f}, {loc.y:.2f}, {loc.z:.2f})")
            except Exception as e:
                log(f"    default_mesh_relative_location: ERROR - {e}")

            try:
                rot = cdo.get_editor_property("default_mesh_relative_rotation")
                log(f"    default_mesh_relative_rotation: (P={rot.pitch:.2f}, Y={rot.yaw:.2f}, R={rot.roll:.2f})")
            except Exception as e:
                log(f"    default_mesh_relative_rotation: ERROR - {e}")

            try:
                mesh = cdo.get_editor_property("default_weapon_mesh")
                mesh_name = mesh.get_name() if mesh else "None"
                log(f"    default_weapon_mesh: {mesh_name}")
            except Exception as e:
                log(f"    default_weapon_mesh: ERROR - {e}")

            try:
                attachment_offset = cdo.get_editor_property("default_attachment_rotation_offset")
                log(f"    default_attachment_rotation_offset: (P={attachment_offset.pitch:.2f}, Y={attachment_offset.yaw:.2f}, R={attachment_offset.roll:.2f})")
            except Exception as e:
                log(f"    default_attachment_rotation_offset: ERROR - {e}")

        except Exception as e:
            log(f"  CDO error: {e}")

if __name__ == "__main__":
    compare()
