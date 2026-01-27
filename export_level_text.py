"""
Export level to text format to find B_StatusEffectArea instances.
"""

import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/level_export.txt"

def log(msg):
    print(msg)
    unreal.log(msg)

def export_level():
    """Export level using asset export."""

    level_path = "/Game/SoulslikeFramework/Demo/_Levels/L_Demo_Showcase"
    log(f"Exporting level: {level_path}")

    # Load the level asset
    level_asset = unreal.EditorAssetLibrary.load_asset(level_path)
    if not level_asset:
        log(f"Failed to load level asset: {level_path}")
        return

    log(f"Loaded: {level_asset.get_class().get_name()}")

    # Try to export to text
    output_dir = os.path.dirname(OUTPUT_FILE)
    os.makedirs(output_dir, exist_ok=True)

    # Use export_text to get Blueprint/level data
    try:
        # Get the outer (World)
        world = level_asset
        log(f"World class: {world.get_class().get_name()}")

        # Try to iterate through levels in the world
        if hasattr(world, 'get_editor_property'):
            # Get persistent level
            try:
                levels = world.get_editor_property('levels')
                log(f"Levels: {levels}")
            except Exception as e:
                log(f"Could not get levels: {e}")

        # Export via AssetExportTask
        task = unreal.AssetExportTask()
        task.object = world
        task.filename = OUTPUT_FILE.replace('.txt', '_world.t3d')
        task.automated = True
        task.prompt = False
        task.replace_identical = True

        result = unreal.Exporter.run_asset_export_task(task)
        log(f"Export result: {result}")

        if os.path.exists(task.filename):
            log(f"Exported to: {task.filename}")
            with open(task.filename, 'r', errors='ignore') as f:
                content = f.read()
                # Search for StatusEffectArea
                if "StatusEffectArea" in content:
                    log("Found StatusEffectArea references!")
                    # Extract relevant sections
                    lines = content.split('\n')
                    for i, line in enumerate(lines):
                        if "StatusEffectArea" in line or "StatusEffect" in line.split('=')[-1] if '=' in line else False:
                            log(f"  Line {i}: {line[:200]}")

    except Exception as e:
        log(f"Export error: {e}")
        import traceback
        traceback.print_exc()

# Run
export_level()
