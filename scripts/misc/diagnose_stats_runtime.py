"""
Diagnose what stats are in the stat table (DT_ExampleStatTable).
This shows which stat classes would be created by StatManagerComponent.
"""
import unreal
import os

STAT_TABLE_PATH = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable"
OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/stat_table_contents.txt"

def diagnose():
    lines = []
    lines.append("=" * 80)
    lines.append("STAT TABLE CONTENTS (DT_ExampleStatTable)")
    lines.append("=" * 80)

    stat_table = unreal.EditorAssetLibrary.load_asset(STAT_TABLE_PATH)
    if not stat_table:
        lines.append(f"ERROR: Could not load {STAT_TABLE_PATH}")
        with open(OUTPUT_FILE, 'w') as f:
            f.write('\n'.join(lines))
        return

    lines.append(f"Loaded: {stat_table.get_name()}")

    # Get row names
    row_names = unreal.DataTableFunctionLibrary.get_data_table_row_names(stat_table)
    lines.append(f"Row count: {len(row_names)}")
    lines.append("")

    # Check for key stat types
    attack_power_stats = []
    primary_stats = []
    secondary_stats = []
    other_stats = []

    for row_name in row_names:
        name = str(row_name)
        if "AP_" in name or "AttackPower" in name:
            attack_power_stats.append(name)
        elif any(s in name for s in ["Vigor", "Endurance", "Mind", "Strength", "Dexterity", "Intelligence", "Faith", "Arcane"]):
            primary_stats.append(name)
        elif any(s in name for s in ["HP", "FP", "Stamina", "Defense", "Resistance"]):
            secondary_stats.append(name)
        else:
            other_stats.append(name)

    lines.append("PRIMARY STATS:")
    for s in sorted(primary_stats):
        lines.append(f"  {s}")

    lines.append("\nSECONDARY STATS (HP/FP/Stamina/Defense/Resistance):")
    for s in sorted(secondary_stats):
        lines.append(f"  {s}")

    lines.append("\nATTACK POWER STATS:")
    for s in sorted(attack_power_stats):
        lines.append(f"  {s}")

    lines.append("\nOTHER STATS:")
    for s in sorted(other_stats):
        lines.append(f"  {s}")

    lines.append("\n" + "=" * 80)
    lines.append(f"Total: {len(row_names)} stats")

    # Check if key attack power stats exist
    lines.append("\n" + "=" * 80)
    lines.append("ATTACK POWER CHECK:")
    expected_ap = ["B_AP_Physical", "B_AP_Magic", "B_AP_Lightning", "B_AP_Fire", "B_AP_Holy", "B_AP_Frost"]
    for ap in expected_ap:
        found = any(ap in str(r) for r in row_names)
        status = "FOUND" if found else "MISSING"
        lines.append(f"  {ap}: {status}")

    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))

if __name__ == "__main__":
    diagnose()
