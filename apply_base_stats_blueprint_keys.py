# apply_base_stats_blueprint_keys.py
# Populates BaseStats TMap using BLUEPRINT class keys (not C++ classes)
# This matches the original bp_only design pattern

import unreal

def log(msg):
    """Log to both UE and print."""
    unreal.log_warning(msg)

# Default starting values for Primary stats (typical Souls-like starting class)
# Keys are BLUEPRINT stat class paths (B_Vigor, B_Mind, etc.)
# NOT C++ class paths (SLFStatVigor, etc.)
DEFAULT_BASE_STATS = {
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor.B_Vigor_C": 10.0,
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Mind.B_Mind_C": 10.0,
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Endurance.B_Endurance_C": 10.0,
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength.B_Strength_C": 10.0,
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity.B_Dexterity_C": 10.0,
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Intelligence.B_Intelligence_C": 10.0,
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Faith.B_Faith_C": 10.0,
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Arcane.B_Arcane_C": 10.0,
}

# Character class data assets to update
CHARACTER_CLASS_ASSETS = [
    "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Manny",
    "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Quinn",
]

def apply_base_stats():
    """Apply default base stats using Blueprint class keys."""

    log("=" * 60)
    log("Applying BaseStats with BLUEPRINT class keys")
    log("=" * 60)

    # Build base stats map with Blueprint class keys
    base_stats_map = {}
    for class_path, value in DEFAULT_BASE_STATS.items():
        stat_class = unreal.load_class(None, class_path)
        if stat_class:
            base_stats_map[stat_class] = value
            log(f"Loaded Blueprint stat class: {stat_class.get_name()} -> {value}")
        else:
            log(f"WARNING: Could not load Blueprint stat class: {class_path}")

    if not base_stats_map:
        log("ERROR: No stat classes loaded!")
        return

    log(f"\nTotal stat classes loaded: {len(base_stats_map)}")

    # Apply to each character class asset
    for asset_path in CHARACTER_CLASS_ASSETS:
        log(f"\nProcessing: {asset_path}")

        asset = unreal.load_asset(asset_path)
        if not asset:
            log(f"  WARNING: Could not load asset: {asset_path}")
            continue

        log(f"  Asset class: {asset.__class__.__name__}")

        # Check if it has base_stats property
        if not hasattr(asset, 'base_stats'):
            log(f"  WARNING: Asset does not have base_stats property")
            # List available properties for debugging
            log(f"  Available properties: {[p for p in dir(asset) if not p.startswith('_')][:20]}...")
            continue

        # Set the BaseStats map
        try:
            asset.set_editor_property('base_stats', base_stats_map)
            log(f"  Set {len(base_stats_map)} base stats")

            # Verify the assignment
            verify_map = asset.get_editor_property('base_stats')
            log(f"  Verification - BaseStats count: {len(verify_map) if verify_map else 0}")

            # Save the asset
            if unreal.EditorAssetLibrary.save_asset(asset_path):
                log(f"  Saved: {asset_path}")
            else:
                log(f"  WARNING: Failed to save: {asset_path}")

        except Exception as e:
            log(f"  ERROR: {e}")

    log("\n" + "=" * 60)
    log("Done applying base stats with Blueprint class keys")
    log("=" * 60)

if __name__ == "__main__":
    apply_base_stats()
