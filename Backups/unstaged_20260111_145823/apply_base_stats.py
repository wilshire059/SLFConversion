"""
Apply BaseStats data to character class data assets after migration.

Run on SLFConversion (main project) AFTER migration:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_base_stats.py" ^
  -stdout -unattended -nosplash

Requires: C:/scripts/SLFConversion/migration_cache/base_stats.json (from extract_base_stats.py)
"""

import unreal

INPUT_PATH = "C:/scripts/SLFConversion/migration_cache/base_stats.json"

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("APPLY BASE STATS (C++ Implementation)")
    unreal.log_warning("=" * 60)

    # Call C++ function
    result = unreal.SLFAutomationLibrary.apply_base_stats_from_cache(INPUT_PATH)

    unreal.log_warning(f"Applied BaseStats to {result} character classes")

if __name__ == "__main__":
    main()
