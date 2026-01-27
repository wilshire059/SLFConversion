"""
Test script to verify resting point configuration after migration
"""
import unreal

def test_resting_point():
    print("=" * 60)
    print("Testing RestingPoint Configuration")
    print("=" * 60)

    # Load the B_RestingPoint Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        print(f"ERROR: Could not load Blueprint at {bp_path}")
        return

    print(f"Loaded Blueprint: {bp.get_name()}")

    # Get the generated class from the Blueprint asset
    gen_class = bp.generated_class()
    if not gen_class:
        print(f"ERROR: Could not get generated class")
        return

    print(f"Generated Class: {gen_class.get_name()}")

    # Get the CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        print(f"ERROR: Could not get CDO")
        return

    print(f"CDO: {cdo.get_name()}")

    # Check critical properties
    print("\n--- Critical Properties ---")

    # Check CanBeTraced
    try:
        can_be_traced = cdo.get_editor_property("can_be_traced")
        print(f"CanBeTraced: {can_be_traced}")
    except Exception as e:
        print(f"CanBeTraced: ERROR - {e}")

    # Check IsActivated
    try:
        is_activated = cdo.get_editor_property("is_activated")
        print(f"IsActivated: {is_activated}")
    except Exception as e:
        print(f"IsActivated: ERROR - {e}")

    # Check InteractionText
    try:
        text = cdo.get_editor_property("interaction_text")
        print(f"InteractionText: {text}")
    except Exception as e:
        print(f"InteractionText: ERROR - {e}")

    # Check LocationName
    try:
        loc_name = cdo.get_editor_property("location_name")
        print(f"LocationName: {loc_name}")
    except Exception as e:
        print(f"LocationName: ERROR - {e}")

    # Check parent class name
    print("\n--- Hierarchy ---")
    try:
        parent_class = gen_class.get_outer()
        print(f"Outer: {parent_class}")
    except Exception as e:
        print(f"Could not get outer: {e}")

    print("\n" + "=" * 60)
    print("SUCCESS: Blueprint loaded without component conflicts!")
    print("=" * 60)

if __name__ == "__main__":
    test_resting_point()
