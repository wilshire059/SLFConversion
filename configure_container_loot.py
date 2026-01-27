# configure_container_loot.py
# Configure different loot items on each container in the demo level

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CONFIGURING CONTAINER LOOT")
    unreal.log_warning("=" * 70)

    # Load the demo level first
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    unreal.log_warning(f"Loading level: {level_path}")

    success = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
    if not success:
        unreal.log_error(f"Failed to load level: {level_path}")
        return

    unreal.log_warning("Level loaded successfully")

    # Configure each container with different loot
    # Container 0 (lower X position): Katana
    result0 = unreal.SLFAutomationLibrary.set_container_loot(
        0,
        "/Game/SoulslikeFramework/Data/Items/DA_Katana",
        1, 1
    )
    unreal.log_warning(f"Container 0 (Katana): {'OK' if result0 else 'FAILED'}")

    # Container 1 (higher X position): Health Flask (2-3 of them)
    result1 = unreal.SLFAutomationLibrary.set_container_loot(
        1,
        "/Game/SoulslikeFramework/Data/Items/DA_HealthFlask",
        2, 3
    )
    unreal.log_warning(f"Container 1 (Health Flask x2-3): {'OK' if result1 else 'FAILED'}")

    # Save the level
    if result0 or result1:
        unreal.log_warning("Saving level...")
        unreal.EditorLoadingAndSavingUtils.save_current_level()
        unreal.log_warning("Level saved!")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Container loot configuration complete!")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
