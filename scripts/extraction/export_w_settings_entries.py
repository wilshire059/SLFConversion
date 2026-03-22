"""
Export W_Settings to see how entry instances are configured.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/export_w_settings_entries.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal
import os


def export_w_settings():
    """Export W_Settings to see entry instance configurations."""
    unreal.log_warning("=" * 70)
    unreal.log_warning("Exporting W_Settings Entry Configurations")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"

    try:
        text = unreal.EditorAssetLibrary.export_text(bp_path)
        if text:
            # Save to file for analysis
            output_path = "C:/scripts/SLFConversion/migration_cache/w_settings_export.txt"
            with open(output_path, 'w', encoding='utf-8') as f:
                f.write(text)
            unreal.log_warning(f"Exported to {output_path}")

            # Look for W_Settings_Entry instances
            lines = text.split('\n')
            in_entry = False
            entry_count = 0
            current_entry = {}

            for i, line in enumerate(lines):
                # Find W_Settings_Entry instances
                if 'Begin Object Class=/Script/SLFConversion.W_Settings_Entry' in line or \
                   'Begin Object Class=/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry' in line:
                    in_entry = True
                    entry_count += 1
                    current_entry = {'line': i}
                    # Get name from line
                    if 'Name="' in line:
                        name_start = line.find('Name="') + 6
                        name_end = line.find('"', name_start)
                        current_entry['name'] = line[name_start:name_end]

                elif in_entry:
                    # Capture key properties
                    if 'EntryType=' in line:
                        current_entry['EntryType'] = line.strip()
                    elif 'SettingName=' in line:
                        current_entry['SettingName'] = line.strip()
                    elif 'SettingTag=' in line:
                        current_entry['SettingTag'] = line.strip()
                    elif 'End Object' in line:
                        in_entry = False
                        # Print entry info
                        unreal.log_warning(f"\nEntry {entry_count}: {current_entry.get('name', 'unknown')}")
                        for key, val in current_entry.items():
                            if key not in ['line', 'name']:
                                unreal.log_warning(f"  {key}: {val}")

            unreal.log_warning(f"\nTotal W_Settings_Entry instances found: {entry_count}")

    except Exception as e:
        unreal.log_warning(f"Export failed: {e}")


def export_w_settings_entry_blueprint():
    """Export W_Settings_Entry to see its widget tree."""
    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning("Exporting W_Settings_Entry Widget Tree")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"

    try:
        text = unreal.EditorAssetLibrary.export_text(bp_path)
        if text:
            # Save to file
            output_path = "C:/scripts/SLFConversion/migration_cache/w_settings_entry_export.txt"
            with open(output_path, 'w', encoding='utf-8') as f:
                f.write(text)
            unreal.log_warning(f"Exported to {output_path}")

            # Look for key widgets and their visibility
            lines = text.split('\n')
            for i, line in enumerate(lines):
                # Look for view widgets and their properties
                if any(name in line for name in ['DoubleButtonView', 'DropdownView', 'SliderView',
                                                   'ButtonsValue', 'DecreaseButton', 'IncreaseButton']):
                    unreal.log_warning(f"  Line {i}: {line.strip()[:100]}")

    except Exception as e:
        unreal.log_warning(f"Export failed: {e}")


def check_entry_default_values():
    """Check W_Settings_Entry CDO default values."""
    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning("W_Settings_Entry CDO Default Values")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"
    bp = unreal.load_asset(bp_path)
    if not bp:
        return

    gen_class = bp.generated_class()
    if not gen_class:
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        return

    props = [
        'entry_type',
        'setting_name',
        'setting_description',
        'current_value',
        'unhovered_color',
        'hovered_color',
    ]

    for prop in props:
        try:
            val = cdo.get_editor_property(prop)
            unreal.log_warning(f"  {prop}: {val}")
        except Exception as e:
            unreal.log_warning(f"  {prop}: ERROR - {e}")


def main():
    export_w_settings()
    export_w_settings_entry_blueprint()
    check_entry_default_values()

    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning("Check migration_cache/*.txt files for full export")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
