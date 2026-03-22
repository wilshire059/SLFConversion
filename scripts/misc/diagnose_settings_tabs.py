"""
Diagnose W_Settings tab clicking issues.

This script:
1. Loads W_Settings Blueprint
2. Creates an instance
3. Checks category entries and their buttons
4. Reports what might be blocking clicks

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/diagnose_settings_tabs.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def log(msg):
    """Print to both stdout and unreal log."""
    print(msg)
    unreal.log(msg)

def inspect_widget_hierarchy(widget, depth=0):
    """Recursively inspect widget hierarchy."""
    indent = "  " * depth
    widget_name = widget.get_name() if widget else "NULL"
    widget_class = widget.get_class().get_name() if widget else "NULL"

    # Get visibility
    try:
        visibility = widget.get_visibility()
        vis_str = str(visibility)
    except:
        vis_str = "unknown"

    # Check if it's a button
    is_button = "Button" in widget_class

    log(f"{indent}{widget_name} [{widget_class}] vis={vis_str}")

    if is_button:
        try:
            # Check button properties
            is_enabled = getattr(widget, 'is_enabled', lambda: True)()
            log(f"{indent}  ^ BUTTON - is_enabled={is_enabled}")
        except Exception as e:
            log(f"{indent}  ^ BUTTON - error checking: {e}")

    # Get children if this is a panel widget
    try:
        if hasattr(widget, 'get_all_children'):
            children = widget.get_all_children()
            for child in children:
                inspect_widget_hierarchy(child, depth + 1)
    except:
        pass

def diagnose_category_entry(entry):
    """Diagnose a single category entry widget."""
    entry_name = entry.get_name()
    log(f"\n{'='*60}")
    log(f"Category Entry: {entry_name}")
    log(f"{'='*60}")

    # Check SwitcherIndex
    try:
        switcher_idx = entry.get_editor_property('switcher_index')
        log(f"  SwitcherIndex: {switcher_idx}")
    except Exception as e:
        log(f"  SwitcherIndex: error - {e}")

    # Check Selected state
    try:
        selected = entry.get_editor_property('selected')
        log(f"  Selected: {selected}")
    except Exception as e:
        log(f"  Selected: error - {e}")

    # Check for Button widget
    button_found = False
    try:
        # Use reflection to find child widgets
        if hasattr(entry, 'get_all_children'):
            children = entry.get_all_children()
            log(f"  Child count: {len(children)}")
            for child in children:
                child_name = child.get_name()
                child_class = child.get_class().get_name()
                log(f"    - {child_name} [{child_class}]")
                if child_name == "Button" or "Button" in child_class:
                    button_found = True
                    # Check button visibility
                    try:
                        vis = child.get_visibility()
                        log(f"      ^ Button visibility: {vis}")
                    except:
                        pass
    except Exception as e:
        log(f"  Children error: {e}")

    if not button_found:
        log(f"  WARNING: No Button widget found!")

    return button_found

def main():
    log("=" * 60)
    log("W_Settings Tab Clicking Diagnostic")
    log("=" * 60)

    # Load W_Settings Blueprint
    settings_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"

    bp = unreal.load_asset(settings_path)
    if not bp:
        log(f"ERROR: Could not load {settings_path}")
        return

    log(f"Loaded: {bp.get_name()}")
    log(f"Class: {bp.get_class().get_name()}")

    # Get generated class
    try:
        gen_class = bp.generated_class()
        if gen_class:
            log(f"Generated class: {gen_class.get_name()}")

            # Get parent class
            parent = gen_class.get_super_class()
            if parent:
                log(f"Parent class: {parent.get_name()}")
    except Exception as e:
        log(f"Could not get generated class: {e}")

    # Load W_Settings_CategoryEntry to check its structure
    category_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"

    log(f"\n{'='*60}")
    log("Checking W_Settings_CategoryEntry Blueprint")
    log("=" * 60)

    cat_bp = unreal.load_asset(category_path)
    if cat_bp:
        log(f"Loaded: {cat_bp.get_name()}")

        try:
            cat_gen_class = cat_bp.generated_class()
            if cat_gen_class:
                log(f"Generated class: {cat_gen_class.get_name()}")
                cat_parent = cat_gen_class.get_super_class()
                if cat_parent:
                    log(f"Parent class: {cat_parent.get_name()}")

                    # Walk up parent chain
                    current = cat_parent
                    depth = 0
                    while current and depth < 10:
                        log(f"  -> {current.get_name()}")
                        current = current.get_super_class()
                        depth += 1
        except Exception as e:
            log(f"Error: {e}")
    else:
        log(f"ERROR: Could not load {category_path}")

    # Check what widgets exist in the Blueprint's widget tree
    log(f"\n{'='*60}")
    log("Blueprint Widget Tree Check (from CDO)")
    log("=" * 60)

    try:
        # Get the CDO
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                log(f"CDO: {cdo.get_name()}")

                # Check CategoriesBox binding
                try:
                    categories_box = cdo.get_editor_property('categories_box')
                    if categories_box:
                        log(f"CategoriesBox: {categories_box.get_name()}")
                    else:
                        log("CategoriesBox: NULL")
                except Exception as e:
                    log(f"CategoriesBox error: {e}")

                # Check CategorySwitcher binding
                try:
                    switcher = cdo.get_editor_property('category_switcher')
                    if switcher:
                        log(f"CategorySwitcher: {switcher.get_name()}")
                    else:
                        log("CategorySwitcher: NULL")
                except Exception as e:
                    log(f"CategorySwitcher error: {e}")
    except Exception as e:
        log(f"CDO error: {e}")

    log(f"\n{'='*60}")
    log("Analysis Complete")
    log("=" * 60)
    log("")
    log("If CategoriesBox or CategorySwitcher are NULL, the widget bindings")
    log("are not working. Check if the names match exactly in the Blueprint.")
    log("")
    log("If button clicks aren't working but bindings are OK, check:")
    log("1. Is there an overlay blocking clicks?")
    log("2. Is the button visibility Collapsed?")
    log("3. Are button events being bound in NativeConstruct?")

if __name__ == "__main__":
    main()
