# check_ac_statmanager.py
# Check the AC_StatManager Blueprint component's parent class and CDO

import unreal

def log(msg):
    unreal.log_warning("[CheckAC] " + str(msg))

def check():
    log("=" * 50)
    log("Checking AC_StatManager Blueprint Component")
    log("=" * 50)

    # Load the AC_StatManager Blueprint component
    bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"
    bp = unreal.load_asset(bp_path)

    if not bp:
        log("ERROR: Could not load AC_StatManager Blueprint")
        return

    log("Loaded: " + bp.get_name())
    log("Asset class: " + str(bp.get_class().get_name()))

    # Get the generated class from the Blueprint
    try:
        gen_class = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
        if gen_class:
            log("Generated class: " + gen_class.get_name())
            log("Generated class path: " + gen_class.get_path_name())

            # Get parent class
            parent = gen_class.get_super_class()
            if parent:
                log("Parent class: " + parent.get_name())
                log("Parent class path: " + parent.get_path_name())

                # Walk up hierarchy
                current = parent
                depth = 1
                while current and depth < 10:
                    pp = current.get_super_class()
                    if pp:
                        log("  Ancestor(" + str(depth) + "): " + pp.get_name())
                        current = pp
                        depth += 1
                    else:
                        break

            # Get CDO
            cdo = gen_class.get_default_object()
            if cdo:
                log("CDO: " + cdo.get_name())
                log("CDO class: " + cdo.get_class().get_name())

                # Check StatTable property
                try:
                    stat_table = cdo.get_editor_property('stat_table')
                    log("CDO StatTable: " + (stat_table.get_name() if stat_table else "NULL"))
                except Exception as e:
                    log("Could not read CDO StatTable: " + str(e))
    except Exception as e:
        log("Error loading generated class: " + str(e))

    log("=" * 50)

check()
