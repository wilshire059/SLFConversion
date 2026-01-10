# diagnose_property_access.py
# Check property access paths in ABP_SoulslikeCharacter_Additive

import unreal

ANIMBP = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def log(msg):
    print(msg)
    unreal.log(msg)

def main():
    log("=== Property Access Diagnosis ===")

    asset = unreal.EditorAssetLibrary.load_asset(ANIMBP)
    if not asset:
        log(f"ERROR: Failed to load {ANIMBP}")
        return

    log(f"\nAnimBP: {asset.get_name()}")

    # Get all graphs and look for PropertyAccess nodes
    anim_bp = unreal.AnimBlueprint.cast(asset)
    if not anim_bp:
        log("ERROR: Not an AnimBlueprint")
        return

    # Get the Blueprint graph and search for property access nodes
    log("\n=== Searching for PropertyAccess nodes ===")

    # Use the automation library to diagnose anim graph nodes
    diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_graph_nodes(asset)
    log(diagnosis)

if __name__ == "__main__":
    main()
