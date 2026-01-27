"""
Extract WeaponStatusEffectInfo from bp_only backup.
Run on bp_only project first!
"""

import unreal
import json

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/weapon_statuseffect_data.json"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

def extract():
    log("=" * 60)
    log("Extracting Weapon Status Effect Data from bp_only")
    log("=" * 60)

    data = {}

    # Find all item data assets
    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    filter = unreal.ARFilter(
        package_paths=["/Game/SoulslikeFramework/Data/Items"],
        recursive_paths=True
    )
    assets = registry.get_assets(filter)

    for asset_data in assets:
        path = str(asset_data.package_name)
        asset = unreal.EditorAssetLibrary.load_asset(path)

        if not asset:
            continue

        name = asset.get_name()

        # Check if it's an item with equipment details
        try:
            item_info = asset.get_editor_property("item_information")
            if not item_info:
                continue

            equip_details = item_info.get_editor_property("equipment_details")
            if not equip_details:
                continue

            # Get status effect info
            status_effects = equip_details.get_editor_property("weapon_status_effect_info")
            if not status_effects or len(status_effects) == 0:
                continue

            log(f"\n{name}: {len(status_effects)} status effects")
            item_data = {"effects": []}

            for effect_asset, application in status_effects.items():
                if effect_asset:
                    effect_path = effect_asset.get_path_name()
                    rank = application.rank if hasattr(application, 'rank') else 1
                    buildup = application.buildup_amount if hasattr(application, 'buildup_amount') else 10.0

                    log(f"  {effect_asset.get_name()}: Rank={rank}, Buildup={buildup}")
                    item_data["effects"].append({
                        "effect_path": effect_path,
                        "rank": rank,
                        "buildup_amount": buildup
                    })

            data[path] = item_data

        except Exception as e:
            pass

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
