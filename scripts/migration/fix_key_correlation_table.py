"""
Fix KeyCorrelationTable property on W_Inventory widget.

The property is on the C++ class UW_Navigable_InputReader.
We need to set the default value on the Blueprint's generated class CDO.

Run:
UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/fix_key_correlation_table.py"
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/key_correlation_fix_output.txt"
OUTPUT_LINES = []

def log(msg):
    """Log to both Unreal and output file."""
    OUTPUT_LINES.append(str(msg))
    unreal.log_warning(str(msg))

def fix_key_correlation_table():
    """Set KeyCorrelationTable on navigable input reader widgets."""

    log("=" * 60)
    log("FIXING KeyCorrelationTable PROPERTY")
    log("=" * 60)

    # Load the data table
    data_table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_KeyMappingCorrelation.DT_KeyMappingCorrelation"
    data_table = unreal.EditorAssetLibrary.load_asset(data_table_path)

    if not data_table:
        log(f"ERROR: Could not load data table: {data_table_path}")
        return

    log(f"Loaded data table: {data_table.get_name()}")

    # Widget Blueprint paths
    widget_paths = [
        "/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory.W_Inventory",
    ]

    success_count = 0

    for widget_path in widget_paths:
        log(f"")
        log(f"Checking: {widget_path}")

        # Load the widget blueprint
        widget_bp = unreal.EditorAssetLibrary.load_asset(widget_path)
        if not widget_bp:
            log(f"  Could not load widget blueprint")
            continue

        log(f"  Loaded Blueprint: {widget_bp.get_name()}")

        # Get the generated class from the blueprint
        try:
            # For widget blueprints, we need to get the default object
            # The blueprint IS a WidgetBlueprint, which has a GeneratedClass
            gen_class = widget_bp.generated_class()
            if gen_class:
                log(f"  Generated class: {gen_class.get_name()}")

                # Get the CDO (Class Default Object)
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    log(f"  CDO: {cdo.get_name()}")

                    # Try to get current value
                    try:
                        current_table = cdo.get_editor_property('key_correlation_table')
                        log(f"  Current KeyCorrelationTable: {current_table}")

                        if current_table is None:
                            log(f"  Setting KeyCorrelationTable to {data_table.get_name()}")
                            cdo.set_editor_property('key_correlation_table', data_table)

                            # Mark the blueprint as modified and save
                            widget_bp.modify()
                            unreal.EditorAssetLibrary.save_loaded_asset(widget_bp)
                            log(f"  SUCCESS - Saved")
                            success_count += 1
                        else:
                            log(f"  Already set to: {current_table.get_name()}")

                    except Exception as e:
                        log(f"  Error accessing property: {e}")
                else:
                    log(f"  Could not get CDO")
            else:
                log(f"  No generated class")

        except Exception as e:
            log(f"  Error: {e}")
            import traceback
            log(traceback.format_exc())

    log(f"")
    log("=" * 60)
    log(f"Fixed {success_count} widgets")
    log("=" * 60)

def list_data_table_rows():
    """List rows in the DT_KeyMappingCorrelation data table."""

    log("")
    log("=" * 60)
    log("DATA TABLE ROWS (first 20)")
    log("=" * 60)

    data_table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_KeyMappingCorrelation.DT_KeyMappingCorrelation"
    data_table = unreal.EditorAssetLibrary.load_asset(data_table_path)

    if not data_table:
        log(f"ERROR: Could not load data table")
        return

    # Get row names
    row_names = unreal.DataTableFunctionLibrary.get_data_table_row_names(data_table)
    log(f"Found {len(row_names)} rows:")

    for i, row_name in enumerate(row_names):
        if i >= 20:
            log(f"  ... and {len(row_names) - 20} more")
            break
        log(f"  {row_name}")

if __name__ == "__main__":
    fix_key_correlation_table()
    list_data_table_rows()

    # Write output to file
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(OUTPUT_LINES))
    unreal.log_warning(f"Output written to {OUTPUT_FILE}")
