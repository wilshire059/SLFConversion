# apply_ai_weapon_transforms.py
# Applies mesh relative location and rotation to AI weapon Blueprint CDOs
# These values were lost during reparenting from Blueprint SCS to C++ components
#
# Run with:
# UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/apply_ai_weapon_transforms.py" -stdout -unattended

import unreal

def log(msg):
    unreal.log_warning(str(msg))

# Mapping of AI weapon Blueprint paths to mesh transform data
# Values extracted from original Blueprint SCS component settings (weapon_mesh_bp_only.json)
# NOTE: Python unreal.Rotator(a, b, c) = (roll, pitch, yaw) order!
# Original data format from export: "(Pitch, Yaw, Roll)"
AI_WEAPON_TRANSFORMS = {
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword": {
        "location": unreal.Vector(0.0, 0.0, 43.66),
        # Original: (0, 0, -130) = Pitch=0, Yaw=0, Roll=-130
        # Python Rotator: (roll, pitch, yaw) = (-130, 0, 0)
        "rotation": unreal.Rotator(-130.0, 0.0, 0.0),
    },
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword": {
        "location": unreal.Vector(0.0, 0.0, -13.46),
        # Original: (0, -43.43, 0) = Pitch=0, Yaw=-43.43, Roll=0
        # Python Rotator: (roll, pitch, yaw) = (0, 0, -43.43)
        "rotation": unreal.Rotator(0.0, 0.0, -43.43),
    },
    # Base AI weapon has no offset
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_AI_Weapon": {
        "location": unreal.Vector(0.0, 0.0, 0.0),
        "rotation": unreal.Rotator(0.0, 0.0, 0.0),
    },
}

def apply_ai_weapon_transforms():
    log("")
    log("=" * 70)
    log("APPLYING MESH TRANSFORMS TO AI WEAPON BLUEPRINT CDOs")
    log("=" * 70)
    log("These values restore original Blueprint SCS component transforms")
    log("")

    success_count = 0
    fail_count = 0

    for bp_path, transform_data in AI_WEAPON_TRANSFORMS.items():
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
            log(f"  [OK] Set rotation: (Pitch={rotation.pitch}, Yaw={rotation.yaw}, Roll={rotation.roll})")
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
    apply_ai_weapon_transforms()
