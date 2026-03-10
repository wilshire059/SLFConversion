"""
Validate NPC system Blueprint compilation status.
Uses SLFAutomationLibrary for proper compile status checking.
"""
import unreal

NPC_PATHS = [
    # NPC Characters
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
    # NPC Component
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager",
    # Dialog/Vendor Data Assets
    "/Game/SoulslikeFramework/Data/Dialog/PDA_Dialog",
    "/Game/SoulslikeFramework/Data/Vendor/PDA_Vendor",
    # NPC Widgets
    "/Game/SoulslikeFramework/Widgets/HUD/W_Dialog",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window_Vendor",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorSlot",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorAction",
]

def validate_npc_compile():
    """Check compilation status of NPC Blueprints."""
    unreal.log_warning("\n=== NPC SYSTEM COMPILE VALIDATION ===\n")

    total = 0
    success = 0
    failed = 0

    for bp_path in NPC_PATHS:
        total += 1
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"[MISSING] {bp_path}")
            failed += 1
            continue

        bp_name = bp.get_name()

        # Use SLFAutomationLibrary for compile validation
        try:
            result = unreal.SLFAutomationLibrary.validate_blueprint_compiles(bp)
            if result:
                unreal.log_warning(f"[OK] {bp_name}")
                success += 1
            else:
                unreal.log_warning(f"[FAIL] {bp_name}")
                failed += 1
        except Exception as e:
            unreal.log_warning(f"[ERROR] {bp_name} - {e}")
            failed += 1

    unreal.log_warning(f"\n=== RESULTS ===")
    unreal.log_warning(f"Total: {total}")
    unreal.log_warning(f"Success: {success}")
    unreal.log_warning(f"Failed: {failed}")

    if failed == 0:
        unreal.log_warning("ALL NPC BLUEPRINTS COMPILE SUCCESSFULLY!")
    else:
        unreal.log_warning(f"{failed} NPC Blueprints have compile errors")

    unreal.log_warning("================\n")

if __name__ == "__main__":
    validate_npc_compile()
