"""
Ladder System Validation using SLFAutomationLibrary (C++)
"""
import unreal

def validate_ladder():
    """Validate ladder system using C++ automation"""
    print("\n" + "="*60)
    print("LADDER SYSTEM VALIDATION (Using C++ SLFAutomationLibrary)")
    print("="*60)

    # Paths
    b_ladder_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Ladder"
    ac_ladder_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_LadderManager"

    # Expected C++ parents
    expected_b_ladder_parent = "/Script/SLFConversion.B_Ladder"  # AB_Ladder class
    expected_ac_ladder_parent = "/Script/SLFConversion.LadderManagerComponent"

    # ==================== B_Ladder ====================
    print("\n--- B_Ladder Validation ---")

    bp = unreal.load_asset(b_ladder_path)
    if bp:
        # Get parent class using C++ library
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        print(f"Current Parent: {parent}")

        # Get SCS components
        components = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
        print(f"SCS Components ({len(components)}):")
        for comp in components:
            print(f"  - {comp}")

        # Check if event graph has logic
        has_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(bp)
        print(f"Has EventGraph Logic: {has_logic}")

        # Get functions
        functions = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
        print(f"Functions ({len(functions)}):")
        for func in functions:
            print(f"  - {func}")

        # Check if parent is correct
        if "B_Ladder" in parent or "AB_Ladder" in parent:
            print("PARENT CHECK: OK - Using C++ AB_Ladder class")
        elif "B_Interactable" in parent:
            print("PARENT CHECK: FAIL - Still using Blueprint parent B_Interactable_C")
            print("  Need to reparent to AB_Ladder C++ class")
    else:
        print("ERROR: Could not load B_Ladder")

    # ==================== AC_LadderManager ====================
    print("\n--- AC_LadderManager Validation ---")

    bp2 = unreal.load_asset(ac_ladder_path)
    if bp2:
        parent2 = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp2)
        print(f"Current Parent: {parent2}")

        has_logic2 = unreal.SLFAutomationLibrary.has_event_graph_logic(bp2)
        print(f"Has EventGraph Logic: {has_logic2}")

        functions2 = unreal.SLFAutomationLibrary.get_blueprint_functions(bp2)
        print(f"Functions ({len(functions2)}):")
        for func in functions2:
            print(f"  - {func}")

        # Check if parent is correct
        if "LadderManager" in parent2:
            print("PARENT CHECK: OK - Using C++ LadderManagerComponent")
        elif "ActorComponent" in parent2:
            print("PARENT CHECK: FAIL - Still using native ActorComponent")
            print("  Need to reparent to ULadderManagerComponent C++ class")
    else:
        print("ERROR: Could not load AC_LadderManager")

    # ==================== Full Validation ====================
    print("\n--- Full Migration Validation ---")

    # Validate B_Ladder
    result = unreal.SLFAutomationLibrary.validate_blueprint_migration(
        b_ladder_path,
        expected_b_ladder_parent,
        True,   # expect cleared event graphs
        True    # expect cleared functions
    )
    print(f"\nB_Ladder Validation Result:\n{result}")

    # Validate AC_LadderManager
    result2 = unreal.SLFAutomationLibrary.validate_blueprint_migration(
        ac_ladder_path,
        expected_ac_ladder_parent,
        True,
        True
    )
    print(f"\nAC_LadderManager Validation Result:\n{result2}")

    print("\n" + "="*60)
    print("VALIDATION COMPLETE")
    print("="*60)

if __name__ == "__main__":
    validate_ladder()
