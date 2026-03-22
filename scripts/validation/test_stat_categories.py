"""
Test stat categories to verify proper grouping in level-up widget.
This checks the actual tags on all stat classes to ensure they match the widget categories.
"""
import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/stat_category_test.txt"

def test():
    lines = []
    lines.append("=" * 80)
    lines.append("STAT CATEGORY TEST - Verifying Widget Grouping")
    lines.append("=" * 80)

    # Load all stat Blueprints and check their tags
    stat_paths = [
        # Primary stats
        ("/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor", "Primary"),
        ("/Game/SoulslikeFramework/Data/Stats/Primary/B_Mind", "Primary"),
        ("/Game/SoulslikeFramework/Data/Stats/Primary/B_Endurance", "Primary"),
        ("/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength", "Primary"),
        ("/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity", "Primary"),
        ("/Game/SoulslikeFramework/Data/Stats/Primary/B_Intelligence", "Primary"),
        ("/Game/SoulslikeFramework/Data/Stats/Primary/B_Faith", "Primary"),
        ("/Game/SoulslikeFramework/Data/Stats/Primary/B_Arcane", "Primary"),

        # Secondary stats (HP, FP, Stamina)
        ("/Game/SoulslikeFramework/Data/Stats/Secondary/B_HP", "Secondary"),
        ("/Game/SoulslikeFramework/Data/Stats/Secondary/B_FP", "Secondary"),
        ("/Game/SoulslikeFramework/Data/Stats/Secondary/B_Stamina", "Secondary"),

        # Attack Power stats
        ("/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Physical", "AttackPower"),
        ("/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Magic", "AttackPower"),
        ("/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Fire", "AttackPower"),
        ("/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Frost", "AttackPower"),
        ("/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Lightning", "AttackPower"),
        ("/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Holy", "AttackPower"),

        # Defense Negation stats
        ("/Game/SoulslikeFramework/Data/Stats/Defense/Negation/B_DN_Physical", "DamageNegation"),
        ("/Game/SoulslikeFramework/Data/Stats/Defense/Negation/B_DN_Magic", "DamageNegation"),
        ("/Game/SoulslikeFramework/Data/Stats/Defense/Negation/B_DN_Fire", "DamageNegation"),
        ("/Game/SoulslikeFramework/Data/Stats/Defense/Negation/B_DN_Frost", "DamageNegation"),
        ("/Game/SoulslikeFramework/Data/Stats/Defense/Negation/B_DN_Lightning", "DamageNegation"),
        ("/Game/SoulslikeFramework/Data/Stats/Defense/Negation/B_DN_Holy", "DamageNegation"),

        # Resistance stats
        ("/Game/SoulslikeFramework/Data/Stats/Defense/Resistances/B_Immunity", "Resistance"),
        ("/Game/SoulslikeFramework/Data/Stats/Defense/Resistances/B_Focus", "Resistance"),
        ("/Game/SoulslikeFramework/Data/Stats/Defense/Resistances/B_Robustness", "Resistance"),
        ("/Game/SoulslikeFramework/Data/Stats/Defense/Resistances/B_Vitality", "Resistance"),

        # Misc stats
        ("/Game/SoulslikeFramework/Data/Stats/Misc/B_EquipLoad", "Misc"),
        ("/Game/SoulslikeFramework/Data/Stats/Misc/B_Discovery", "Misc"),
    ]

    # Expected widget categories and their tag prefixes
    widget_categories = {
        "Primary": "SoulslikeFramework.Stat.Primary",
        "Secondary": "SoulslikeFramework.Stat.Secondary",  # HP, FP, Stamina only
        "AttackPower": "SoulslikeFramework.Stat.Secondary.AttackPower",
        "DamageNegation": "SoulslikeFramework.Stat.Defense.Negation",
        "Resistance": "SoulslikeFramework.Stat.Defense.Resistances",
        "Misc": "SoulslikeFramework.Stat.Misc",
    }

    lines.append("\n--- Widget Category Tag Prefixes ---")
    for cat, prefix in widget_categories.items():
        lines.append(f"  {cat}: {prefix}")

    lines.append("\n--- Stat Tags by Expected Category ---")

    current_category = None
    for bp_path, expected_category in stat_paths:
        if expected_category != current_category:
            current_category = expected_category
            lines.append(f"\n[{expected_category}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            lines.append(f"  ERROR: Could not load {bp_path}")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            lines.append(f"  ERROR: {bp_path} has no generated_class")
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            lines.append(f"  ERROR: Could not get CDO for {bp_path}")
            continue

        try:
            stat_info = cdo.get_editor_property('stat_info')
            tag = stat_info.get_editor_property('tag')
            tag_str = str(tag) if tag else "NO TAG"
            display_name = stat_info.get_editor_property('display_name')

            # Check if tag matches expected category
            expected_prefix = widget_categories.get(expected_category, "")
            matches = expected_prefix in tag_str
            status = "OK" if matches else "MISMATCH"

            lines.append(f"  {status}: {gen_class.get_name()}")
            lines.append(f"       Tag: {tag_str}")
            lines.append(f"       DisplayName: {display_name}")

            if not matches:
                lines.append(f"       Expected prefix: {expected_prefix}")

        except Exception as e:
            lines.append(f"  ERROR: {gen_class.get_name()} - {e}")

    # Test the Secondary category exclusion
    lines.append("\n" + "=" * 80)
    lines.append("SECONDARY EXCLUSION TEST")
    lines.append("Stats matching Secondary but should be excluded from Base Stats widget:")
    lines.append("=" * 80)

    secondary_prefix = "SoulslikeFramework.Stat.Secondary"
    attack_power_prefix = "SoulslikeFramework.Stat.Secondary.AttackPower"

    for bp_path, expected_category in stat_paths:
        if expected_category not in ["Secondary", "AttackPower"]:
            continue

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            continue

        try:
            stat_info = cdo.get_editor_property('stat_info')
            tag = stat_info.get_editor_property('tag')
            tag_str = str(tag) if tag else ""

            matches_secondary = secondary_prefix in tag_str
            matches_attack_power = attack_power_prefix in tag_str

            if matches_secondary:
                should_exclude = matches_attack_power
                lines.append(f"  {gen_class.get_name()}: Tag={tag_str}")
                lines.append(f"    -> Matches Secondary: Yes")
                lines.append(f"    -> Matches AttackPower: {'Yes' if matches_attack_power else 'No'}")
                lines.append(f"    -> Should exclude from Base Stats: {'Yes' if should_exclude else 'No'}")
        except:
            pass

    lines.append("\n" + "=" * 80)

    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))

    print(f"Output written to {OUTPUT_FILE}")

if __name__ == "__main__":
    test()
