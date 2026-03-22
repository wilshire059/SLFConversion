"""
Apply BaseStats to character class data assets (DA_Manny, DA_Quinn).
This sets the starting primary stat values for each character class.

Run on SLFConversion project:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_base_stats.py" ^
  -stdout -unattended -nosplash
"""
import unreal

# Character class data assets to update
CHARACTER_CLASSES = [
    "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Manny",
    "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Quinn",
]

# Stat class paths and their base values
# These are the C++ stat classes that will be the keys in BaseStats TMap
BASE_STAT_VALUES = {
    # Primary stats - all start at 10 for balanced starting character
    "/Script/SLFConversion.SLFStatVigor": 10.0,
    "/Script/SLFConversion.SLFStatEndurance": 10.0,
    "/Script/SLFConversion.SLFStatMind": 10.0,
    "/Script/SLFConversion.SLFStatStrength": 10.0,
    "/Script/SLFConversion.SLFStatDexterity": 10.0,
    "/Script/SLFConversion.SLFStatIntelligence": 10.0,
    "/Script/SLFConversion.SLFStatFaith": 10.0,
    "/Script/SLFConversion.SLFStatArcane": 10.0,
}

def apply_base_stats():
    print("=" * 80)
    print("APPLYING BASESTATS TO CHARACTER CLASSES")
    print("=" * 80)

    # Load stat classes
    stat_classes = {}
    for stat_path, value in BASE_STAT_VALUES.items():
        stat_class = unreal.load_class(None, stat_path)
        if stat_class:
            stat_classes[stat_class] = value
            print(f"  Loaded: {stat_class.get_name()} -> {value}")
        else:
            print(f"  ERROR: Could not load stat class: {stat_path}")

    if not stat_classes:
        print("ERROR: No stat classes loaded!")
        return

    print(f"\nLoaded {len(stat_classes)} stat classes")

    # Apply to each character class
    for char_path in CHARACTER_CLASSES:
        print(f"\n--- Processing: {char_path} ---")

        char_asset = unreal.EditorAssetLibrary.load_asset(char_path)
        if not char_asset:
            print(f"  ERROR: Could not load {char_path}")
            continue

        print(f"  Asset class: {char_asset.get_class().get_name()}")

        # Get the BaseStats property
        try:
            # Create a new TMap with our stat values
            base_stats_map = {}
            for stat_class, value in stat_classes.items():
                base_stats_map[stat_class] = value

            # Set the BaseStats property
            char_asset.set_editor_property('base_stats', base_stats_map)
            print(f"  Set BaseStats with {len(base_stats_map)} entries")

            # Verify
            verify_map = char_asset.get_editor_property('base_stats')
            print(f"  Verified: {len(verify_map)} entries in BaseStats")
            for stat_class, value in verify_map.items():
                if stat_class:
                    print(f"    {stat_class.get_name()}: {value}")

            # Save the asset
            if unreal.EditorAssetLibrary.save_asset(char_path):
                print(f"  Saved: {char_path}")
            else:
                print(f"  ERROR: Failed to save {char_path}")

        except Exception as e:
            print(f"  ERROR: {e}")

    print("\n" + "=" * 80)
    print("Done!")
    print("=" * 80)

if __name__ == "__main__":
    apply_base_stats()
