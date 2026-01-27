"""
Test W_Settings tab clicking - diagnose Cast and button binding issues.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/test_settings_click_debug.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def check_blueprint_parent(bp_path, expected_cpp_class):
    """Check if a Blueprint is reparented to the expected C++ class."""
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return False

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning(f"FAIL: No generated class for {bp_path}")
        return False

    # For UClass, use static_class() comparison instead of get_super_class
    # Check if the generated class is a child of the expected C++ class
    unreal.log_warning(f"=== {bp.get_name()} ===")
    unreal.log_warning(f"  Generated class: {gen_class.get_name()}")
    unreal.log_warning(f"  Path: {gen_class.get_path_name()}")

    # Try to load the expected C++ class and check inheritance
    try:
        cpp_class = unreal.load_class(None, f"/Script/SLFConversion.{expected_cpp_class}")
        if cpp_class:
            unreal.log_warning(f"  Expected C++ class: {cpp_class.get_name()}")
            is_child = gen_class.is_child_of(cpp_class)
            unreal.log_warning(f"  Is child of {expected_cpp_class}: {is_child}")

            if is_child:
                unreal.log_warning(f"  PASS: Reparenting correct")
                return True
            else:
                unreal.log_warning(f"  FAIL: NOT reparented to {expected_cpp_class}!")
                return False
        else:
            unreal.log_warning(f"  Could not load C++ class: {expected_cpp_class}")
    except Exception as e:
        unreal.log_warning(f"  ERROR checking inheritance: {e}")

    return False

def check_widget_tree(bp_path, widget_names):
    """Check if named widgets exist in the Blueprint."""
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return False

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning(f"FAIL: No generated class for {bp_path}")
        return False

    unreal.log_warning(f"=== Widget Tree Check: {bp.get_name()} ===")

    # Get CDO to check widget bindings
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_warning(f"FAIL: No CDO for {gen_class.get_name()}")
        return False

    for name in widget_names:
        # Convert property name to snake_case
        prop_name = name[0].lower() + ''.join('_' + c.lower() if c.isupper() else c for c in name[1:])
        try:
            # Try to get the property
            value = cdo.get_editor_property(prop_name)
            if value:
                unreal.log_warning(f"  {name} ({prop_name}): FOUND - {value.get_name()}")
            else:
                unreal.log_warning(f"  {name} ({prop_name}): NULL (BindWidgetOptional not resolved)")
        except Exception as e:
            unreal.log_warning(f"  {name} ({prop_name}): ERROR - {e}")

    return True

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("W_Settings Tab Click Diagnostic")
    unreal.log_warning("=" * 60)

    # Check W_Settings_CategoryEntry Blueprint reparenting
    unreal.log_warning("")
    unreal.log_warning("STEP 1: Checking Blueprint Reparenting")
    unreal.log_warning("-" * 60)

    cat_ok = check_blueprint_parent(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry",
        "W_Settings_CategoryEntry"
    )

    settings_ok = check_blueprint_parent(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
        "W_Settings"
    )

    # Check widget bindings
    unreal.log_warning("")
    unreal.log_warning("STEP 2: Checking W_Settings Widget Bindings")
    unreal.log_warning("-" * 60)

    check_widget_tree(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
        ["CategoriesBox", "CategorySwitcher"]
    )

    unreal.log_warning("")
    unreal.log_warning("STEP 3: Checking W_Settings_CategoryEntry Properties")
    unreal.log_warning("-" * 60)

    # Load W_Settings_CategoryEntry and check for properties
    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"
    bp = unreal.load_asset(bp_path)
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            # Check widget tree in Blueprint (not CDO since Button isn't a UPROPERTY)
            unreal.log_warning(f"  Blueprint: {bp.get_name()}")
            unreal.log_warning(f"  Class: {gen_class.get_name()}")

            # Get CDO
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Check properties that should exist
                props = ['switcher_index', 'selected', 'selected_color', 'deselected_color', 'setting_category']
                for prop in props:
                    try:
                        val = cdo.get_editor_property(prop)
                        unreal.log_warning(f"    {prop}: {val}")
                    except Exception as e:
                        unreal.log_warning(f"    {prop}: ERROR - {e}")

    # Summary
    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("DIAGNOSIS SUMMARY")
    unreal.log_warning("=" * 60)

    if cat_ok and settings_ok:
        unreal.log_warning("PASS: Both Blueprints are properly reparented to C++")
        unreal.log_warning("")
        unreal.log_warning("If tabs still don't click, the issue is likely:")
        unreal.log_warning("  1. Button widget not found in W_Settings_CategoryEntry")
        unreal.log_warning("  2. Button visibility is Collapsed")
        unreal.log_warning("  3. Another widget is blocking button clicks")
        unreal.log_warning("  4. OnClicked event not being fired")
        unreal.log_warning("")
        unreal.log_warning("Check UE_LOG output in PIE for:")
        unreal.log_warning("  - '[W_Settings_CategoryEntry] Button found' messages")
        unreal.log_warning("  - '[W_Settings_CategoryEntry] HandleButtonClicked' when clicking")
    else:
        if not cat_ok:
            unreal.log_warning("FAIL: W_Settings_CategoryEntry NOT reparented!")
            unreal.log_warning("  The Cast<UW_Settings_CategoryEntry> in W_Settings.cpp will FAIL")
            unreal.log_warning("  Tab clicking will not work until reparented")
        if not settings_ok:
            unreal.log_warning("FAIL: W_Settings NOT reparented!")
            unreal.log_warning("  The C++ NativeConstruct will not be called")

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("Diagnostic Complete")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    main()
