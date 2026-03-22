#!/usr/bin/env python3
"""Verify weapon runtime behavior by spawning with BeginPlay call"""

import unreal

def log(msg):
    unreal.log_warning(str(msg))

def verify_weapon():
    log("=" * 70)
    log("WEAPON RUNTIME VERIFICATION")
    log("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01"
    bp_name = bp_path.split("/")[-1]

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load Blueprint")
        return

    gen_class = bp.generated_class()
    cdo = unreal.get_default_object(gen_class)

    log(f"\n=== CDO Properties ===")
    log(f"DefaultWeaponMesh: {cdo.get_editor_property('default_weapon_mesh')}")
    log(f"DefaultMeshRelativeLocation: {cdo.get_editor_property('default_mesh_relative_location')}")
    log(f"DefaultMeshRelativeRotation: {cdo.get_editor_property('default_mesh_relative_rotation')}")

    # Check FRotator.IsZero behavior
    test_rot = cdo.get_editor_property('default_mesh_relative_rotation')
    log(f"\nFRotator values: pitch={test_rot.pitch}, yaw={test_rot.yaw}, roll={test_rot.roll}")
    log(f"Is roll near zero? {abs(test_rot.roll) < 0.001}")
    log(f"Is roll effectively 180? {abs(test_rot.roll - 180.0) < 0.001 or abs(test_rot.roll + 180.0) < 0.001}")

    # Get the actual FRotator::IsZero behavior
    test_loc = cdo.get_editor_property('default_mesh_relative_location')
    log(f"\nFVector values: x={test_loc.x}, y={test_loc.y}, z={test_loc.z}")
    log(f"Is Z effectively zero? {abs(test_loc.z) < 0.001}")

    # Also check shield
    log("\n" + "=" * 70)
    shield_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield"
    shield_bp = unreal.EditorAssetLibrary.load_asset(shield_path)
    shield_cdo = unreal.get_default_object(shield_bp.generated_class())

    log(f"\n=== Shield CDO Properties ===")
    shield_rot = shield_cdo.get_editor_property('default_mesh_relative_rotation')
    log(f"DefaultMeshRelativeRotation: pitch={shield_rot.pitch}, yaw={shield_rot.yaw}, roll={shield_rot.roll}")
    log(f"Roll is -360? {abs(shield_rot.roll + 360.0) < 0.001}")
    log(f"Normalized roll: {shield_rot.roll % 360}")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    verify_weapon()
