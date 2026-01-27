# refresh_level_containers.py
# Refresh B_Container instances in the level to use updated Blueprint

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("REFRESHING LEVEL CONTAINER INSTANCES")
    unreal.log_warning("=" * 70)

    # Just save the level to refresh instances
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    try:
        # Mark dirty and save
        unreal.EditorLevelLibrary.save_current_level()
        unreal.log_warning("Level saved")
    except Exception as e:
        unreal.log_warning(f"Save level: {e}")

    # Also try to save all dirty packages
    try:
        unreal.EditorAssetLibrary.save_directory("/Game/SoulslikeFramework/")
        unreal.log_warning("Directory saved")
    except Exception as e:
        unreal.log_warning(f"Save directory: {e}")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Done - test containers in PIE")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
