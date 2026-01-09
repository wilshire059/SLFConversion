# apply_weapon_meshes.py
# Assigns DefaultWeaponMesh property to weapon Blueprint CDOs
# This property persists to Blueprint and is applied in BeginPlay
#
# Run with:
# UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/apply_weapon_meshes.py" -stdout -unattended

import unreal

def log(msg):
    unreal.log_warning(str(msg))

# Mapping of weapon Blueprint paths to mesh paths
WEAPON_MESH_MAP = {
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01":
        "/Game/SoulslikeFramework/Meshes/SM/Sword/SM_ExampleSword01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02":
        "/Game/SoulslikeFramework/Meshes/SM/Sword_02/SM_ExampleSword02",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword":
        "/Game/SoulslikeFramework/Meshes/SM/Greatsword/SM_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana":
        "/Game/SoulslikeFramework/Meshes/SM/Katana/SM_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield":
        "/Game/SoulslikeFramework/Meshes/SM/Shield/SM_Shield",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_BossMace":
        "/Game/SoulslikeFramework/Meshes/SM/Greatsword/SM_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword":
        "/Game/SoulslikeFramework/Meshes/SM/Greatsword/SM_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword":
        "/Game/SoulslikeFramework/Meshes/SM/Sword/SM_ExampleSword01",
}

def apply_weapon_meshes():
    log("")
    log("=" * 70)
    log("APPLYING DEFAULT WEAPON MESHES TO BLUEPRINT CDOs")
    log("=" * 70)

    success_count = 0
    fail_count = 0

    for bp_path, mesh_path in WEAPON_MESH_MAP.items():
        bp_name = bp_path.split("/")[-1]
        log(f"[{bp_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  [SKIP] Could not load Blueprint")
            fail_count += 1
            continue

        mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
        if not mesh:
            log(f"  [WARN] Could not load mesh: {mesh_path}")
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

        # Set the DefaultWeaponMesh property (TSoftObjectPtr<UStaticMesh>)
        try:
            cdo.set_editor_property("default_weapon_mesh", mesh)
            log(f"  [OK] Set DefaultWeaponMesh: {mesh_path.split(chr(47))[-1]}")
        except Exception as e:
            log(f"  [ERROR] Could not set property: {e}")
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
    apply_weapon_meshes()
