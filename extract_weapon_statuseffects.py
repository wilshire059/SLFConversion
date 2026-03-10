"""
Extract WeaponStatusEffectInfo from bp_only backup.
Run on bp_only project first!
Uses EXACT Blueprint GUID-suffixed property names found in JSON exports.
"""

import unreal
import json

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/weapon_statuseffect_data.json"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

# Blueprint struct property names with GUIDs
EQUIPMENT_DETAILS_PROP = "EquipmentDetails_26_1D4D712C4D1C866608D5FB86D442615C"
WEAPON_STATUS_EFFECT_PROP = "WeaponStatusEffectInfo_118_50E270B04A50041AE79D29AC86F423A2"
RANK_PROP = "Rank_4_75FF88964E30AA02AAA1BFAD2A0FFD27"  # Common GUID for Rank
BUILDUP_PROP = "BuildupAmount_8_EDF77EA344E14C05C59EA89AE44F0CEC"  # Common GUID for BuildupAmount

def extract():
    log("=" * 60)
    log("Extracting Weapon Status Effect Data from bp_only")
    log("Using GUID-suffixed property names")
    log("=" * 60)

    data = {}

    # Find all item data assets
    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    filter = unreal.ARFilter(
        package_paths=["/Game/SoulslikeFramework/Data/Items"],
        recursive_paths=True
    )
    assets = registry.get_assets(filter)

    log(f"\nFound {len(assets)} assets to check")

    for asset_data in assets:
        path = str(asset_data.package_name)
        name = str(asset_data.asset_name)

        try:
            asset = unreal.EditorAssetLibrary.load_asset(path)
            if not asset:
                continue

            # Get ItemInformation (no GUID for top-level PDA property)
            item_info = None
            try:
                item_info = asset.get_editor_property("ItemInformation")
            except:
                continue

            if not item_info:
                continue

            # Get EquipmentDetails using GUID name
            equip_details = None
            try:
                equip_details = item_info.get_editor_property(EQUIPMENT_DETAILS_PROP)
            except:
                continue

            if not equip_details:
                continue

            # Get WeaponStatusEffectInfo using GUID name
            status_info = None
            try:
                status_info = equip_details.get_editor_property(WEAPON_STATUS_EFFECT_PROP)
            except:
                continue

            if not status_info or len(status_info) == 0:
                continue

            log(f"\n{name}: Found {len(status_info)} status effects")
            item_data = {"effects": []}

            for effect_asset, application in status_info.items():
                if effect_asset:
                    effect_name = effect_asset.get_name() if hasattr(effect_asset, 'get_name') else str(effect_asset)
                    effect_path = effect_asset.get_path_name() if hasattr(effect_asset, 'get_path_name') else str(effect_asset)

                    # Get Rank using GUID name
                    rank = 1
                    try:
                        rank = int(application.get_editor_property(RANK_PROP))
                    except:
                        # Try without GUID
                        try:
                            rank = int(application.get_editor_property("rank"))
                        except:
                            pass

                    # Get BuildupAmount using GUID name
                    buildup = 50.0
                    try:
                        buildup = float(application.get_editor_property(BUILDUP_PROP))
                    except:
                        # Try without GUID
                        try:
                            buildup = float(application.get_editor_property("buildup_amount"))
                        except:
                            pass

                    log(f"  - {effect_name}: Rank={rank}, Buildup={buildup}")
                    item_data["effects"].append({
                        "effect_path": effect_path,
                        "effect_name": effect_name,
                        "rank": rank,
                        "buildup_amount": buildup
                    })

            if item_data["effects"]:
                data[path] = item_data

        except Exception as e:
            log(f"  Error processing {name}: {e}")

    log(f"\n\nTotal weapons with status effects: {len(data)}")

    # Save to JSON
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(data, f, indent=2)
    log(f"Saved to: {OUTPUT_FILE}")

# Run
extract()

# Save log
LOG_FILE = "C:/scripts/SLFConversion/migration_cache/weapon_statuseffect_extract_log.txt"
with open(LOG_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog: {LOG_FILE}")
