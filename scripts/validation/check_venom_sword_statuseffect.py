"""
Check if venom sword has WeaponStatusEffectInfo configured.
"""

import unreal

LOG_FILE = "C:/scripts/SLFConversion/migration_cache/venom_sword_check.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

def check():
    log("=" * 60)
    log("Checking Venom Sword Status Effect Configuration")
    log("=" * 60)

    # Find venom weapon data assets
    weapon_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
        "/Game/SoulslikeFramework/Data/Items/Weapons/DA_Sword01",
        "/Game/SoulslikeFramework/Data/Items/DA_VenomSword",
        "/Game/SoulslikeFramework/Data/Items/Weapons/DA_VenomSword",
    ]

    found = False
    for path in weapon_paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            found = True
            log(f"\nFound: {path}")
            log(f"  Class: {asset.get_class().get_name()}")

            # Get ItemInformation
            try:
                item_info = asset.get_editor_property("item_information")
                if item_info:
                    log(f"  ItemInformation: OK")

                    # Get EquipmentDetails
                    try:
                        equip_details = item_info.get_editor_property("equipment_details")
                        if equip_details:
                            log(f"  EquipmentDetails: OK")

                            # Get WeaponStatusEffectInfo
                            try:
                                status_effects = equip_details.get_editor_property("weapon_status_effect_info")
                                if status_effects:
                                    log(f"  WeaponStatusEffectInfo: {len(status_effects)} entries")
                                    for key, value in status_effects.items():
                                        log(f"    {key.get_name() if key else 'None'}: Rank={value.rank}, BuildupAmount={value.buildup_amount}")
                                else:
                                    log(f"  WeaponStatusEffectInfo: EMPTY or None")
                            except Exception as e:
                                log(f"  WeaponStatusEffectInfo: ERROR - {e}")
                        else:
                            log(f"  EquipmentDetails: None")
                    except Exception as e:
                        log(f"  EquipmentDetails: ERROR - {e}")
                else:
                    log(f"  ItemInformation: None")
            except Exception as e:
                log(f"  ItemInformation: ERROR - {e}")

    if not found:
        log("\nNo weapon assets found at expected paths.")
        log("Searching for any asset with 'Venom' or 'Sword' in name...")

        # Search for weapon assets
        registry = unreal.AssetRegistryHelpers.get_asset_registry()
        filter = unreal.ARFilter(
            package_paths=["/Game/SoulslikeFramework/Data/Items"],
            recursive_paths=True
        )
        assets = registry.get_assets(filter)

        for asset_data in assets:
            name = str(asset_data.asset_name)
            if "sword" in name.lower() or "venom" in name.lower() or "weapon" in name.lower():
                log(f"  Found: {asset_data.package_name}")

# Run
check()

# Save log
with open(LOG_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog: {LOG_FILE}")
