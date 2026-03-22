"""
Migrate NPC Blueprints to C++ parent classes using SLFAutomationLibrary.
Run on SLFConversion project.
"""
import unreal

# NPC migration map: Blueprint path -> C++ parent class path
NPC_MIGRATION_MAP = {
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC": "/Script/SLFConversion.SLFSoulslikeNPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide": "/Script/SLFConversion.SLFNPCShowcaseGuide",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor": "/Script/SLFConversion.SLFNPCShowcaseVendor",
}

def migrate_npcs():
    """Reparent NPC Blueprints to C++ classes using SLFAutomationLibrary."""
    unreal.log_warning("\n=== Migrating NPC Blueprints ===\n")

    for bp_path, target_cpp in NPC_MIGRATION_MAP.items():
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"MISSING: {bp_path}")
            continue

        bp_name = bp.get_name()
        unreal.log_warning(f"Processing: {bp_name}")

        # Use SLFAutomationLibrary to reparent (better handling)
        try:
            success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, target_cpp)
            if success:
                unreal.log_warning(f"  REPARENTED: {bp_name} -> {target_cpp}")
            else:
                unreal.log_warning(f"  REPARENT FAILED: {bp_name}")
        except Exception as e:
            unreal.log_warning(f"  REPARENT ERROR: {bp_name} - {e}")

        # Clear EventGraph
        try:
            success = unreal.SLFAutomationLibrary.clear_event_graph(bp)
            if success:
                unreal.log_warning(f"  CLEARED EventGraph: {bp_name}")
            else:
                unreal.log_warning(f"  CLEAR EventGraph FAILED: {bp_name}")
        except Exception as e:
            unreal.log_warning(f"  CLEAR ERROR: {bp_name} - {e}")

        # Save
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning(f"  SAVED: {bp_name}")
        except Exception as e:
            unreal.log_warning(f"  SAVE ERROR: {bp_name} - {e}")

if __name__ == "__main__":
    migrate_npcs()
    unreal.log_warning("\n=== NPC Migration Complete ===")
