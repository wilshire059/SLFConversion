"""
Check compile errors on NPC system Blueprints.
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
    unreal.log_warning("CHECKING NPC BLUEPRINT COMPILE STATUS")
    unreal.log_warning("="*70 + "\n")

    for bp_path in NPC_BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        unreal.log_warning(f"\n{bp_name}:")

        # Load the Blueprint
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"  [FAIL] Could not load Blueprint")
            continue

        try:
            # Get parent class
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
            unreal.log_warning(f"  Parent: {parent}")

            # Get compile errors
            errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
            if errors and len(errors) > 0:
                unreal.log_warning(f"  Compile errors:")
                for line in errors.split("\n")[:10]:  # Limit to first 10 lines
                    unreal.log_warning(f"    {line}")
            else:
                unreal.log_warning(f"  [OK] No compile errors")
        except Exception as e:
            unreal.log_warning(f"  [ERROR] {e}")

    unreal.log_warning("\n" + "="*70)


if __name__ == "__main__":
    main()
