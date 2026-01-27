"""
Check B_Chaos_ForceField Blueprint state using C++ validation.
"""
import unreal

def main():
    unreal.log_warning("=== B_CHAOS_FORCEFIELD STATE CHECK ===")
    unreal.log_warning("")

    # Run comprehensive validation
    result = unreal.SLFAutomationLibrary.validate_blueprint_migration(
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField",
        "/Script/SLFConversion.B_Chaos_ForceField",  # Expected parent
        True,  # bExpectClearedEventGraphs
        True   # bExpectClearedFunctions
    )

    unreal.log_warning(f"Validation Result:\n{result}")

    # Also check if it has event graph logic
    bp = unreal.load_asset("/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField")
    if bp:
        has_eg_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(bp)
        unreal.log_warning(f"Has EventGraph Logic: {has_eg_logic}")

        # Check function graph node counts
        enable_count = unreal.SLFAutomationLibrary.get_function_graph_node_count(bp, "EnableChaosDestroy")
        disable_count = unreal.SLFAutomationLibrary.get_function_graph_node_count(bp, "DisableChaosDestroy")
        tick_count = unreal.SLFAutomationLibrary.get_function_graph_node_count(bp, "ReceiveTick")

        unreal.log_warning(f"EnableChaosDestroy nodes: {enable_count}")
        unreal.log_warning(f"DisableChaosDestroy nodes: {disable_count}")
        unreal.log_warning(f"ReceiveTick nodes: {tick_count}")

    unreal.log_warning("")
    unreal.log_warning("=== DONE ===")

if __name__ == "__main__":
    main()
