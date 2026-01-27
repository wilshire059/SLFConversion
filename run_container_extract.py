# run_container_extract.py
# Run C++ container loot extraction on SLFConversion

import unreal

def main():
    output_path = "C:/scripts/SLFConversion/migration_cache/container_loot_slfconversion.json"

    result = unreal.SLFAutomationLibrary.extract_container_loot_config(output_path)

    unreal.log_warning(f"Extraction result:\n{result}")

if __name__ == "__main__":
    main()
