"""
Check if Camera Settings entries have their SettingTag set.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/check_camera_settingtag.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal


def check_camera_settings():
    """Check the SettingTag values for camera entries in W_Settings."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("Checking Camera Settings SettingTag Values")
    unreal.log_warning("=" * 70)

    # Load W_Settings
    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Failed to load {bp_path}")
        return

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    # Get the widget tree
    try:
        widget_tree = bp.get_editor_property("WidgetTree")
        if widget_tree:
            unreal.log_warning(f"Widget tree found")

            # Try to get InvertCameraX widget
            root = widget_tree.get_editor_property("RootWidget")
            unreal.log_warning(f"Root widget: {root.get_name() if root else 'None'}")

            # Use find_widget to get specific widgets
            for widget_name in ["InvertCameraX", "InvertCameraY", "CameraSpeed"]:
                widget = widget_tree.find_widget(widget_name)
                if widget:
                    unreal.log_warning(f"Found widget: {widget_name}")

                    # Check if it's a W_Settings_Entry
                    if hasattr(widget, 'get_editor_property'):
                        try:
                            setting_tag = widget.get_editor_property("SettingTag")
                            if setting_tag:
                                tag_name = setting_tag.get_editor_property("TagName")
                                unreal.log_warning(f"  SettingTag: {tag_name}")
                            else:
                                unreal.log_warning(f"  SettingTag: None/Empty")
                        except Exception as e:
                            unreal.log_warning(f"  Error getting SettingTag: {e}")

                        try:
                            entry_type = widget.get_editor_property("EntryType")
                            unreal.log_warning(f"  EntryType: {entry_type}")
                        except Exception as e:
                            pass

                        # Check ButtonsValue text
                        try:
                            buttons_value = widget.get_editor_property("ButtonsValue")
                            if buttons_value:
                                text = buttons_value.get_text()
                                unreal.log_warning(f"  ButtonsValue text: {text}")
                            else:
                                unreal.log_warning(f"  ButtonsValue: None")
                        except Exception as e:
                            unreal.log_warning(f"  Error getting ButtonsValue: {e}")
                else:
                    unreal.log_warning(f"Widget NOT FOUND: {widget_name}")
    except Exception as e:
        unreal.log_error(f"Error accessing widget tree: {e}")

    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    check_camera_settings()
