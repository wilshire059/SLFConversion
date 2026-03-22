"""
Check the Category tag settings on W_StatBlock_LevelUp widgets in W_LevelUp.
"""
import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/levelup_widget_categories.txt"

def test():
    lines = []
    lines.append("=" * 80)
    lines.append("W_LevelUp Widget Category Settings")
    lines.append("=" * 80)

    # Load W_LevelUp widget
    bp_path = "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        lines.append(f"ERROR: Could not load {bp_path}")
        with open(OUTPUT_FILE, 'w') as f:
            f.write('\n'.join(lines))
        return

    # Get widget tree
    widget_tree = bp.get_editor_property('widget_tree')
    if not widget_tree:
        lines.append("ERROR: No widget tree")
        with open(OUTPUT_FILE, 'w') as f:
            f.write('\n'.join(lines))
        return

    # Get all widgets
    root = widget_tree.get_editor_property('root_widget')
    lines.append(f"Root widget: {root.get_name() if root else 'None'}")

    # Find all W_StatBlock_LevelUp instances using AllWidgets
    lines.append("\n--- Searching for W_StatBlock_LevelUp widgets ---")

    all_widgets = widget_tree.get_editor_property('all_widgets')
    lines.append(f"Total widgets: {len(all_widgets) if all_widgets else 0}")

    if all_widgets:
        for widget in all_widgets:
            widget_class = widget.get_class()
            class_name = widget_class.get_name()

            # Check for StatBlock widgets
            if "StatBlock" in class_name or "stat_block" in class_name.lower():
                widget_name = widget.get_name()
                lines.append(f"\n  Found: {widget_name} (Class: {class_name})")

                # Try to get Category property
                try:
                    category = widget.get_editor_property('category')
                    if category:
                        tag_name = category.get_editor_property('tag_name') if hasattr(category, 'get_editor_property') else str(category)
                        lines.append(f"    Category: {tag_name}")
                    else:
                        lines.append(f"    Category: (not set)")
                except Exception as e:
                    lines.append(f"    Category: ERROR - {e}")

                # Get all properties for investigation
                lines.append(f"    Widget class path: {widget_class.get_path_name()}")

    # List known stat block widget names from Blueprint JSON
    lines.append("\n--- Expected Stat Blocks (from Blueprint) ---")
    expected_blocks = [
        ("PrimaryAttributes", "SoulslikeFramework.Stat.Primary"),
        ("SecondaryStats", "SoulslikeFramework.Stat.Secondary"),
        ("MiscStats", "SoulslikeFramework.Stat.Misc"),
        ("AttackStats", "SoulslikeFramework.Stat.Secondary.AttackPower"),
        ("NegationStats", "SoulslikeFramework.Stat.DamageNegation"),
        ("ResistanceStats", "SoulslikeFramework.Stat.Resistance"),
    ]
    for name, expected_category in expected_blocks:
        lines.append(f"  {name}: {expected_category}")

    lines.append("\n" + "=" * 80)
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))
    print(f"Output written to {OUTPUT_FILE}")

if __name__ == "__main__":
    test()
