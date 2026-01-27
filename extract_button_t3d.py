import unreal
import os

# Export W_GenericButton as T3D to see CDO values
bp_path = "/Game/SoulslikeFramework/Widgets/_Generic/W_GenericButton"
output_path = "C:/scripts/SLFConversion/migration_cache/w_generic_button.t3d"

bp = unreal.load_asset(bp_path)
if bp:
    result = unreal.EditorAssetLibrary.export_text(bp_path)
    if result:
        with open(output_path, "w", encoding="utf-8") as f:
            f.write(result)
        print(f"Exported to {output_path}")
        print(f"Length: {len(result)} chars")
    else:
        print("Export failed")
else:
    print(f"Could not load {bp_path}")
