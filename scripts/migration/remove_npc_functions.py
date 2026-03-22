"""
Remove specific conflicting functions from NPC Blueprints.
"""
import unreal


def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("REMOVING CONFLICTING FUNCTIONS FROM NPC BLUEPRINTS")
    unreal.log_warning("="*70 + "\n")

    # B_Soulslike_NPC - remove GetLookAtLocation
    bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC"
    unreal.log_warning(f"\nB_Soulslike_NPC:")

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if bp:
        # Get functions first
        funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
        unreal.log_warning(f"  Current functions: {funcs}")

        # Remove GetLookAtLocation
        removed = unreal.SLFAutomationLibrary.remove_function(bp, "GetLookAtLocation")
        unreal.log_warning(f"  Removed GetLookAtLocation: {removed}")

        # Remove TryGetItemInfo if still there
        removed2 = unreal.SLFAutomationLibrary.remove_function(bp, "TryGetItemInfo")
        unreal.log_warning(f"  Removed TryGetItemInfo: {removed2}")

        # Compile and save
        unreal.SLFAutomationLibrary.compile_and_save(bp)

        # Check errors
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if errors and "Error" in errors:
            unreal.log_warning(f"  STILL HAS ERRORS")
        else:
            unreal.log_warning(f"  [OK] No compile errors")

    # AC_AI_InteractionManager - remove GetCurrentDialogEntry
    bp_path2 = "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager"
    unreal.log_warning(f"\nAC_AI_InteractionManager:")

    bp2 = unreal.EditorAssetLibrary.load_asset(bp_path2)
    if bp2:
        # Get functions first
        funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp2)
        unreal.log_warning(f"  Current functions: {funcs}")

        # Remove GetCurrentDialogEntry
        removed = unreal.SLFAutomationLibrary.remove_function(bp2, "GetCurrentDialogEntry")
        unreal.log_warning(f"  Removed GetCurrentDialogEntry: {removed}")

        # Remove ALL variables (they shadow C++ properties)
        removed_vars = unreal.SLFAutomationLibrary.remove_all_variables(bp2)
        unreal.log_warning(f"  Removed {removed_vars} variables")

        # Compile and save
        unreal.SLFAutomationLibrary.compile_and_save(bp2)

        # Check errors
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp2)
        if errors and "Error" in errors:
            unreal.log_warning(f"  STILL HAS ERRORS")
        else:
            unreal.log_warning(f"  [OK] No compile errors")

    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("COMPLETE")
    unreal.log_warning("="*70 + "\n")


if __name__ == "__main__":
    main()
