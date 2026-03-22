import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

log("=" * 70)
log("CHECKING ALL WEAPON STATUS EFFECTS (V2)")
log("=" * 70)

# Check specific weapons we know exist
weapons = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_MidnightSteel",
]

# Also find any other DA_ items
asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
items_path = "/Game/SoulslikeFramework/Data/Items"
assets = asset_registry.get_assets_by_path(items_path, recursive=True)

for asset_data in assets:
    asset_name = str(asset_data.asset_name)
    full_path = str(asset_data.package_name)
    if full_path not in weapons:
        weapons.append(full_path)

log(f"\nFound {len(weapons)} items to check")

for wp_path in weapons:
    asset = unreal.EditorAssetLibrary.load_asset(wp_path)
    if not asset:
        continue

    asset_name = wp_path.split('/')[-1]

    try:
        item_info = asset.get_editor_property("item_information")
        if not item_info:
            continue

        equip_details = item_info.get_editor_property("equipment_details")
        if not equip_details:
            continue

        # Check if this has attack power (i.e., is a weapon)
        attack_power = equip_details.get_editor_property("attack_power")

        status_effects = equip_details.get_editor_property("weapon_status_effect_info")

        # Log if it has attack power or status effects (is weapon-like)
        if attack_power or status_effects:
            log(f"\n{asset_name}:")
            if attack_power:
                log(f"  AttackPower: {attack_power}")
            if status_effects and len(status_effects) > 0:
                log(f"  StatusEffects: {len(status_effects)} entries")
                for key, value in status_effects.items():
                    effect_name = key.get_name() if key else 'None'
                    rank = value.get_editor_property('rank')
                    buildup = value.get_editor_property('buildup_amount')
                    log(f"    - {effect_name}: Rank={rank}, Buildup={buildup}")
            else:
                log(f"  StatusEffects: NONE (no bleed/poison configured)")
    except Exception as e:
        log(f"\n{asset_name}: Error - {e}")

# Write output
with open("C:/scripts/SLFConversion/migration_cache/all_weapon_status_v2.txt", 'w') as f:
    f.write('\n'.join(output))

log("\nDone")
