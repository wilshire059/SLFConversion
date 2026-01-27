"""
Test stat derivation by checking the StatsToAffect configuration in C++ stat classes.
This verifies that the stat derivation chain is properly set up.
"""
import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/stat_derivation_test.txt"

# Stats to test - map stat Blueprint path to expected C++ parent class
PRIMARY_STATS = {
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength": "SLFStatStrength",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Intelligence": "SLFStatIntelligence",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity": "SLFStatDexterity",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Faith": "SLFStatFaith",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Arcane": "SLFStatArcane",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor": "SLFStatVigor",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Endurance": "SLFStatEndurance",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Mind": "SLFStatMind",
}

ATTACK_POWER_STATS = {
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Physical": "SLFAttackPowerPhysical",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Magic": "SLFAttackPowerMagic",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Lightning": "SLFAttackPowerLightning",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Fire": "SLFAttackPowerFire",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Holy": "SLFAttackPowerHoly",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Frost": "SLFAttackPowerFrost",
}

def test():
    lines = []
    lines.append("=" * 80)
    lines.append("STAT DERIVATION TEST")
    lines.append("=" * 80)

    # Test 1: Check primary stat classes have StatsToAffect configured
    lines.append("\n--- PRIMARY STATS: Checking StatsToAffect ---")

    for bp_path, expected_cpp_parent in PRIMARY_STATS.items():
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            lines.append(f"  ERROR: Could not load {bp_path}")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            lines.append(f"  ERROR: {bp_path} has no generated_class")
            continue

        # Create CDO to check StatBehavior
        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            lines.append(f"  ERROR: Could not get CDO for {bp_path}")
            continue

        # Check if StatBehavior has StatsToAffect
        try:
            stat_behavior = cdo.get_editor_property('stat_behavior')
            stats_to_affect = stat_behavior.get_editor_property('stats_to_affect')
            num_affected = len(stats_to_affect) if stats_to_affect else 0

            lines.append(f"  {gen_class.get_name()}")
            lines.append(f"    StatsToAffect count: {num_affected}")

            if stats_to_affect:
                for tag, affected_stats in stats_to_affect.items():
                    softcap_data = affected_stats.get_editor_property('softcap_data')
                    if softcap_data:
                        for entry in softcap_data:
                            modifier = entry.get_editor_property('modifier')
                            lines.append(f"      -> {tag}: modifier={modifier}")
        except Exception as e:
            lines.append(f"    ERROR getting stat_behavior: {e}")

    # Test 2: Check attack power stat tags
    lines.append("\n--- ATTACK POWER STATS: Checking Tags ---")

    for bp_path, expected_cpp_parent in ATTACK_POWER_STATS.items():
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
            display_name = stat_info.get_editor_property('display_name')
            current_value = stat_info.get_editor_property('current_value')

            lines.append(f"  {gen_class.get_name()}")
            lines.append(f"    Tag: {tag}")
            lines.append(f"    DisplayName: {display_name}")
            lines.append(f"    CurrentValue: {current_value}")
        except Exception as e:
            lines.append(f"    ERROR: {e}")

    lines.append("\n" + "=" * 80)

    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))

if __name__ == "__main__":
    test()
