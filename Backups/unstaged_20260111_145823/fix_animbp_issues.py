# fix_animbp_issues.py
# Apply fixes to AnimBPs using C++ automation functions

import unreal

ANIMBPS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
]

# Variables that need "?" suffix removed
VARIABLE_RENAMES = [
    ("IsGuarding?", "IsGuarding"),
    ("bIsAccelerating?", "bIsAccelerating"),
    ("bIsBlocking?", "bIsBlocking"),
    ("bIsFalling?", "bIsFalling"),
    ("IsResting?", "IsResting"),
]

# LinkedAnimLayer configurations
LINKED_ANIM_LAYERS = [
    ("ALI_LocomotionStates", "/Game/SoulslikeFramework/Blueprints/_Characters/Animation/LinkedAnimGraphs/ABP_LocomotionStates"),
    ("ALI_OverlayStates", "/Game/SoulslikeFramework/Blueprints/_Characters/Animation/LinkedAnimGraphs/ABP_OverlayStates"),
]

def log(msg):
    print(f"[Fix AnimBP] {msg}")
    unreal.log(f"[Fix AnimBP] {msg}")

def main():
    automation_lib = unreal.SLFAutomationLibrary
    total_fixes = 0

    for asset_path in ANIMBPS:
        asset_name = asset_path.split("/")[-1]
        log(f"\n=== Processing: {asset_name} ===")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log(f"  ERROR: Could not load {asset_path}")
            continue

        # Step 1: Diagnose current state
        log(f"  Diagnosing...")
        diagnosis = automation_lib.diagnose_anim_graph_nodes(asset)
        log(f"  {diagnosis[:500]}...")  # Print first 500 chars

        # Step 2: Fix Property Access paths
        for old_name, new_name in VARIABLE_RENAMES:
            log(f"  Fixing Property Access: {old_name} -> {new_name}")
            fixed = automation_lib.fix_property_access_paths(asset, old_name, new_name)
            if fixed > 0:
                log(f"    Fixed {fixed} references")
                total_fixes += fixed

        # Step 3: Fix LinkedAnimLayer nodes
        for interface_name, linked_path in LINKED_ANIM_LAYERS:
            log(f"  Fixing LinkedAnimLayer: {interface_name}")
            fixed = automation_lib.fix_linked_anim_layers(asset, interface_name, linked_path)
            if fixed > 0:
                log(f"    Fixed {fixed} nodes")
                total_fixes += fixed

        # Step 4: Save the asset
        log(f"  Saving...")
        automation_lib.compile_and_save(asset)

    log(f"\n=== COMPLETE ===")
    log(f"Total fixes applied: {total_fixes}")

if __name__ == "__main__":
    main()
