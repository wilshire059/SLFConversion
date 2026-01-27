# fix_stat_manager_data.py
# Sets StatTable property on AC_StatManager SCS components in character Blueprints
#
# Run with:
# UnrealEditor-Cmd.exe "project.uproject" -run=pythonscript -script="fix_stat_manager_data.py" -stdout -unattended

import unreal

# Path to the StatTable
STAT_TABLE_PATH = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable"

# Child Blueprints that need StatManager data
CHILD_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter",
    "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
]

def log(msg):
    """Log using Unreal's logging system"""
    unreal.log_warning("[FixStatData] " + msg)

def fix_stat_manager_data():
    """Fix StatManager component data on all character Blueprints"""

    log("=" * 50)
    log("Fix StatManager Data (via C++ helper)")
    log("=" * 50)

    # Load the stat table
    stat_table = unreal.load_asset(STAT_TABLE_PATH)
    if not stat_table:
        log("ERROR: Could not load StatTable from " + STAT_TABLE_PATH)
        return

    log("Loaded StatTable: " + stat_table.get_name())
    log("StatTable class: " + str(stat_table.get_class().get_name()))

    fixed_count = 0
    skipped_count = 0

    for bp_path in CHILD_BLUEPRINTS:
        bp = unreal.load_asset(bp_path)
        if not bp:
            log("WARNING: Could not load " + bp_path)
            continue

        bp_name = bp_path.split("/")[-1]
        log(bp_name + ":")

        # Use C++ helper to set StatTable
        result = unreal.SLFAutomationLibrary.set_scs_stat_table(bp, stat_table)

        if result:
            log("  SUCCESS - Set StatTable via C++")
            fixed_count += 1

            # Save the asset
            unreal.EditorAssetLibrary.save_loaded_asset(bp)
            log("  Saved!")
        else:
            log("  SKIPPED - No StatManager found or already set")
            skipped_count += 1

    log("=" * 50)
    log("Summary:")
    log("  Fixed: " + str(fixed_count))
    log("  Skipped: " + str(skipped_count))
    log("=" * 50)

fix_stat_manager_data()
