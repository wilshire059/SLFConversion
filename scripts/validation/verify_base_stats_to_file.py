"""
Verify BaseStats were applied to character class data assets.
Writes output to file since commandlet doesn't show print statements.
"""
import unreal
import os

CHARACTER_CLASSES = [
    "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Manny",
    "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Quinn",
]

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/base_stats_verify.txt"

def verify():
    lines = []
    lines.append("=" * 80)
    lines.append("VERIFYING BASESTATS")
    lines.append("=" * 80)

    for char_path in CHARACTER_CLASSES:
        lines.append(f"\n--- {char_path} ---")

        char_asset = unreal.EditorAssetLibrary.load_asset(char_path)
        if not char_asset:
            lines.append(f"  ERROR: Could not load")
            continue

        try:
            base_stats = char_asset.get_editor_property('base_stats')
            lines.append(f"  BaseStats entries: {len(base_stats)}")
            for stat_class, value in base_stats.items():
                if stat_class:
                    lines.append(f"    {stat_class.get_name()}: {value}")
                else:
                    lines.append(f"    <None>: {value}")
        except Exception as e:
            lines.append(f"  ERROR: {e}")

    lines.append("\n" + "=" * 80)

    # Write to file
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))

if __name__ == "__main__":
    verify()
