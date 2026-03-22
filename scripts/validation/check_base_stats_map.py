# check_base_stats_map.py
# Verify BaseStats TMap on character class data assets

import unreal

def check_base_stats():
    """Check if BaseStats TMap has entries on character class data assets."""

    assets_to_check = [
        "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Quinn",
        "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Manny",
    ]

    unreal.log_warning("=" * 60)
    unreal.log_warning("Checking BaseStats TMap on character class data assets")
    unreal.log_warning("=" * 60)

    for asset_path in assets_to_check:
        asset = unreal.load_asset(asset_path)
        if not asset:
            unreal.log_warning(f"Could not load: {asset_path}")
            continue

        unreal.log_warning(f"\nAsset: {asset_path}")
        unreal.log_warning(f"  Class: {asset.__class__.__name__}")

        # Check if it has base_stats property
        if hasattr(asset, 'base_stats'):
            base_stats = asset.get_editor_property('base_stats')
            unreal.log_warning(f"  BaseStats count: {len(base_stats) if base_stats else 0}")

            if base_stats:
                for stat_class, value in base_stats.items():
                    class_name = stat_class.get_name() if stat_class else "None"
                    unreal.log_warning(f"    {class_name}: {value}")
            else:
                unreal.log_warning("  BaseStats is EMPTY or None!")
        else:
            unreal.log_warning(f"  ERROR: No base_stats property!")

            # Try to list all properties
            unreal.log_warning("  Available properties:")
            for prop in dir(asset):
                if not prop.startswith('_'):
                    unreal.log_warning(f"    - {prop}")

    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    check_base_stats()
