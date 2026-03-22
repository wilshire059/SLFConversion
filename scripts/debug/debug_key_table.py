"""
Debug the DT_KeyMappingCorrelation data table to see actual key values.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/key_table_debug.txt"
OUTPUT_LINES = []

def log(msg):
    OUTPUT_LINES.append(str(msg))
    unreal.log_warning(str(msg))

def debug_key_table():
    log("=" * 60)
    log("DEBUGGING DT_KeyMappingCorrelation")
    log("=" * 60)

    data_table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_KeyMappingCorrelation.DT_KeyMappingCorrelation"
    data_table = unreal.EditorAssetLibrary.load_asset(data_table_path)

    if not data_table:
        log("ERROR: Could not load data table")
        return

    log(f"Data table: {data_table.get_name()}")
    log(f"Row struct: {data_table.get_editor_property('row_struct')}")
    log("")

    # Get all row names
    row_names = unreal.DataTableFunctionLibrary.get_data_table_row_names(data_table)
    log(f"Total rows: {len(row_names)}")
    log("")

    # Check specific keyboard keys we care about
    keys_to_check = ["Q", "E", "Enter", "Escape", "A", "D", "Tab", "Backspace", "Space"]

    log("Checking specific keyboard keys:")
    for key_name in keys_to_check:
        found = False
        for row_name in row_names:
            if str(row_name) == key_name:
                found = True
                log(f"  Row '{key_name}': EXISTS")
                break
        if not found:
            log(f"  Row '{key_name}': NOT FOUND")

    log("")
    log("All keyboard-like row names (non-Gamepad, non-Mouse):")
    for row_name in row_names:
        name_str = str(row_name)
        if not name_str.startswith("Gamepad") and not name_str.startswith("Mouse"):
            log(f"  {name_str}")

if __name__ == "__main__":
    debug_key_table()

    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(OUTPUT_LINES))
    unreal.log_warning(f"Output written to {OUTPUT_FILE}")
