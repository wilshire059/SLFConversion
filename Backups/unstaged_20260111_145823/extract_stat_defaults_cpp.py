"""
Extract stat default values from Blueprint CDOs using C++ function.

Run on BACKUP project:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/bp_only/bp_only.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_stat_defaults_cpp.py" ^
  -stdout -unattended -nosplash

Output: C:/scripts/SLFConversion/migration_cache/stat_defaults.json
"""

import unreal

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/stat_defaults.json"

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("EXTRACT STAT DEFAULTS (C++ Implementation)")
    unreal.log_warning("=" * 60)

    # Call C++ function
    result = unreal.SLFAutomationLibrary.extract_stat_defaults(OUTPUT_PATH)

    unreal.log_warning(f"Extracted {result} stat defaults to {OUTPUT_PATH}")

if __name__ == "__main__":
    main()
