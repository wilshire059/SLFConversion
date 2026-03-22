"""Check what weapons enemies use and if they have status effects configured."""

import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

log("=" * 70)
log("CHECKING ENEMY WEAPON STATUS EFFECTS")
log("=" * 70)

# Check DA_BossMace - this was in our extraction as having Burn
log("\n--- DA_BossMace (should have Burn) ---")
boss_mace = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Items/DA_BossMace")
if boss_mace:
    try:
        item_info = boss_mace.get_editor_property("item_information")
        if item_info:
            equip_details = item_info.get_editor_property("equipment_details")
            if equip_details:
                status_effects = equip_details.get_editor_property("weapon_status_effect_info")
                if status_effects and len(status_effects) > 0:
                    log(f"DA_BossMace has {len(status_effects)} status effects:")
                    for key, value in status_effects.items():
                        log(f"  - {key.get_name() if key else 'None'}: Rank={value.get_editor_property('rank')}, Buildup={value.get_editor_property('buildup_amount')}")
                else:
                    log("DA_BossMace: NO STATUS EFFECTS CONFIGURED!")
    except Exception as e:
        log(f"Error checking DA_BossMace: {e}")
else:
    log("Could not load DA_BossMace")

# Check if there are other enemy weapon data assets
log("\n--- Searching for enemy weapon assets ---")
enemy_weapon_candidates = [
    "/Game/SoulslikeFramework/Data/Items/DA_EnemySword",
    "/Game/SoulslikeFramework/Data/Items/DA_Enemy_Sword",
    "/Game/SoulslikeFramework/Data/Items/DA_EnemyWeapon",
    "/Game/SoulslikeFramework/Data/Items/DA_DemoEnemyWeapon",
]

for path in enemy_weapon_candidates:
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset:
        log(f"Found: {path}")

# List all weapon data assets
log("\n--- All weapon data assets ---")
all_items = unreal.EditorAssetLibrary.list_assets("/Game/SoulslikeFramework/Data/Items", recursive=True, include_folder=False)
weapon_assets = []
for item_path in all_items:
    if item_path.endswith("_C"):
        continue
    try:
        asset = unreal.EditorAssetLibrary.load_asset(item_path)
        if asset:
            try:
                item_info = asset.get_editor_property("item_information")
                if item_info:
                    category = item_info.get_editor_property("category")
                    if category == 6:  # Weapons category
                        weapon_assets.append(item_path)
            except:
                pass
    except:
        pass

log(f"Found {len(weapon_assets)} weapon data assets")

# Check each for status effects
for wp_path in weapon_assets:
    wp = unreal.EditorAssetLibrary.load_asset(wp_path)
    if wp:
        try:
            item_info = wp.get_editor_property("item_information")
            if item_info:
                equip_details = item_info.get_editor_property("equipment_details")
                if equip_details:
                    status_effects = equip_details.get_editor_property("weapon_status_effect_info")
                    if status_effects and len(status_effects) > 0:
                        name = wp_path.split("/")[-1]
                        log(f"  HAS STATUS EFFECTS: {name}")
                        for key, value in status_effects.items():
                            log(f"    - {key.get_name() if key else 'None'}: Rank={value.get_editor_property('rank')}, Buildup={value.get_editor_property('buildup_amount')}")
        except:
            pass

# Write output
with open("C:/scripts/SLFConversion/migration_cache/enemy_weapon_status.txt", 'w') as f:
    f.write('\n'.join(output))

log("\nDone - see migration_cache/enemy_weapon_status.txt")
