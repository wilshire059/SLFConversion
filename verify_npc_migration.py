"""
Verify NPC Blueprints are reparented to C++ classes.
Run on SLFConversion project.
"""
import unreal

NPC_MAP = {
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC": "SLFSoulslikeNPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide": "SLFNPCShowcaseGuide",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor": "SLFNPCShowcaseVendor",
}

def verify_npcs():
    """Verify NPC Blueprint parents."""
    unreal.log_warning("=== Verifying NPC Migration ===")

    all_ok = True
    for bp_path, expected_parent in NPC_MAP.items():
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"MISSING: {bp_path}")
            all_ok = False
            continue

        bp_name = bp.get_name()

        # Use SLFAutomationLibrary.get_blueprint_parent_class to get parent class name
        # Returns full path like "/Script/SLFConversion.SLFSoulslikeNPC"
        parent_path = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        # Check if expected parent is at the end of the path
        if parent_path.endswith(expected_parent):
            unreal.log_warning(f"OK: {bp_name} -> {expected_parent}")
        else:
            unreal.log_warning(f"WRONG: {bp_name} -> {parent_path} (expected {expected_parent})")
            all_ok = False

    if all_ok:
        unreal.log_warning("=== All NPCs Migrated Successfully ===")
    else:
        unreal.log_warning("=== Some NPCs Have Issues ===")

if __name__ == "__main__":
    verify_npcs()
