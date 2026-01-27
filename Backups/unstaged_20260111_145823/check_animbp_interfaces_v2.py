# check_animbp_interfaces_v2.py
# Check AnimBP interface implementation via automation

import unreal

ANIMBP = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def log(msg):
    print(msg)
    unreal.log(msg)

def main():
    log("=== Checking AnimBP via Automation ===")

    asset = unreal.EditorAssetLibrary.load_asset(ANIMBP)
    if not asset:
        log("Failed to load AnimBP")
        return

    # Use automation library to diagnose
    automation_lib = unreal.SLFAutomationLibrary
    diagnosis = automation_lib.diagnose_anim_graph_nodes(asset)
    log(diagnosis)

if __name__ == "__main__":
    main()
