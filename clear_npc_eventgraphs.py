"""
Clear event graphs on NPC system Blueprints after reparenting to C++.

The C++ classes implement all the logic, so Blueprint event graphs
should be cleared to remove stale logic causing compile errors.
"""
import unreal

NPC_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager",
]


def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("CLEARING EVENT GRAPHS ON NPC SYSTEM BLUEPRINTS")
    unreal.log_warning("="*70 + "\n")

    succeeded = 0
    failed = 0

    for bp_path in NPC_BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        unreal.log_warning(f"\n{bp_name}:")

        # Load the Blueprint
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"  [FAIL] Could not load Blueprint")
            failed += 1
            continue

        try:
            # Clear event graphs using C++ function
            cleared_count = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
            unreal.log_warning(f"  Cleared {cleared_count} event graph(s)")

            # Compile and save
            unreal.SLFAutomationLibrary.compile_and_save(bp)
            unreal.log_warning(f"  [OK] Compiled and saved")
            succeeded += 1
        except Exception as e:
            unreal.log_warning(f"  [ERROR] {e}")
            failed += 1

    unreal.log_warning("\n" + "="*70)
    unreal.log_warning(f"COMPLETE: {succeeded} succeeded, {failed} failed")
    unreal.log_warning("="*70 + "\n")


if __name__ == "__main__":
    main()
