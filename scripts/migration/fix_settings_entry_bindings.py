"""
Fix W_Settings_Entry - Clear stale widget delegate bindings.

The ComboBox 'DropDown' has OnGenerateItemWidget and OnGenerateContentWidget
bindings that point to cleared Blueprint functions. We need to clear these
so C++ can bind them at runtime.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/fix_settings_entry_bindings.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal


def fix_w_settings_entry():
    """Clear stale delegate bindings in W_Settings_Entry widget tree."""
    unreal.log_warning("=" * 70)
    unreal.log_warning("Fixing W_Settings_Entry Widget Bindings")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return False

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    # Get the WidgetTree from the WidgetBlueprint
    # In UE5, WidgetBlueprint has a WidgetTree property
    try:
        widget_tree = bp.get_editor_property('widget_tree')
        if widget_tree:
            unreal.log_warning(f"WidgetTree: {widget_tree}")

            # Find the DropDown widget
            root_widget = widget_tree.get_editor_property('root_widget')
            unreal.log_warning(f"RootWidget: {root_widget}")

            # We need to traverse the widget tree to find DropDown
            # This is complex in Python, let's try a different approach
    except Exception as e:
        unreal.log_warning(f"WidgetTree access failed: {e}")

    # Alternative: Use SLFAutomationLibrary if available
    if hasattr(unreal, 'SLFAutomationLibrary'):
        unreal.log_warning("Using SLFAutomationLibrary...")

        # Check if we have a function to clear widget bindings
        # If not, we may need to add one

    # Another approach: Export the Blueprint, modify, reimport
    # Or use the PropertyBindingLibrary

    # Let's try to access the widget blueprint's bindings directly
    try:
        # Get all bindings
        bindings = bp.get_editor_property('bindings')
        if bindings:
            unreal.log_warning(f"Found {len(bindings)} bindings")
            for binding in bindings:
                unreal.log_warning(f"  Binding: {binding}")
    except Exception as e:
        unreal.log_warning(f"Bindings access failed: {e}")

    # Try to find and clear the problematic property bindings
    # WidgetBlueprint has PropertyBindings array
    try:
        prop_bindings = bp.get_editor_property('property_bindings')
        if prop_bindings:
            unreal.log_warning(f"Found {len(prop_bindings)} property bindings")
    except Exception as e:
        unreal.log_warning(f"PropertyBindings: {e}")

    unreal.log_warning("")
    unreal.log_warning("NOTE: May need to manually clear bindings in editor or use C++ automation")

    return False


def check_automation_functions():
    """Check what SLFAutomationLibrary functions are available."""
    unreal.log_warning("")
    unreal.log_warning("=== SLFAutomationLibrary Functions ===")

    if hasattr(unreal, 'SLFAutomationLibrary'):
        lib = unreal.SLFAutomationLibrary
        # List available functions
        for attr in dir(lib):
            if not attr.startswith('_'):
                unreal.log_warning(f"  {attr}")
    else:
        unreal.log_warning("SLFAutomationLibrary not available")


def export_widget_blueprint_text():
    """Export the widget blueprint to see its structure."""
    unreal.log_warning("")
    unreal.log_warning("=== Widget Blueprint Export ===")

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"

    try:
        text = unreal.EditorAssetLibrary.export_text(bp_path)
        if text:
            # Look for DropDown and its bindings
            lines = text.split('\n')
            in_dropdown = False
            for i, line in enumerate(lines):
                if 'Name="DropDown"' in line or 'DropDown' in line:
                    in_dropdown = True
                if in_dropdown:
                    if 'OnGenerate' in line or 'Binding' in line or 'Function' in line:
                        unreal.log_warning(f"  Line {i}: {line.strip()[:100]}")
                    if 'End Object' in line and in_dropdown:
                        in_dropdown = False
                        break
    except Exception as e:
        unreal.log_warning(f"Export failed: {e}")


def main():
    fix_w_settings_entry()
    check_automation_functions()
    export_widget_blueprint_text()

    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning("RECOMMENDATION:")
    unreal.log_warning("The DropDown ComboBox has stale delegate bindings.")
    unreal.log_warning("Need to either:")
    unreal.log_warning("  1. Manually clear bindings in Widget Designer")
    unreal.log_warning("  2. Add C++ automation to clear widget bindings")
    unreal.log_warning("  3. Restore from bp_only and re-migrate carefully")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
