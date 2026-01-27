"""
Check and fix B_DeathCurrency parent class.
"""
import unreal

def test():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_DeathCurrency"
    expected_cpp_path = "/Script/SLFConversion.B_DeathCurrency"

    # Load the Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return

    print(f"Blueprint: {bp_path}")
    print(f"Blueprint type: {type(bp)}")

    # Try to reparent to correct C++ class
    correct_parent = unreal.load_class(None, expected_cpp_path)
    if correct_parent:
        print(f"Target parent class: {correct_parent.get_name()}")

        # Use SLFAutomationLibrary to reparent
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, expected_cpp_path)
        if result:
            print("Reparented successfully!")
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print("Saved Blueprint")
        else:
            print("Reparenting returned false (may have failed or already correct)")
    else:
        print(f"Could not load parent class: {expected_cpp_path}")

if __name__ == "__main__":
    test()
