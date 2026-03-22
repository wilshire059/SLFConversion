"""
Diagnose remaining compile errors in NPC Blueprints.
"""
import unreal


def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("DIAGNOSING NPC BLUEPRINT COMPILE ERRORS")
    unreal.log_warning("="*70 + "\n")

    blueprints = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager",
    ]

    for bp_path in blueprints:
        bp_name = bp_path.split("/")[-1]
        unreal.log_warning(f"\n=== {bp_name} ===")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"  [FAIL] Could not load")
            continue

        # Get parent
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        unreal.log_warning(f"  Parent: {parent}")

        # Get functions
        funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
        unreal.log_warning(f"  Functions: {funcs}")

        # Get variables
        vars_ = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        unreal.log_warning(f"  Variables: {vars_}")

        # Get dispatchers
        dispatchers = unreal.SLFAutomationLibrary.get_blueprint_event_dispatchers(bp)
        unreal.log_warning(f"  Dispatchers: {dispatchers}")

        # Full compile errors
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        unreal.log_warning(f"  Compile errors:\n{errors}")

    unreal.log_warning("\n" + "="*70)


if __name__ == "__main__":
    main()
