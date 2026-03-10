"""
Remove implemented interfaces and clear all event nodes from NPC Blueprints.
"""
import unreal

NPC_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager",
]


def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("REMOVING INTERFACES AND CLEARING ALL EVENT NODES FROM NPC BLUEPRINTS")
    unreal.log_warning("="*70 + "\n")

    for bp_path in NPC_BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        unreal.log_warning(f"\n{bp_name}:")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"  [FAIL] Could not load Blueprint")
            continue

        try:
            # Remove implemented interfaces
            removed = unreal.SLFAutomationLibrary.remove_implemented_interfaces(bp)
            unreal.log_warning(f"  Removed {removed} interfaces")

            # Clear ALL event graph nodes including Event nodes
            result = unreal.SLFAutomationLibrary.clear_all_event_graph_nodes(bp_path, True)
            unreal.log_warning(f"  ClearAllEventGraphNodes: {result}")

            # Clear function graphs
            cleared = unreal.SLFAutomationLibrary.clear_function_graphs(bp)
            unreal.log_warning(f"  Cleared {cleared} function graphs")

            # Remove ALL variables
            removed_vars = unreal.SLFAutomationLibrary.remove_all_variables(bp)
            unreal.log_warning(f"  Removed {removed_vars} variables")

            # Compile and save
            unreal.SLFAutomationLibrary.compile_and_save(bp)
            unreal.log_warning(f"  [OK] Compiled and saved")

            # Check for errors
            errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
            if errors and "Error" in errors:
                unreal.log_warning(f"  STILL HAS ERRORS: {errors[:500]}")
        except Exception as e:
            unreal.log_warning(f"  [ERROR] {e}")

    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("COMPLETE")
    unreal.log_warning("="*70 + "\n")


if __name__ == "__main__":
    main()
