"""
Apply TimeInfoAsset to B_SkyManager Blueprint CDO
This ensures the rest menu gets time entries from DA_ExampleDayNightInfo
"""
import unreal

def main():
    # Load the DayNight data asset
    daynight_path = "/Game/SoulslikeFramework/Data/DayNight/DA_ExampleDayNightInfo"
    daynight_asset = unreal.EditorAssetLibrary.load_asset(daynight_path)

    if not daynight_asset:
        unreal.log_error(f"Could not load: {daynight_path}")
        return

    unreal.log_warning(f"Loaded DayNight asset: {daynight_asset.get_name()}")

    # Load B_SkyManager Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
    bp_asset = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp_asset:
        unreal.log_error(f"Could not load Blueprint: {bp_path}")
        return

    unreal.log_warning(f"Loaded Blueprint: {bp_asset.get_name()}")

    # Get the generated class and CDO
    gen_class = bp_asset.generated_class()
    if not gen_class:
        unreal.log_error("Could not get generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_error("Could not get CDO")
        return

    # Check current TimeInfoAsset
    try:
        current = cdo.get_editor_property("time_info_asset")
        unreal.log_warning(f"Current TimeInfoAsset: {current.get_name() if current else 'None'}")
    except:
        unreal.log_warning("TimeInfoAsset property not found")

    # Set the TimeInfoAsset
    try:
        cdo.set_editor_property("time_info_asset", daynight_asset)
        unreal.log_warning(f"Set TimeInfoAsset to: {daynight_asset.get_name()}")
    except Exception as e:
        unreal.log_error(f"Failed to set TimeInfoAsset: {e}")
        return

    # Verify
    try:
        verify = cdo.get_editor_property("time_info_asset")
        unreal.log_warning(f"Verified TimeInfoAsset: {verify.get_name() if verify else 'None'}")
    except Exception as e:
        unreal.log_error(f"Verification failed: {e}")

    # Save
    if unreal.EditorAssetLibrary.save_asset(bp_path):
        unreal.log_warning(f"Saved: {bp_path}")
    else:
        unreal.log_error(f"Failed to save: {bp_path}")

    unreal.log_warning("Done!")

if __name__ == "__main__":
    main()
