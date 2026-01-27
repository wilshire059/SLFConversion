# extract_base_stats_bponly.py
# Extract BaseStats TMap from bp_only project to see original structure

import unreal
import json

def extract_base_stats():
    """Extract BaseStats from character class data assets."""

    assets_to_check = [
        "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Quinn",
        "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Manny",
    ]

    result = {}

    unreal.log_warning("=" * 60)
    unreal.log_warning("Extracting BaseStats from bp_only character class assets")
    unreal.log_warning("=" * 60)

    for asset_path in assets_to_check:
        asset = unreal.load_asset(asset_path)
        if not asset:
            unreal.log_warning(f"Could not load: {asset_path}")
            continue

        asset_name = asset_path.split("/")[-1]
        unreal.log_warning(f"\nAsset: {asset_name}")
        unreal.log_warning(f"  Class: {asset.__class__.__name__}")

        # Check if it has base_stats property
        if hasattr(asset, 'base_stats'):
            base_stats = asset.get_editor_property('base_stats')
            unreal.log_warning(f"  BaseStats count: {len(base_stats) if base_stats else 0}")

            result[asset_name] = {}
            if base_stats:
                for stat_class, value in base_stats.items():
                    if stat_class:
                        class_name = stat_class.get_name()
                        class_path = stat_class.get_path_name()
                        unreal.log_warning(f"    {class_name}: {value}")
                        unreal.log_warning(f"      Path: {class_path}")
                        result[asset_name][class_path] = value
            else:
                unreal.log_warning("  BaseStats is EMPTY!")
        else:
            unreal.log_warning(f"  No base_stats property found")

    # Save to cache
    cache_path = "C:/scripts/SLFConversion/migration_cache/base_stats_bponly.json"
    with open(cache_path, 'w') as f:
        json.dump(result, f, indent=2)
    unreal.log_warning(f"\nSaved to: {cache_path}")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    extract_base_stats()
