"""
Test W_Settings Blueprint compilation after DisplayName fixes.

Loads the W_Settings Blueprint and checks for compile errors.
"""

import unreal

def test_w_settings_compile():
    """Test if W_Settings compiles without errors."""

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"

    unreal.log("=" * 60)
    unreal.log("Testing W_Settings Blueprint Compilation")
    unreal.log("=" * 60)

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load Blueprint: {bp_path}")
        return False

    unreal.log(f"Loaded Blueprint: {bp.get_name()}")

    # Try to compile
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log("SUCCESS: W_Settings compiled without errors!")
        return True
    except Exception as e:
        unreal.log_error(f"COMPILE ERROR: {str(e)}")
        return False


def test_related_widgets():
    """Test related widget Blueprints that had issues."""

    widgets = [
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_KeyMapping",
        "/Game/SoulslikeFramework/Widgets/_Generic/W_GenericButton",
    ]

    unreal.log("=" * 60)
    unreal.log("Testing Settings-Related Widget Compilation")
    unreal.log("=" * 60)

    success_count = 0
    fail_count = 0

    for bp_path in widgets:
        bp = unreal.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"Could not load: {bp_path}")
            fail_count += 1
            continue

        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log(f"OK: {bp.get_name()}")
            success_count += 1
        except Exception as e:
            unreal.log_error(f"FAIL: {bp.get_name()} - {str(e)}")
            fail_count += 1

    unreal.log("=" * 60)
    unreal.log(f"Results: {success_count} passed, {fail_count} failed")
    unreal.log("=" * 60)

    return fail_count == 0


if __name__ == "__main__":
    test_w_settings_compile()
    test_related_widgets()
