# apply_base_stats_defaults.py
# Populates BaseStats TMap on character class data assets (DA_Quinn, DA_Manny, etc.)
# This sets default starting values for Primary stats (Vigor, Mind, Dexterity, etc.)

import unreal

def log(msg):
    """Log to both UE and print."""
    unreal.log_warning(msg)

# Default starting values for Primary stats (typical Souls-like starting class)
# Maps stat class path -> starting value
DEFAULT_BASE_STATS = {
    "/Script/SLFConversion.SLFStatVigor": 10.0,
    "/Script/SLFConversion.SLFStatMind": 10.0,
    "/Script/SLFConversion.SLFStatEndurance": 10.0,
    "/Script/SLFConversion.SLFStatStrength": 10.0,
    "/Script/SLFConversion.SLFStatDexterity": 10.0,
    "/Script/SLFConversion.SLFStatIntelligence": 10.0,
    "/Script/SLFConversion.SLFStatFaith": 10.0,
    "/Script/SLFConversion.SLFStatArcane": 10.0,
}

# Character class data assets to update
# These are PDA_BaseCharacterInfo assets that have the BaseStats property
CHARACTER_CLASS_ASSETS = [
    "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Manny",
    "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Quinn",
]

def apply_base_stats():
    """Apply default base stats to character class data assets."""

    log("=" * 60)
    log("Applying default BaseStats to character class data assets")
    log("=" * 60)

    # Build base stats map
    base_stats_map = {}
    for class_path, value in DEFAULT_BASE_STATS.items():
        stat_class = unreal.load_class(None, class_path)
        if stat_class:
            base_stats_map[stat_class] = value
            log(f"Loaded stat class: {class_path} -> {value}")
        else:
            log(f"WARNING: Could not load stat class: {class_path}")

    if not base_stats_map:
        log("ERROR: No stat classes loaded!")
        return

    # Apply to each character class asset
    for asset_path in CHARACTER_CLASS_ASSETS:
        log(f"Processing: {asset_path}")

        asset = unreal.load_asset(asset_path)
        if not asset:
            log(f"  WARNING: Could not load asset: {asset_path}")
            continue

        # Check if it's the right type
        if not hasattr(asset, 'base_stats'):
            log(f"  WARNING: Asset does not have base_stats property (class: {asset.__class__.__name__})")
            continue

        # Set the BaseStats map
        # Note: In Python, UE TMap<UClass*, double> is accessed as a regular dict
        try:
            asset.set_editor_property('base_stats', base_stats_map)
            log(f"  Set {len(base_stats_map)} base stats")

            # Save the asset
            if unreal.EditorAssetLibrary.save_asset(asset_path):
                log(f"  Saved: {asset_path}")
            else:
                log(f"  WARNING: Failed to save: {asset_path}")

        except Exception as e:
            log(f"  ERROR: {e}")

    log("=" * 60)
    log("Done applying base stats")
    log("=" * 60)

if __name__ == "__main__":
    apply_base_stats()
