#!/usr/bin/env python3
"""
Diagnose and fix Level Blueprint SpawnActor node that lost its class reference.
"""

import unreal

def log(msg):
    print(f"[FixLevelBP] {msg}")
    unreal.log_warning(f"[FixLevelBP] {msg}")

def main():
    log("=" * 70)
    log("DIAGNOSING LEVEL BLUEPRINT SPAWNACTOR ISSUE")
    log("=" * 70)

    # Load the level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    log(f"Loading level: {level_path}")

    # Get the level blueprint
    level_asset = unreal.EditorAssetLibrary.load_asset(level_path)
    if not level_asset:
        log(f"ERROR: Could not load level: {level_path}")
        return

    log(f"Level loaded: {level_asset.get_name()}")

    # Check B_SequenceActor class is valid
    log("")
    log("Checking B_SequenceActor class...")
    seq_bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
    seq_bp = unreal.EditorAssetLibrary.load_asset(seq_bp_path)
    if seq_bp:
        log(f"  B_SequenceActor Blueprint: VALID")
        gen_class = seq_bp.generated_class()
        if gen_class:
            log(f"  Generated Class: {gen_class.get_name()}")
            log(f"  Class Path: {gen_class.get_path_name()}")
        else:
            log(f"  ERROR: No generated class!")
    else:
        log(f"  ERROR: Could not load B_SequenceActor")

    # Check if level blueprint exists
    log("")
    log("Checking Level Blueprint...")

    # The level blueprint is embedded in the level
    # We need to find and examine it
    try:
        # Get all blueprints in the level
        subsystem = unreal.get_editor_subsystem(unreal.AssetEditorSubsystem)
        if subsystem:
            log(f"  AssetEditorSubsystem available")
    except Exception as e:
        log(f"  Subsystem error: {e}")

    # Try to get the level script blueprint
    log("")
    log("Looking for Level Blueprint graph...")

    # Export the level to text to examine the blueprint
    log("  Exporting level to check blueprint nodes...")

    try:
        # Use export_text approach
        exporter = unreal.LevelExporterT3D()
        export_task = unreal.AssetExportTask()
        export_task.set_editor_property('object', level_asset)
        export_task.set_editor_property('filename', 'C:/scripts/SLFConversion/migration_cache/level_export.t3d')
        export_task.set_editor_property('exporter', exporter)
        export_task.set_editor_property('automated', True)
        export_task.set_editor_property('prompt', False)

        result = unreal.Exporter.run_asset_export_task(export_task)
        log(f"  Export result: {result}")
    except Exception as e:
        log(f"  Export error: {e}")

    # The real fix: Since level blueprints are tricky, let's check if we need to
    # use the editor to manually fix this, or if there's a SLFAutomationLibrary function

    log("")
    log("=" * 70)
    log("RECOMMENDATION:")
    log("The Level Blueprint's SpawnActor node has lost its class reference.")
    log("This typically happens when the target Blueprint is reparented.")
    log("")
    log("To fix this manually in Unreal Editor:")
    log("1. Open L_Demo_Showcase level")
    log("2. Open Level Blueprint (Blueprints > Open Level Blueprint)")
    log("3. Find the SpawnActor node for B_SequenceActor")
    log("4. Re-select the class: B_SequenceActor")
    log("5. Reconnect the 'Sequence to Play' pin to your LevelSequence asset")
    log("6. Compile and Save the Level Blueprint")
    log("=" * 70)

if __name__ == "__main__":
    main()
