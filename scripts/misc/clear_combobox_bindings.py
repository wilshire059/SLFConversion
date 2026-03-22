"""
Clear ComboBoxKey delegate bindings from W_Settings_Entry.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/clear_combobox_bindings.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def clear_widget_delegate_bindings(widget_bp_path):
    """Clear delegate bindings from ComboBoxKey widgets."""

    unreal.log_warning(f"Loading {widget_bp_path}")

    widget_bp = unreal.load_asset(widget_bp_path)
    if not widget_bp:
        unreal.log_error(f"Failed to load {widget_bp_path}")
        return False

    unreal.log_warning(f"WidgetBlueprint type: {type(widget_bp)}")

    # List all attributes
    attrs = dir(widget_bp)
    unreal.log_warning(f"Attributes: {[a for a in attrs if not a.startswith('_')]}")

    # Check for bindings property
    if hasattr(widget_bp, 'bindings'):
        bindings = widget_bp.bindings
        unreal.log_warning(f"Bindings: {bindings}")
    else:
        unreal.log_warning("No 'bindings' attribute")

    # Try get_editor_property
    try:
        bindings = widget_bp.get_editor_property('bindings')
        unreal.log_warning(f"get_editor_property('bindings'): {bindings}")
        if bindings:
            unreal.log_warning(f"Bindings count: {len(bindings)}")
            for i, b in enumerate(bindings):
                unreal.log_warning(f"  Binding {i}: {b}")
    except Exception as e:
        unreal.log_warning(f"No bindings property: {e}")

    # Try other properties
    property_names = [
        'widget_tree', 'WidgetTree',
        'bindings', 'Bindings',
        'delegate_signature_graphs', 'DelegateSignatureGraphs'
    ]

    for prop in property_names:
        try:
            val = widget_bp.get_editor_property(prop)
            unreal.log_warning(f"Property {prop}: {val}")
        except:
            pass

    return True


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Investigating Widget Bindings")
    unreal.log_warning("=" * 70)

    clear_widget_delegate_bindings("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Done!")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
