# check_action_struct_types.py
# Diagnostic: Check what struct types are stored in FInstancedStruct RelevantData
# Uses export_text() to examine the serialized data since assets are Blueprint-derived

import unreal
import re

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"

def check_action_struct_types():
    """
    Check the struct type stored in FInstancedStruct RelevantData for each action data asset.
    Since PDA_Action is still Blueprint-based, we use export_text() to examine struct types.
    """
    lines = []
    lines.append("=" * 70)
    lines.append("CHECKING FInstancedStruct STRUCT TYPES IN ACTION DATA ASSETS")
    lines.append("=" * 70)
    lines.append("")
    lines.append("Expected after Core Redirect: /Script/SLFConversion.FSLF*")
    lines.append("Before Core Redirect: /Game/SoulslikeFramework/Structures/...")
    lines.append("")

    actions_path = "/Game/SoulslikeFramework/Data/Actions/ActionData"
    assets = unreal.EditorAssetLibrary.list_assets(actions_path, recursive=True, include_folder=False)

    cpp_count = 0
    bp_count = 0
    empty_count = 0
    error_count = 0

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]

        # Skip the parent PDA_Action Blueprint
        if asset_name == "PDA_Action":
            lines.append(f"  {asset_name}: [SKIP] Parent Blueprint")
            continue

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            lines.append(f"  {asset_name}: [ERROR] Could not load")
            error_count += 1
            continue

        try:
            asset_class = asset.get_class()
            lines.append(f"  {asset_name}:")
            lines.append(f"      Class: {asset_class.get_name()}")

            # Use export_text() on the whole asset to see serialized form
            if hasattr(asset, 'export_text'):
                export = asset.export_text()

                # Look for InstancedStruct type reference pattern
                # Format: ScriptStruct=/Game/... or ScriptStruct=/Script/...
                struct_match = re.search(r'ScriptStruct=([^\s,\)]+)', export)
                if struct_match:
                    struct_path = struct_match.group(1)
                    lines.append(f"      InstancedStruct type: {struct_path}")

                    if "/Script/SLFConversion" in struct_path:
                        lines.append(f"      Status: [C++] Core Redirect applied!")
                        cpp_count += 1
                    elif "/Game/" in struct_path:
                        lines.append(f"      Status: [BP] Needs re-save to apply Core Redirect")
                        bp_count += 1
                    else:
                        lines.append(f"      Status: [???] Unknown path format")
                else:
                    # Check if there's any RelevantData content
                    if "RelevantData" in export:
                        # Extract the RelevantData section
                        rd_match = re.search(r'RelevantData=\(([^)]*)\)', export)
                        if rd_match:
                            rd_content = rd_match.group(1)
                            if rd_content.strip():
                                lines.append(f"      RelevantData: {rd_content[:100]}...")
                                bp_count += 1
                            else:
                                lines.append(f"      RelevantData: [EMPTY]")
                                empty_count += 1
                        else:
                            lines.append(f"      RelevantData: [EMPTY] No struct set")
                            empty_count += 1
                    else:
                        lines.append(f"      RelevantData: [NOT FOUND]")
                        empty_count += 1
            else:
                lines.append(f"      [ERROR] No export_text method")
                error_count += 1

        except Exception as e:
            lines.append(f"  {asset_name}: [ERROR] {e}")
            error_count += 1

    lines.append("")
    lines.append("=" * 70)
    lines.append("SUMMARY")
    lines.append("=" * 70)
    lines.append(f"  C++ structs (Core Redirect applied): {cpp_count}")
    lines.append(f"  Blueprint structs (needs re-save): {bp_count}")
    lines.append(f"  Empty (no RelevantData): {empty_count}")
    lines.append(f"  Errors: {error_count}")
    lines.append("")

    if bp_count > 0:
        lines.append("ACTION REQUIRED: Run resave_action_assets.py to apply Core Redirects")
    elif cpp_count > 0:
        lines.append("SUCCESS: All action assets are using C++ structs!")
    else:
        lines.append("INFO: No RelevantData found in action assets (may be expected)")

    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))

    print(f"Output written to {OUTPUT_FILE}")
    print(f"C++: {cpp_count}, BP: {bp_count}, Empty: {empty_count}, Errors: {error_count}")


if __name__ == "__main__":
    check_action_struct_types()
