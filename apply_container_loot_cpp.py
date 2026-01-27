# apply_container_loot_cpp.py
# Calls C++ ApplyContainerLootConfig function
# Run on SLFConversion project to apply loot configuration from JSON

import unreal

def main():
    json_path = "C:/scripts/SLFConversion/migration_cache/container_loot_config.json"

    count = unreal.SLFAutomationLibrary.apply_container_loot_config(json_path)

    unreal.log_warning(f"Applied loot config to {count} containers")

    # Save the level
    if count > 0:
        unreal.log_warning("Marking level dirty...")
        unreal.EditorLevelLibrary.mark_actors_dirty_for_cook()

if __name__ == "__main__":
    main()
