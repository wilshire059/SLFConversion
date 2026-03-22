"""
Comprehensive fix for B_Chaos_ForceField to use C++ implementations.

The issue: B_Chaos_ForceField has K2Node_Event nodes for EnableChaosDestroy, DisableChaosDestroy,
and ReceiveTick. These override the C++ _Implementation methods.

Solution: Clear ALL event graph nodes including the Event nodes themselves, so C++ handles everything.
"""
import unreal

def main():
    unreal.log_warning("=== COMPREHENSIVE B_CHAOS_FORCEFIELD FIX ===")
    unreal.log_warning("")

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField"
    cpp_parent = "/Script/SLFConversion.B_Chaos_ForceField"

    # Step 1: Check current state
    unreal.log_warning("=== STEP 1: Current State ===")
    bp = unreal.load_asset(bp_path)
    if bp:
        has_eg_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(bp)
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        unreal.log_warning(f"Current Parent: {parent}")
        unreal.log_warning(f"Has EventGraph Logic: {has_eg_logic}")
    else:
        unreal.log_error(f"Failed to load Blueprint: {bp_path}")
        return

    unreal.log_warning("")

    # Step 2: Clear ALL event graph nodes (including Event nodes)
    unreal.log_warning("=== STEP 2: Clear ALL EventGraph Nodes ===")
    clear_result = unreal.SLFAutomationLibrary.clear_all_event_graph_nodes(bp_path, True)  # True = include event nodes
    unreal.log_warning(f"Clear Result:\n{clear_result}")

    unreal.log_warning("")

    # Step 3: Verify parent is correct
    unreal.log_warning("=== STEP 3: Verify Parent ===")
    bp = unreal.load_asset(bp_path)
    if bp:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        unreal.log_warning(f"Parent after clear: {parent}")

        # If parent is not correct, reparent
        if cpp_parent not in parent:
            unreal.log_warning(f"Parent incorrect, reparenting to {cpp_parent}...")
            reparent_result = unreal.SLFAutomationLibrary.force_reparent_and_validate(bp_path, cpp_parent)
            unreal.log_warning(f"Reparent Result:\n{reparent_result}")

    unreal.log_warning("")

    # Step 4: Final validation
    unreal.log_warning("=== STEP 4: Final Validation ===")
    bp = unreal.load_asset(bp_path)
    if bp:
        has_eg_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(bp)
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        unreal.log_warning(f"Final Parent: {parent}")
        unreal.log_warning(f"Final Has EventGraph Logic: {has_eg_logic}")

        if cpp_parent in parent and not has_eg_logic:
            unreal.log_warning("")
            unreal.log_warning("=== SUCCESS: B_Chaos_ForceField is now fully using C++ ===")
        else:
            unreal.log_error("=== ISSUE: Check results above ===")

    unreal.log_warning("")
    unreal.log_warning("=== DONE ===")

if __name__ == "__main__":
    main()
