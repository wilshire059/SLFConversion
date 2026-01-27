"""
Extract BaseStats from bp_only character class data assets.

These are the stat values that get applied when a character class is selected.
Example: DA_Manny might have Vigor=10, Strength=12, etc.
"""
import unreal
import json
import os

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/character_basestats.json"

# Character class data assets to extract from
CHARACTER_CLASS_ASSETS = [
    "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Manny",
    "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Quinn",
]

def extract_basestats():
    """Extract BaseStats from character class data assets."""
    print("=" * 80)
    print("EXTRACTING CHARACTER CLASS BASESTATS")
    print("=" * 80)

    results = {}

    for asset_path in CHARACTER_CLASS_ASSETS:
        print(f"\n--- Processing: {asset_path} ---")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            print(f"  ERROR: Could not load {asset_path}")
            continue

        asset_name = asset.get_name()
        results[asset_name] = {
            "path": asset_path,
            "base_stats": {}
        }

        # Check if this asset has BaseStats property
        if hasattr(asset, 'base_stats'):
            base_stats_map = asset.base_stats
            print(f"  Found base_stats property with {len(base_stats_map) if base_stats_map else 0} entries")

            if base_stats_map:
                for stat_class, value in base_stats_map.items():
                    if stat_class:
                        class_name = stat_class.get_name()
                        results[asset_name]["base_stats"][class_name] = value
                        print(f"    {class_name}: {value}")
        else:
            # Try to access via get_editor_property
            try:
                base_stats_map = asset.get_editor_property('base_stats')
                print(f"  Found BaseStats via get_editor_property with {len(base_stats_map) if base_stats_map else 0} entries")

                if base_stats_map:
                    for stat_class, value in base_stats_map.items():
                        if stat_class:
                            class_name = stat_class.get_name()
                            results[asset_name]["base_stats"][class_name] = value
                            print(f"    {class_name}: {value}")
            except Exception as e:
                print(f"  Could not access BaseStats: {e}")

        # Also try using reflection to find any TMap properties
        print(f"\n  All properties on {asset_name}:")
        for prop_name in dir(asset):
            if not prop_name.startswith('_') and not callable(getattr(asset, prop_name, None)):
                try:
                    val = getattr(asset, prop_name)
                    if val is not None:
                        val_str = str(val)[:100]
                        print(f"    {prop_name}: {val_str}")
                except:
                    pass

    # Save results
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(results, f, indent=2)

    print(f"\n\nSaved to: {OUTPUT_PATH}")
    print("=" * 80)

    return results

if __name__ == "__main__":
    extract_basestats()
