# export_container_t3d.py
# Export B_Container to see all its data

import unreal
import os

def main():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"

    # Export to temp file
    output_dir = "C:/scripts/SLFConversion/migration_cache"
    output_file = os.path.join(output_dir, "b_container_export.txt")

    try:
        result = unreal.AssetExportTask()
        result.set_editor_property("automated", True)
        result.set_editor_property("filename", output_file)
        result.set_editor_property("object", unreal.load_asset(bp_path))
        result.set_editor_property("prompt", False)
        result.set_editor_property("replace_identical", True)

        if unreal.Exporter.run_asset_export_task(result):
            unreal.log_warning(f"Exported to: {output_file}")
        else:
            unreal.log_error("Export failed")

            # Try alternative approach - just dump what we can
            bp = unreal.load_asset(bp_path)
            if bp:
                unreal.log_warning(f"\nBlueprint: {bp.get_name()}")
                unreal.log_warning(f"Class: {bp.get_class().get_name()}")

    except Exception as e:
        unreal.log_error(f"Export error: {e}")

if __name__ == "__main__":
    main()
