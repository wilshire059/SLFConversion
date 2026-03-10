"""
Verify all NPC Blueprints are ready for PIE testing.
"""
import unreal


def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("FINAL NPC BLUEPRINT VERIFICATION")
    unreal.log_warning("="*70 + "\n")

    blueprints = [
        ("/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC", "SLFSoulslikeNPC"),
        ("/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide", "SLFNPCShowcaseGuide"),
        ("/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor", "SLFNPCShowcaseVendor"),
        ("/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager", "AIInteractionManagerComponent"),
    ]

    all_ok = True
    for bp_path, expected_parent in blueprints:
        bp_name = bp_path.split("/")[-1]
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"[FAIL] {bp_name} - Could not load")
            all_ok = False
            continue

        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        parent_name = parent.split(".")[-1] if "." in parent else parent.split("/")[-1]

        if expected_parent in parent:
            unreal.log_warning(f"[OK] {bp_name}")
            unreal.log_warning(f"     Parent: {parent_name}")
        else:
            unreal.log_warning(f"[WRONG] {bp_name}")
            unreal.log_warning(f"     Expected: {expected_parent}")
            unreal.log_warning(f"     Got: {parent}")
            all_ok = False

    unreal.log_warning("\n" + "="*70)
    if all_ok:
        unreal.log_warning("ALL NPC BLUEPRINTS READY FOR PIE TESTING!")
    else:
        unreal.log_warning("SOME NPC BLUEPRINTS NEED ATTENTION")
    unreal.log_warning("="*70 + "\n")


if __name__ == "__main__":
    main()
