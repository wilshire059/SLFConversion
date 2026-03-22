"""
Resave data assets to update serialized struct type names.
This should fix the F-prefix mismatch issue.
"""
import unreal

print("=" * 60)
print("RESAVING DATA ASSETS")
print("=" * 60)

eal = unreal.EditorAssetLibrary

# Assets that have struct properties with the mismatch issue
assets_to_resave = [
    # BaseCharacter data (MeshData - FSLFSkeletalMeshData)
    "/Game/SoulslikeFramework/Data/BaseCharacters/DA_QuinnMeshDefault",
    "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData",

    # Stats (StatInfo - FStatInfo)
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Arcane",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Endurance",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Faith",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Intelligence",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Mind",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/B_FP",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/B_HP",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/B_Stamina",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Fire",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Frost",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Holy",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Lightning",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Magic",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Physical",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Fire",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Frost",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Holy",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Lightning",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Magic",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Physical",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Focus",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Immunity",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Robustness",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Vitality",
    "/Game/SoulslikeFramework/Data/Stats/Misc/B_Discovery",
    "/Game/SoulslikeFramework/Data/Stats/Misc/B_Weight",
    "/Game/SoulslikeFramework/Data/Stats/_Backend/B_IncantationPower",
    "/Game/SoulslikeFramework/Data/Stats/_Backend/B_Poise",
    "/Game/SoulslikeFramework/Data/Stats/_Backend/B_Stance",
    "/Game/SoulslikeFramework/Data/Stats/_Backend/B_StatusEffectBuildup",

    # Radar (CardinalData - FSLFCardinalData)
    "/Game/SoulslikeFramework/Widgets/Radar/W_Radar",
    "/Game/SoulslikeFramework/Widgets/Radar/W_Radar_Cardinal",

    # Items (ItemInfo - FSLFItemInfo)
    "/Game/SoulslikeFramework/Data/Items/DA_Apple",
    "/Game/SoulslikeFramework/Data/Items/DA_BossMace",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_HealthFlask",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword02",

    # Day/Night (TimeInfo - FSLFDayNightInfo)
    "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager",
    "/Game/SoulslikeFramework/Data/PDA_DayNight",
]

resaved_count = 0
failed_count = 0

for asset_path in assets_to_resave:
    if eal.does_asset_exist(asset_path):
        try:
            # Load the asset
            asset = eal.load_asset(asset_path)
            if asset:
                # Mark it dirty and save
                # Use save_asset which handles the resave
                if eal.save_asset(asset_path, only_if_is_dirty=False):
                    print(f"[RESAVED] {asset_path}")
                    resaved_count += 1
                else:
                    print(f"[SAVE FAILED] {asset_path}")
                    failed_count += 1
            else:
                print(f"[LOAD FAILED] {asset_path}")
                failed_count += 1
        except Exception as e:
            print(f"[ERROR] {asset_path}: {e}")
            failed_count += 1
    else:
        print(f"[NOT FOUND] {asset_path}")

print("")
print("=" * 60)
print(f"RESAVE COMPLETE: {resaved_count} resaved, {failed_count} failed")
print("=" * 60)

# Now test loading the character to see if MeshData loads
print("")
print("TESTING CHARACTER MESH DATA...")
bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
if eal.does_asset_exist(bp_path):
    bp = eal.load_asset(bp_path)
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = gen_class.get_default_object()
            if cdo:
                # Try to get the mesh component
                mesh = cdo.get_editor_property('mesh')
                print(f"Character Mesh: {mesh}")
                if mesh:
                    sk_mesh = mesh.get_editor_property('skeletal_mesh')
                    print(f"  SkeletalMesh: {sk_mesh}")
