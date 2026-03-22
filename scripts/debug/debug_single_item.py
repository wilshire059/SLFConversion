# debug_single_item.py
# Debug - try export_text() to get struct contents

import unreal
import re

OUTPUT_FILE = "C:/scripts/SLFConversion/debug_output.txt"

def debug_single_item():
    lines = []
    lines.append("=" * 60)
    lines.append("DEBUGGING DA_Lantern - Using export_text()")
    lines.append("=" * 60)

    asset = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Items/DA_Lantern")
    if not asset:
        lines.append("Could not load DA_Lantern")
        with open(OUTPUT_FILE, 'w') as f:
            f.write('\n'.join(lines))
        return

    lines.append(f"Asset: {asset.get_name()}")
    lines.append(f"Class: {asset.get_class().get_name()}")

    # Get ItemInformation
    try:
        item_info = asset.get_editor_property('ItemInformation')
        lines.append(f"\nItemInformation type: {type(item_info).__name__}")

        # Try export_text
        if hasattr(item_info, 'export_text'):
            try:
                export = item_info.export_text()
                lines.append(f"\nexport_text() result:")
                lines.append(export[:2000] if len(export) > 2000 else export)

                # Try to find WorldNiagaraSystem in the export
                if 'Niagara' in export:
                    lines.append(f"\n--- Found 'Niagara' in export ---")
                    # Find the line containing Niagara
                    for line in export.split(','):
                        if 'Niagara' in line:
                            lines.append(f"  {line.strip()}")
            except Exception as e:
                lines.append(f"export_text() error: {e}")
        else:
            lines.append("No export_text() method")

        # Also try __str__ and __repr__
        lines.append(f"\n__str__: {str(item_info)[:500]}")

        # Try to access the struct using unreal.StructBase methods
        lines.append(f"\n--- StructBase methods ---")
        if hasattr(item_info, 'to_tuple'):
            try:
                lines.append(f"to_tuple(): {item_info.to_tuple()}")
            except Exception as e:
                lines.append(f"to_tuple(): ERROR - {e}")

    except Exception as e:
        lines.append(f"\nError: {e}")
        import traceback
        lines.append(traceback.format_exc())

    # Write to file
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))

    print(f"Debug output written to {OUTPUT_FILE}")


if __name__ == "__main__":
    debug_single_item()
