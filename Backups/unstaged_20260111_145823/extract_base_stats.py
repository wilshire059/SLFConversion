"""
Extract BaseStats TMap data from character class data assets.

Run on BACKUP (bp_only) BEFORE migration:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/bp_only/bp_only.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_base_stats.py" ^
  -stdout -unattended -nosplash

Output: C:/scripts/SLFConversion/migration_cache/base_stats.json
"""

import unreal

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/base_stats.json"

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("EXTRACT BASE STATS (C++ Implementation)")
    unreal.log_warning("=" * 60)

    # Call C++ function
    result = unreal.SLFAutomationLibrary.extract_all_base_stats(OUTPUT_PATH)

    unreal.log_warning(f"Extracted {result} character classes to {OUTPUT_PATH}")

if __name__ == "__main__":
    main()
