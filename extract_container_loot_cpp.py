# extract_container_loot_cpp.py
# Calls C++ ExtractContainerLootConfig function
# Run on bp_only project to extract original loot configuration

import unreal

def main():
    output_path = "C:/scripts/SLFConversion/migration_cache/container_loot_config.json"

    result = unreal.SLFAutomationLibrary.extract_container_loot_config(output_path)

    if result and result != "{}":
        unreal.log_warning(f"Extraction complete. Output: {output_path}")
    else:
        unreal.log_warning("No container loot configs found")

if __name__ == "__main__":
    main()
