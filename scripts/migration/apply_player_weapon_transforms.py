# apply_player_weapon_transforms.py
# Applies mesh relative location and rotation to player weapon Blueprint CDOs
# Values extracted from original bp_only Blueprint SCS component settings
#
# Run with:
# UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/apply_player_weapon_transforms.py" -stdout -unattended

import unreal

def log(msg):
    unreal.log_warning(str(msg))

# NOTE: Python unreal.Rotator(a, b, c) = (roll, pitch, yaw) order!
# Original data format from export: "(Pitch, Yaw, Roll)"

PLAYER_WEAPON_TRANSFORMS = {
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01": {
        "location": unreal.Vector(0.0, 0.0, 36.49),
        # Original: P=0, Y=0, R=180 -> Python Rotator(roll, pitch, yaw) = (180, 0, 0)
        "rotation": unreal.Rotator(180.0, 0.0, 0.0),
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02": {
        "location": unreal.Vector(0.0, 0.0, 50.09),
        # Original: P=0, Y=0, R=-130 -> Python Rotator = (-130, 0, 0)
        "rotation": unreal.Rotator(-130.0, 0.0, 0.0),
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword": {
        "location": unreal.Vector(2.23, 0.0, 0.0),
        # Original: P=-0.90, Y=-65.85, R=-4.45 -> Python Rotator = (-4.45, -0.90, -65.85)
        "rotation": unreal.Rotator(-4.45, -0.90, -65.85),
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana": {
        "location": unreal.Vector(0.0, 0.0, -1.64),
        # Original: P=0, Y=-103.24, R=0 -> Python Rotator = (0, 0, -103.24)
        "rotation": unreal.Rotator(0.0, 0.0, -103.24),
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield": {
        "location": unreal.Vector(0.0, 0.0, 0.23),
        # Original: P=0, Y=0, R=-360 (same as 0) -> Python Rotator = (0, 0, 0)
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_BossMace": {
        "location": unreal.Vector(0.0, 0.0, 0.0),
        # Original: P=0, Y=0, R=0 -> Python Rotator = (0, 0, 0)
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_PoisonSword": {
        "location": unreal.Vector(0.0, 0.0, 0.0),
        # Original: P=0, Y=-90, R=0 -> Python Rotator = (0, 0, -90)
        "rotation": unreal.Rotator(0.0, 0.0, -90.0),
    },
    # Base weapon has no offset
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon": {
        "location": unreal.Vector(0.0, 0.0, 0.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
    },
}

def apply_player_weapon_transforms():
    log("")
    log("=" * 70)
    log("APPLYING MESH TRANSFORMS TO PLAYER WEAPON BLUEPRINT CDOs")
    log("=" * 70)
    log("These values restore original Blueprint SCS component transforms")
    log("")

    success_count = 0
    fail_count = 0

    for bp_path, transform_data in PLAYER_WEAPON_TRANSFORMS.items():
        bp_name = bp_path.split("/")[-1]
        log(f"[{bp_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  [SKIP] Could not load Blueprint")
            fail_count += 1
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  [SKIP] No generated class")
            fail_count += 1
            continue

        try:
            cdo = unreal.get_default_object(gen_class)
        except:
            cdo = None

        if not cdo:
            log(f"  [SKIP] Could not get CDO")
            fail_count += 1
            continue

        location = transform_data["location"]
        rotation = transform_data["rotation"]

        # Set the DefaultMeshRelativeLocation property (FVector)
        try:
            cdo.set_editor_property("default_mesh_relative_location", location)
            log(f"  [OK] Set location: ({location.x}, {location.y}, {location.z})")
        except Exception as e:
            log(f"  [ERROR] Could not set location: {e}")
            fail_count += 1
            continue

        # Set the DefaultMeshRelativeRotation property (FRotator)
        try:
            cdo.set_editor_property("default_mesh_relative_rotation", rotation)
            log(f"  [OK] Set rotation: (P={rotation.pitch}, Y={rotation.yaw}, R={rotation.roll})")
        except Exception as e:
            log(f"  [ERROR] Could not set rotation: {e}")
            fail_count += 1
            continue

        # Save the Blueprint
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path)
            log(f"  [SAVED] {bp_name}")
            success_count += 1
        except Exception as e:
            log(f"  [ERROR] Could not save: {e}")
            fail_count += 1

    log("")
    log("=" * 70)
    log(f"COMPLETE: {success_count} succeeded, {fail_count} failed")
    log("=" * 70)


if __name__ == "__main__":
    apply_player_weapon_transforms()
