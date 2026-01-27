"""
Verify BaseStats were applied to character class data assets.
"""
import unreal

CHARACTER_CLASSES = [
    "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Manny",
    "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Quinn",
]

def verify():
    print("=" * 80)
    print("VERIFYING BASESTATS")
    print("=" * 80)

    for char_path in CHARACTER_CLASSES:
        print(f"\n--- {char_path} ---")

        char_asset = unreal.EditorAssetLibrary.load_asset(char_path)
        if not char_asset:
            print(f"  ERROR: Could not load")
            continue

        try:
            base_stats = char_asset.get_editor_property('base_stats')
            print(f"  BaseStats entries: {len(base_stats)}")
            for stat_class, value in base_stats.items():
                if stat_class:
                    print(f"    {stat_class.get_name()}: {value}")
        except Exception as e:
            print(f"  ERROR: {e}")

    print("\n" + "=" * 80)

if __name__ == "__main__":
    verify()
