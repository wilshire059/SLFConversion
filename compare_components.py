"""Compare components between original and migrated B_PickupItem"""
import unreal

def check_blueprint(path, name):
    """Load and check a blueprint's components using our C++ function"""
    print("")
    print("=== " + name + " ===")
    print("Path: " + path)

    if not unreal.EditorAssetLibrary.does_asset_exist(path):
        print("  ERROR: Asset does not exist!")
        return

    bp = unreal.EditorAssetLibrary.load_asset(path)
    if not bp:
        print("  ERROR: Could not load asset!")
        return

    # Use our C++ function to list SCS components
    components = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
    print("  SCS Components (" + str(len(components)) + "):")
    for comp in components:
        print("    - " + comp)

    if not components:
        print("    (NO SCS COMPONENTS FOUND)")

# Check migrated B_PickupItem
check_blueprint("/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem", "MIGRATED B_PickupItem")

# Check migrated B_Interactable
check_blueprint("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable", "MIGRATED B_Interactable")

print("")
print("=== COMPARISON COMPLETE ===")
