# fix_component_cdo.py
# Sets StatTable on the AC_StatManager Blueprint component's CDO
#
# Run with:
# UnrealEditor-Cmd.exe "project.uproject" -run=pythonscript -script="fix_component_cdo.py" -stdout -unattended

import unreal

STAT_TABLE_PATH = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable"
COMPONENT_BP_PATH = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"

def log(msg):
    unreal.log_warning("[FixCDO] " + str(msg))

def fix():
    log("=" * 50)
    log("Fix AC_StatManager CDO StatTable")
    log("=" * 50)

    # Load the stat table
    stat_table = unreal.load_asset(STAT_TABLE_PATH)
    if not stat_table:
        log("ERROR: Could not load StatTable")
        return

    log("Loaded StatTable: " + stat_table.get_name())

    # Use C++ helper to set the CDO's StatTable
    result = unreal.SLFAutomationLibrary.set_blueprint_component_cdo_stat_table(
        COMPONENT_BP_PATH, stat_table
    )

    if result:
        log("SUCCESS - Set StatTable on CDO")

        # Save the component Blueprint
        bp = unreal.load_asset(COMPONENT_BP_PATH)
        if bp:
            unreal.EditorAssetLibrary.save_loaded_asset(bp)
            log("Saved component Blueprint!")
    else:
        log("FAILED - Could not set StatTable on CDO")

    log("=" * 50)

fix()
