"""Check what equipment/weapons enemies have that would apply status effects."""

import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

log("=" * 70)
log("CHECKING ENEMY EQUIPMENT FOR STATUS EFFECTS")
log("=" * 70)

# Check B_Soulslike_Enemy for default equipped weapons
log("\n--- Checking Enemy Character Defaults ---")

enemy_bp_paths = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
]

for bp_path in enemy_bp_paths:
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if bp:
        name = bp_path.split("/")[-1]
        log(f"\n{name}:")
        try:
            # Get CDO
            gen_class = bp.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)

                # Check for DefaultWeapon property (common pattern for enemy characters)
                try:
                    default_weapon = cdo.get_editor_property("default_weapon")
                    if default_weapon:
                        log(f"  DefaultWeapon: {default_weapon.get_name()}")
                except:
                    pass

                # Check for weapon in right hand equipment slot
                try:
                    right_hand = cdo.get_editor_property("right_hand_weapon")
                    if right_hand:
                        log(f"  RightHandWeapon: {right_hand.get_name()}")
                except:
                    pass

                # Check BaseCharacterInfo for equipment
                try:
                    char_info = cdo.get_editor_property("character_info")
                    if char_info:
                        log(f"  CharacterInfo: {char_info.get_name()}")
                except:
                    pass

                # Check AI data
                try:
                    ai_data = cdo.get_editor_property("ai_data")
                    if ai_data:
                        log(f"  AIData: {ai_data.get_name()}")
                except:
                    pass

        except Exception as e:
            log(f"  Error: {e}")

# Check if there's a DA_EnemyCharacterInfo or similar data asset
log("\n--- Searching for Enemy Character Data Assets ---")
enemy_data_paths = [
    "/Game/SoulslikeFramework/Data/BaseCharacters/",
    "/Game/SoulslikeFramework/Data/Enemies/",
    "/Game/SoulslikeFramework/Data/AI/",
]

for data_path in enemy_data_paths:
    if unreal.EditorAssetLibrary.does_directory_exist(data_path):
        assets = unreal.EditorAssetLibrary.list_assets(data_path, recursive=True, include_folder=False)
        log(f"\nAssets in {data_path}:")
        for asset_path in assets:
            if "_C" not in asset_path:  # Skip generated classes
                log(f"  {asset_path.split('/')[-1]}")

# Check what the demo level enemies use
log("\n--- Demo Level Check ---")
# Look at the demo showcase level for enemy actors
showcase_blueprints = unreal.EditorAssetLibrary.list_assets("/Game/SoulslikeFramework/Demo/ShowcaseRoom/", recursive=True, include_folder=False)
for bp_path in showcase_blueprints:
    if "Enemy" in bp_path and "_C" not in bp_path:
        log(f"Found showcase enemy BP: {bp_path.split('/')[-1]}")

# Write output
with open("C:/scripts/SLFConversion/migration_cache/enemy_equipment.txt", 'w') as f:
    f.write('\n'.join(output))

log("\nDone")
