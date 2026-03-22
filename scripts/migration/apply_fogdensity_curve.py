"""
Apply FogDensityTrack curve to B_SkyManager Blueprint CDO
This curve controls fog density based on time of day
"""
import unreal

def main():
    # Load the fog density curve asset
    curve_path = "/Game/SoulslikeFramework/Blueprints/Sky/CF_FogDensity"
    curve = unreal.EditorAssetLibrary.load_asset(curve_path)

    if not curve:
        unreal.log_error(f"Could not load curve: {curve_path}")
        return

    unreal.log_warning(f"Loaded curve: {curve.get_name()}")

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

    # Check current value
    try:
        current = cdo.get_editor_property("fog_density_track")
        unreal.log_warning(f"Current FogDensityTrack: {current}")
    except:
        unreal.log_warning("FogDensityTrack property not found yet (may need compile)")

    # Set the curve
    try:
        cdo.set_editor_property("fog_density_track", curve)
        unreal.log_warning(f"Set FogDensityTrack to: {curve.get_name()}")
    except Exception as e:
        unreal.log_error(f"Failed to set FogDensityTrack: {e}")
        return

    # Verify
    try:
        verify = cdo.get_editor_property("fog_density_track")
        unreal.log_warning(f"Verified FogDensityTrack: {verify.get_name() if verify else 'None'}")
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
