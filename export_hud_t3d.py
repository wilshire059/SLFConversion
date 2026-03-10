"""
Export W_HUD to T3D format to see widget configuration.
"""
import unreal
import os

OUTPUT_FILE = "C:/scripts/slfconversion/migration_cache/W_HUD_export.t3d"

def export_hud():
    hud_path = "/Game/SoulslikeFramework/Widgets/HUD/W_HUD"
    hud = unreal.EditorAssetLibrary.load_asset(hud_path)

    if not hud:
        print(f"ERROR: Could not load {hud_path}")
        return

    # Export to text
    export_task = unreal.AssetExportTask()
    export_task.object = hud
    export_task.filename = OUTPUT_FILE
    export_task.exporter = None
    export_task.automated = True
    export_task.prompt = False
    export_task.write_empty_files = False

    if unreal.Exporter.run_asset_export_task(export_task):
        print(f"Exported to {OUTPUT_FILE}")
    else:
        print("Export failed")

if __name__ == "__main__":
    export_hud()
