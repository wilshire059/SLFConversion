# apply_player_weapon_transforms_v2.py
# Apply correct weapon transforms and mesh paths extracted from bp_only

import unreal

def log(msg):
    unreal.log_warning(str(msg))

# Correct data extracted from bp_only backup
# unreal.Rotator constructor is (roll, pitch, yaw) in Python!
PLAYER_WEAPON_DATA = {
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01": {
        "location": unreal.Vector(0.0, 0.0, 36.49),
        "rotation": unreal.Rotator(180.0, 0.0, 0.0),  # Roll=180, P=0, Y=0
        "mesh": "/Game/SoulslikeFramework/Meshes/SM/Sword/SM_ExampleSword01"
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02": {
        "location": unreal.Vector(0.0, 0.0, 50.09),
        "rotation": unreal.Rotator(-130.0, 0.0, 0.0),  # Roll=-130, P=0, Y=0
        "mesh": "/Game/SoulslikeFramework/Meshes/SM/Sword_02/SM_ExampleSword02"
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword": {
        "location": unreal.Vector(2.23, 0.0, 0.0),
        "rotation": unreal.Rotator(-4.45, -0.90, -65.85),  # R=-4.45, P=-0.90, Y=-65.85
        "mesh": "/Game/SoulslikeFramework/Meshes/SM/Greatsword/SM_Greatsword"
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana": {
        "location": unreal.Vector(0.0, 0.0, -1.64),
        "rotation": unreal.Rotator(0.0, 0.0, -103.24),  # R=0, P=0, Y=-103.24
        "mesh": "/Game/SoulslikeFramework/Meshes/SM/Katana/SM_Katana"
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield": {
        "location": unreal.Vector(0.0, 0.0, 0.23),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),  # All zero
        "mesh": "/Game/SoulslikeFramework/Meshes/SM/Shield/SM_Shield"
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_BossMace": {
        "location": unreal.Vector(0.0, 0.0, 0.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),  # All zero
        "mesh": "/Game/SoulslikeFramework/Demo/ParagonGrux/Characters/Heroes/Grux/Meshes/SM_Grux_Weapon"
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_PoisonSword": {
        "location": unreal.Vector(0.0, 0.0, 0.0),
        "rotation": unreal.Rotator(0.0, 0.0, -90.0),  # R=0, P=0, Y=-90
        "mesh": "/Game/SoulslikeFramework/Meshes/SM/PoisonSword/SM_PoisonSword"
    },
}

def apply_transforms():
    log("=" * 70)
    log("APPLYING PLAYER WEAPON TRANSFORMS AND MESHES")
    log("=" * 70)

    success_count = 0
    fail_count = 0

    for bp_path, data in PLAYER_WEAPON_DATA.items():
        bp_name = bp_path.split("/")[-1]
        log(f"\n[{bp_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load Blueprint")
            fail_count += 1
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  ERROR: No generated class")
            fail_count += 1
            continue

        try:
            cdo = unreal.get_default_object(gen_class)

            # Apply location
            cdo.set_editor_property("default_mesh_relative_location", data["location"])
            log(f"  Set location: ({data['location'].x:.2f}, {data['location'].y:.2f}, {data['location'].z:.2f})")

            # Apply rotation
            cdo.set_editor_property("default_mesh_relative_rotation", data["rotation"])
            log(f"  Set rotation: (P={data['rotation'].pitch:.2f}, Y={data['rotation'].yaw:.2f}, R={data['rotation'].roll:.2f})")

            # Load and apply mesh
            mesh = unreal.EditorAssetLibrary.load_asset(data["mesh"])
            if mesh:
                cdo.set_editor_property("default_weapon_mesh", mesh)
                log(f"  Set mesh: {mesh.get_name()}")
            else:
                log(f"  WARNING: Could not load mesh {data['mesh']}")

            # Save
            unreal.EditorAssetLibrary.save_asset(bp_path)
            log(f"  SAVED")
            success_count += 1

        except Exception as e:
            log(f"  ERROR: {e}")
            fail_count += 1

    log("\n" + "=" * 70)
    log(f"COMPLETE: {success_count} succeeded, {fail_count} failed")
    log("=" * 70)

if __name__ == "__main__":
    apply_transforms()
