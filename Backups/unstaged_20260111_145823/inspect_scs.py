# inspect_scs.py
# Lists all SCS components in character Blueprints using C++ helper

import unreal

BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter",
    "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
]

def log(msg):
    unreal.log_warning("[InspectSCS] " + msg)

def inspect():
    log("=" * 50)

    for bp_path in BLUEPRINTS:
        bp = unreal.load_asset(bp_path)
        if not bp:
            log("Could not load: " + bp_path)
            continue

        bp_name = bp_path.split("/")[-1]
        log(bp_name + ":")
        log("  Object type: " + str(type(bp)))
        log("  Class: " + str(bp.get_class().get_name()))

        # Use C++ helper to get SCS components
        comps = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
        log("  SCS components (via C++): " + str(len(comps)))
        for comp in comps:
            log("    - " + comp)

    log("=" * 50)

inspect()
