"""
Verify stat tags are being set correctly from C++ parent classes.
This checks the C++ stat class defaults directly.
"""
import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/stat_tags_verification.txt"

# C++ stat classes - these have the authoritative tag values
CPP_STAT_CLASSES = {
    # Primary stats
    "Vigor": "/Script/SLFConversion.SLFStatVigor",
    "Mind": "/Script/SLFConversion.SLFStatMind",
    "Endurance": "/Script/SLFConversion.SLFStatEndurance",
    "Strength": "/Script/SLFConversion.SLFStatStrength",
    "Dexterity": "/Script/SLFConversion.SLFStatDexterity",
    "Intelligence": "/Script/SLFConversion.SLFStatIntelligence",
    "Faith": "/Script/SLFConversion.SLFStatFaith",
    "Arcane": "/Script/SLFConversion.SLFStatArcane",

    # Secondary stats (HP, FP, Stamina)
    "HP": "/Script/SLFConversion.SLFStatHP",
    "FP": "/Script/SLFConversion.SLFStatFP",
    "Stamina": "/Script/SLFConversion.SLFStatStamina",

    # Attack Power stats
    "AP_Physical": "/Script/SLFConversion.SLFAttackPowerPhysical",
    "AP_Magic": "/Script/SLFConversion.SLFAttackPowerMagic",
    "AP_Fire": "/Script/SLFConversion.SLFAttackPowerFire",
    "AP_Frost": "/Script/SLFConversion.SLFAttackPowerFrost",
    "AP_Lightning": "/Script/SLFConversion.SLFAttackPowerLightning",
    "AP_Holy": "/Script/SLFConversion.SLFAttackPowerHoly",

    # Defense Negation stats
    "DN_Physical": "/Script/SLFConversion.SLFDamageNegationPhysical",
    "DN_Magic": "/Script/SLFConversion.SLFDamageNegationMagic",
    "DN_Fire": "/Script/SLFConversion.SLFDamageNegationFire",
    "DN_Frost": "/Script/SLFConversion.SLFDamageNegationFrost",
    "DN_Lightning": "/Script/SLFConversion.SLFDamageNegationLightning",
    "DN_Holy": "/Script/SLFConversion.SLFDamageNegationHoly",

    # Resistance stats
    "R_Focus": "/Script/SLFConversion.SLFResistanceFocus",
    "R_Immunity": "/Script/SLFConversion.SLFResistanceImmunity",
    "R_Robustness": "/Script/SLFConversion.SLFResistanceRobustness",
    "R_Vitality": "/Script/SLFConversion.SLFResistanceVitality",
}

def get_tag_string(tag):
    """Convert GameplayTag to string properly."""
    if tag is None:
        return "None"
    try:
        tag_name = tag.get_editor_property('tag_name')
        if tag_name:
            return str(tag_name)
    except:
        pass
    return str(tag)

def test():
    lines = []
    lines.append("=" * 80)
    lines.append("C++ STAT CLASS TAG VERIFICATION")
    lines.append("=" * 80)
    lines.append("\nThese are the tags set in C++ constructors and used at runtime.\n")

    # Group stats by expected category
    categories = {
        "Primary (SoulslikeFramework.Stat.Primary)": ["Vigor", "Mind", "Endurance", "Strength", "Dexterity", "Intelligence", "Faith", "Arcane"],
        "Secondary/Base (SoulslikeFramework.Stat.Secondary)": ["HP", "FP", "Stamina"],
        "Attack Power (SoulslikeFramework.Stat.Secondary.AttackPower)": ["AP_Physical", "AP_Magic", "AP_Fire", "AP_Frost", "AP_Lightning", "AP_Holy"],
        "Damage Negation (SoulslikeFramework.Stat.Defense.Negation)": ["DN_Physical", "DN_Magic", "DN_Fire", "DN_Frost", "DN_Lightning", "DN_Holy"],
        "Resistance (SoulslikeFramework.Stat.Defense.Resistances)": ["R_Focus", "R_Immunity", "R_Robustness", "R_Vitality"],
    }

    for category_name, stat_keys in categories.items():
        lines.append(f"\n--- {category_name} ---")

        for stat_key in stat_keys:
            class_path = CPP_STAT_CLASSES.get(stat_key)
            if not class_path:
                lines.append(f"  {stat_key}: CLASS NOT FOUND")
                continue

            stat_class = unreal.load_class(None, class_path)
            if not stat_class:
                lines.append(f"  {stat_key}: LOAD FAILED ({class_path})")
                continue

            # Get CDO
            cdo = unreal.get_default_object(stat_class)
            if not cdo:
                lines.append(f"  {stat_key}: NO CDO")
                continue

            try:
                stat_info = cdo.get_editor_property('stat_info')
                tag = stat_info.get_editor_property('tag')
                display = stat_info.get_editor_property('display_name')
                current = stat_info.get_editor_property('current_value')
                max_val = stat_info.get_editor_property('max_value')

                tag_str = get_tag_string(tag)
                is_valid = "SoulslikeFramework.Stat" in tag_str
                status = "OK" if is_valid else "MISSING"

                lines.append(f"  [{status}] {stat_key}")
                lines.append(f"       Class: {stat_class.get_name()}")
                lines.append(f"       Tag: {tag_str}")
                lines.append(f"       Display: {display}")
                lines.append(f"       Values: {current}/{max_val}")

            except Exception as e:
                lines.append(f"  {stat_key}: ERROR - {e}")

    lines.append("\n" + "=" * 80)
    lines.append("WIDGET CATEGORY MAPPING CHECK")
    lines.append("=" * 80)
    lines.append("\nW_StatBlock_LevelUp category remapping (in C++):")
    lines.append("  - Secondary.AttackPower → SoulslikeFramework.Stat.Secondary.AttackPower")
    lines.append("  - DamageNegation/Negation → SoulslikeFramework.Stat.Defense.Negation")
    lines.append("  - Resistance → SoulslikeFramework.Stat.Defense.Resistances")
    lines.append("\nSecondary block EXCLUDES stats matching:")
    lines.append("  - SoulslikeFramework.Stat.Secondary.AttackPower (prevents attack stats in Base Stats)")

    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))

    print(f"Output written to {OUTPUT_FILE}")

if __name__ == "__main__":
    test()
