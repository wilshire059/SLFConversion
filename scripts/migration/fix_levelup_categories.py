"""
Fix W_LevelUp stat block category tags.
The category tags set on W_StatBlock_LevelUp instances don't match actual stat tags.

Current (wrong) -> Correct:
- AttackStats: SoulslikeFramework.Stat.Secondary.AttackPower -> SoulslikeFramework.Stat.AttackPower
- NegationStats: SoulslikeFramework.Stat.Defense.Negation -> SoulslikeFramework.Stat.DamageNegation
- ResistanceStats: SoulslikeFramework.Stat.Defense.Resistances -> SoulslikeFramework.Stat.Resistance
"""

import unreal

# Widget path
W_LEVELUP_PATH = "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp"

# Category tag fixes (widget name -> correct category tag string)
CATEGORY_FIXES = {
    "AttackStats": "SoulslikeFramework.Stat.AttackPower",
    "NegationStats": "SoulslikeFramework.Stat.DamageNegation",
    "ResistanceStats": "SoulslikeFramework.Stat.Resistance",
}

def fix_levelup_categories():
    """Fix category tags on W_LevelUp stat blocks."""

    print(f"\n{'='*60}")
    print(f"Fixing W_LevelUp stat block categories")
    print('='*60)

    # Load the widget blueprint
    bp = unreal.EditorAssetLibrary.load_asset(W_LEVELUP_PATH)
    if not bp:
        print(f"ERROR: Could not load {W_LEVELUP_PATH}")
        return False

    print(f"Loaded: {bp.get_name()}")

    # Get the generated class
    gen_class = bp.generated_class()
    if not gen_class:
        print("ERROR: No generated class")
        return False

    print(f"Generated class: {gen_class.get_name()}")

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        print("ERROR: No CDO")
        return False

    print(f"CDO: {cdo.get_name()}")

    fixed_count = 0

    # Try to access widget variables through the CDO
    # The widgets are UPROPERTY variables on the Blueprint class
    for widget_name, correct_tag in CATEGORY_FIXES.items():
        print(f"\n  Looking for: {widget_name}")

        try:
            # Try to get the widget as a property (case-insensitive snake_case)
            widget = cdo.get_editor_property(widget_name)
            if widget:
                print(f"    Found widget: {widget.get_name()} (class: {widget.get_class().get_name()})")

                # Check current category
                try:
                    current_cat = widget.get_editor_property("category")
                    print(f"    Current category: {current_cat}")
                except Exception as e:
                    print(f"    Could not read current category: {e}")

                # Create gameplay tag
                tag = unreal.GameplayTag.request_gameplay_tag(unreal.Name(correct_tag))
                if tag.is_valid():
                    print(f"    Setting category to: {correct_tag}")
                    try:
                        widget.set_editor_property("category", tag)
                        fixed_count += 1
                        print(f"    SUCCESS: Category updated")
                    except Exception as e:
                        print(f"    ERROR setting category: {e}")
                else:
                    print(f"    ERROR: Invalid tag: {correct_tag}")
            else:
                print(f"    Widget is None")

        except Exception as e:
            print(f"    Exception accessing {widget_name}: {e}")

    if fixed_count > 0:
        # Mark dirty and save
        print(f"\n  Marking dirty and saving...")
        bp.modify(True)

        print(f"  Compiling blueprint...")
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        except Exception as e:
            print(f"    Compile warning: {e}")

        print(f"  Saving...")
        unreal.EditorAssetLibrary.save_asset(W_LEVELUP_PATH, only_if_is_dirty=False)

        print(f"\nFixed {fixed_count} category tags")
        return True
    else:
        print(f"\nNo widgets fixed")
        print(f"\nNote: The widget category values may need to be manually changed in the UMG editor,")
        print(f"or the W_StatBlock_LevelUp C++ code can be updated to match alternate category tags.")
        return False


if __name__ == "__main__":
    fix_levelup_categories()
    print("\n" + "="*60)
    print("DONE")
    print("="*60)
