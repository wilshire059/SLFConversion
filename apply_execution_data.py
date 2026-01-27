"""Apply execution animation data to DA_Execute_Generic using C++ SLFAutomationLibrary."""
import unreal

def apply_execution_data():
    """Set ExecuteFront and ExecuteBack animations on execution data assets."""

    # Known animation paths from bp_only exploration
    FRONT_ANIM = "/Game/SoulslikeFramework/Demo/_Animations/Combat/Generic/AM_SLF_Generic_Execute.AM_SLF_Generic_Execute"
    BACK_ANIM = "/Game/SoulslikeFramework/Demo/_Animations/Combat/Generic/AM_SLF_Generic_Execute.AM_SLF_Generic_Execute"  # Same anim for both for now

    # Execution data assets to update
    assets = [
        "/Game/SoulslikeFramework/Data/_AnimationData/Executions/DA_Execute_Generic",
    ]

    for asset_path in assets:
        unreal.log("=" * 60)
        unreal.log("Processing: " + asset_path)

        # First, diagnose current state
        diag = unreal.SLFAutomationLibrary.get_execution_anim_data(asset_path)
        unreal.log("Current state:")
        unreal.log(diag)

        # Apply the animation data
        result = unreal.SLFAutomationLibrary.set_execution_anim_data(asset_path, FRONT_ANIM, BACK_ANIM)
        if result:
            unreal.log("SUCCESS: Applied execution animations")
        else:
            unreal.log_error("FAILED: Could not apply execution animations")

        # Verify
        diag_after = unreal.SLFAutomationLibrary.get_execution_anim_data(asset_path)
        unreal.log("After applying:")
        unreal.log(diag_after)

    unreal.log("=" * 60)
    unreal.log("Done applying execution animation data")

if __name__ == "__main__":
    apply_execution_data()
