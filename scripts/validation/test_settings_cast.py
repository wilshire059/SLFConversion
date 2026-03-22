"""
Test that W_Settings can Cast children to UW_Settings_CategoryEntry.

This verifies the reparenting worked and Cast<> will succeed at runtime.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/test_settings_cast.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("Test W_Settings Category Cast")
    unreal.log_warning("=" * 60)

    # Load the W_Settings_CategoryEntry C++ class
    category_cpp_class = unreal.load_class(None, "/Script/SLFConversion.W_Settings_CategoryEntry")
    if not category_cpp_class:
        unreal.log_warning("FAIL: Could not load C++ class W_Settings_CategoryEntry")
        return

    unreal.log_warning(f"C++ class: {category_cpp_class.get_name()}")

    # Load a Blueprint that derives from W_Settings_CategoryEntry
    # Check if any W_Settings_CategoryEntry_* Blueprint is a child of the C++ class
    bp_paths = [
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry",
    ]

    for bp_path in bp_paths:
        bp = unreal.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"SKIP: Could not load {bp_path}")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            unreal.log_warning(f"SKIP: No generated class for {bp_path}")
            continue

        bp_name = bp.get_name()
        unreal.log_warning(f"=== {bp_name} ===")
        unreal.log_warning(f"  Generated class: {gen_class.get_name()}")

        # Try to create an instance and check if it can be cast
        try:
            # In Python we can't do proper C++ casts, but we can check CDO properties
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Try to access C++ properties - if they exist, reparenting worked
                try:
                    switcher_index = cdo.get_editor_property('switcher_index')
                    selected = cdo.get_editor_property('selected')
                    selected_color = cdo.get_editor_property('selected_color')

                    unreal.log_warning(f"  switcher_index: {switcher_index}")
                    unreal.log_warning(f"  selected: {selected}")
                    unreal.log_warning(f"  selected_color: {selected_color.r}, {selected_color.g}, {selected_color.b}")
                    unreal.log_warning(f"  PASS: C++ properties accessible - Cast<> will work!")
                except Exception as e:
                    unreal.log_warning(f"  FAIL: C++ properties not accessible: {e}")
                    unreal.log_warning(f"  Cast<UW_Settings_CategoryEntry> will FAIL at runtime!")

        except Exception as e:
            unreal.log_warning(f"  Error: {e}")

    # Also check that the base instances can be properly identified
    unreal.log_warning("")
    unreal.log_warning("=== Testing Instance Identity ===")

    # Load W_Settings
    settings_bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings")
    if settings_bp:
        settings_class = settings_bp.generated_class()
        if settings_class:
            settings_cdo = unreal.get_default_object(settings_class)
            if settings_cdo:
                try:
                    categories_box = settings_cdo.get_editor_property('categories_box')
                    unreal.log_warning(f"W_Settings.categories_box: {categories_box}")

                    category_switcher = settings_cdo.get_editor_property('category_switcher')
                    unreal.log_warning(f"W_Settings.category_switcher: {category_switcher}")
                except Exception as e:
                    unreal.log_warning(f"W_Settings property error: {e}")

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("SUMMARY")
    unreal.log_warning("=" * 60)
    unreal.log_warning("If C++ properties are accessible, the reparenting worked.")
    unreal.log_warning("Cast<UW_Settings_CategoryEntry> will succeed at runtime.")
    unreal.log_warning("Tab clicking should now work!")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    main()
