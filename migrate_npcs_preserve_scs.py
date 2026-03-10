"""
Migrate NPC Blueprints to C++ - Reparent and clear EventGraphs.
SCS components should be preserved (they're separate from EventGraph).
Run on SLFConversion project AFTER restoring from bp_only.
"""
import unreal

NPC_MIGRATION_MAP = {
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC": "/Script/SLFConversion.SLFSoulslikeNPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide": "/Script/SLFConversion.SLFNPCShowcaseGuide",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor": "/Script/SLFConversion.SLFNPCShowcaseVendor",
}

def migrate_npcs():
    """Reparent NPC Blueprints to C++ classes and clear EventGraphs."""
    unreal.log_warning("\n=== Migrating NPCs (Reparent + Clear EventGraph) ===\n")

    success_count = 0
    fail_count = 0

    for bp_path, new_parent in NPC_MIGRATION_MAP.items():
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"MISSING: {bp_path}")
            fail_count += 1
            continue

        bp_name = bp.get_name()
        unreal.log_warning(f"Processing: {bp_name}")

        # Reparent using SLFAutomationLibrary
        try:
            result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, new_parent)
            if result:
                unreal.log_warning(f"  REPARENTED: {bp_name} -> {new_parent.split('.')[-1]}")
            else:
                unreal.log_warning(f"  REPARENT FAILED: {bp_name}")
                fail_count += 1
                continue
        except Exception as e:
            unreal.log_warning(f"  REPARENT ERROR: {bp_name} - {e}")
            fail_count += 1
            continue

        # Clear EventGraphs (SCS components should be preserved - they're in SCS, not EventGraph)
        try:
            nodes_cleared = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
            unreal.log_warning(f"  CLEARED EventGraph: {nodes_cleared} nodes")
        except Exception as e:
            unreal.log_warning(f"  CLEAR ERROR: {bp_name} - {e}")

        # Compile
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log_warning(f"  COMPILED: {bp_name}")
        except Exception as e:
            unreal.log_warning(f"  COMPILE WARNING: {bp_name} - {e}")

        # Save
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning(f"  SAVED: {bp_name}")
            success_count += 1
        except Exception as e:
            unreal.log_warning(f"  SAVE ERROR: {bp_name} - {e}")
            fail_count += 1

    unreal.log_warning(f"\n=== Migration Complete: {success_count} succeeded, {fail_count} failed ===")

if __name__ == "__main__":
    migrate_npcs()
