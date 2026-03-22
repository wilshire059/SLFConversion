"""
Reparent B_DeathCurrency Blueprint to AB_DeathCurrency C++ class.

The Blueprint was incorrectly parented to ASLFInteractableBase.
It needs to be reparented to AB_DeathCurrency which has the OnInteract_Implementation.
"""
import unreal

def reparent_deathcurrency():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_DeathCurrency"
    cpp_parent_path = "/Script/SLFConversion.B_DeathCurrency"

    # Load the Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load Blueprint at {bp_path}")
        return False

    print(f"Loaded Blueprint: {bp_path}")
    print(f"Blueprint type: {type(bp)}")

    # Get current parent
    if hasattr(bp, 'parent_class'):
        print(f"Current parent: {bp.parent_class.get_name() if bp.parent_class else 'None'}")

    # Load the target C++ parent class
    cpp_parent = unreal.load_class(None, cpp_parent_path)
    if not cpp_parent:
        print(f"ERROR: Could not load C++ class at {cpp_parent_path}")
        return False

    print(f"Target C++ parent: {cpp_parent.get_name()}")

    # Use SLFAutomationLibrary to reparent
    result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent_path)
    if result:
        print(f"Reparented successfully!")

        # Save the Blueprint
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print(f"Saved Blueprint: {bp_path}")
        return True
    else:
        print(f"Reparenting returned false - may already be correct or failed")

        # Check if already correct
        if hasattr(bp, 'parent_class') and bp.parent_class:
            current = bp.parent_class.get_name()
            if current == "B_DeathCurrency":
                print(f"Blueprint is already parented to B_DeathCurrency (AB_DeathCurrency)")
                return True

        return False

if __name__ == "__main__":
    success = reparent_deathcurrency()
    print(f"\nResult: {'SUCCESS' if success else 'FAILED'}")
