# check_animbp_interfaces.py
# Check which interfaces AnimBPs implement

import unreal

ANIMBPS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
]

def log(msg):
    print(msg)
    unreal.log(msg)

def main():
    log("=== Checking AnimBP Interfaces ===")

    for asset_path in ANIMBPS:
        asset_name = asset_path.split("/")[-1]

        # Load the asset
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log(f"[FAIL] {asset_name}: Could not load")
            continue

        blueprint = asset

        # Check implemented interfaces
        log(f"\n{asset_name}:")
        log(f"  Parent Class: {blueprint.parent_class.get_name() if blueprint.parent_class else 'None'}")

        # Get implemented interfaces
        try:
            implemented = blueprint.implemented_interfaces
            if implemented:
                for iface in implemented:
                    log(f"  Interface: {iface.get_name()}")
            else:
                log(f"  No implemented interfaces")
        except Exception as e:
            log(f"  Error getting interfaces: {e}")

        # Check blueprint graphs for linked layers
        log(f"\n  Graphs in Blueprint:")
        try:
            for graph in blueprint.ubergraph_pages:
                log(f"    UberGraph: {graph.get_name()}")
        except:
            pass

        try:
            for graph in blueprint.function_graphs:
                log(f"    Function: {graph.get_name()}")
        except:
            pass

if __name__ == "__main__":
    main()
