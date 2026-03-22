"""
Check NPC Blueprint components to verify AC_AI_InteractionManager exists.
Run on SLFConversion project.
"""
import unreal

NPC_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
]

def check_npc_components():
    """Check NPC Blueprint components using export_text."""
    unreal.log_warning("\n=== Checking NPC Components ===\n")

    for bp_path in NPC_PATHS:
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"MISSING: {bp_path}")
            continue

        bp_name = bp.get_name()
        unreal.log_warning(f"\n--- {bp_name} ---")

        # Export text and search for AC_AI_InteractionManager
        export_text = unreal.EditorAssetLibrary.export_text(bp_path)

        # Check for component references
        if "AC_AI_InteractionManager" in export_text:
            unreal.log_warning(f"  AC_AI_InteractionManager: FOUND in export")
            # Find the line
            for line in export_text.split('\n'):
                if "AC_AI_InteractionManager" in line and "Begin Object" in line:
                    unreal.log_warning(f"    {line.strip()}")
        else:
            unreal.log_warning(f"  AC_AI_InteractionManager: NOT FOUND!")

        # Check for LookAtRadius/SphereComponent
        if "Look At Radius" in export_text or "LookAtRadius" in export_text:
            unreal.log_warning(f"  LookAtRadius: FOUND")
        else:
            unreal.log_warning(f"  LookAtRadius: NOT FOUND")

        # Check AnimBlueprint
        if "AnimBlueprintGeneratedClass" in export_text or "ABP_Soulslike" in export_text:
            unreal.log_warning(f"  AnimBlueprint reference: FOUND")
            for line in export_text.split('\n'):
                if "AnimClass" in line or "AnimBlueprintGeneratedClass" in line:
                    unreal.log_warning(f"    {line.strip()[:200]}")
        else:
            unreal.log_warning(f"  AnimBlueprint reference: NOT FOUND - THIS CAUSES A-POSE!")

if __name__ == "__main__":
    check_npc_components()
