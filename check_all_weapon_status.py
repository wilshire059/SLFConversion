import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

log("=" * 70)
log("CHECKING ALL WEAPON STATUS EFFECTS")
log("=" * 70)

# Find all weapon data assets
asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
items_path = "/Game/SoulslikeFramework/Data/Items"

# Get all assets in the Items folder
assets = asset_registry.get_assets_by_path(items_path, recursive=True)

for asset_data in assets:
    asset_name = str(asset_data.asset_name)

    # Load the asset
    asset_path = str(asset_data.package_name) + "." + asset_name
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)

    if not asset:
        continue

    try:
        # Check if it has ItemInformation (is a PDA_Item)
        item_info = asset.get_editor_property("item_information")
        if not item_info:
            continue

        equip_details = item_info.get_editor_property("equipment_details")
        if not equip_details:
            continue

        status_effects = equip_details.get_editor_property("weapon_status_effect_info")

        # Only log weapons with status effects OR weapons (check category)
        category = item_info.get_editor_property("category")
        if category:
            cat_enum = category.get_editor_property("category")
            # Check if it's a weapon (category 6 = Weapons)
            if cat_enum and int(cat_enum) == 6:  # Weapons
                log(f"\n{asset_name}:")
                if status_effects and len(status_effects) > 0:
                    log(f"  StatusEffects: {len(status_effects)} entries")
                    for key, value in status_effects.items():
                        log(f"    - {key.get_name() if key else 'None'}: Rank={value.get_editor_property('rank')}, Buildup={value.get_editor_property('buildup_amount')}")
                else:
                    log(f"  StatusEffects: NONE")
    except Exception as e:
        pass

# Write output
with open("C:/scripts/SLFConversion/migration_cache/all_weapon_status.txt", 'w') as f:
    f.write('\n'.join(output))

log("\nDone")
