import unreal
import subprocess
import os

# Export W_Resources to T3D format to see raw widget properties
bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"
output_dir = "C:/scripts/SLFConversion/migration_cache"

# Load and export
bp = unreal.load_asset(bp_path)
if bp:
    # Try to export to text
    text = unreal.EditorAssetLibrary.export_asset_with_selected_exporter(
        bp_path,
        os.path.join(output_dir, "W_Resources_export.t3d"),
        True  # bForceAllowOverwrite
    )
    unreal.log_warning(f"Export result: {text}")

    # Also try just getting the exported text directly
    try:
        # Alternative: Use export_to_file
        obj = unreal.load_object(None, bp_path)
        if obj:
            unreal.log_warning(f"Loaded object: {obj.get_class().get_name()}")
    except Exception as e:
        unreal.log_warning(f"Load object error: {e}")
else:
    unreal.log_error("Could not load asset")
