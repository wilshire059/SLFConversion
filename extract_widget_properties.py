"""
Extract widget instance properties from W_Inventory Blueprint
"""
import unreal
import re

def log(msg):
    unreal.log_warning(str(msg))

def extract_widget_properties():
    log("=" * 80)
    log("EXTRACTING WIDGET PROPERTIES FROM W_INVENTORY")
    log("=" * 80)

    # Load the W_Inventory widget blueprint
    bp_path = '/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory'
    bp = unreal.load_asset(bp_path)

    if not bp:
        log(f"ERROR: Failed to load {bp_path}")
        return

    log(f"Loaded: {bp.get_name()}")
    log(f"Class: {bp.get_class().get_name()}")

    # Try to use the Exporter
    log("Trying to export...")
    try:
        # Use unreal.Exporter to export to text
        export_task = unreal.AssetExportTask()
        export_task.object = bp
        export_task.filename = 'C:/scripts/slfconversion/w_inventory_export.txt'
        export_task.automated = True
        export_task.prompt = False
        export_task.replace_identical = True

        result = unreal.Exporter.run_asset_export_task(export_task)
        log(f"Export result: {result}")

        if result:
            # Read the exported file
            with open('C:/scripts/slfconversion/w_inventory_export.txt', 'r', encoding='utf-8') as f:
                content = f.read()
            log(f"Read {len(content)} chars from export file")

            # Search for patterns
            lines = content.split('\n')
            for i, line in enumerate(lines):
                if 'AllItemsCategoryEntry' in line or 'T_Category' in line or 'InventoryCategoryData' in line or 'AllCategoriesButton' in line:
                    log(f"Line {i}: {line[:300]}")
    except Exception as e:
        log(f"Export error: {e}")

    # Try a different approach - use the object's export_text_item
    log("=" * 80)
    log("TRYING DIRECT PROPERTY ACCESS")
    log("=" * 80)

    try:
        # List all available methods/attributes on the blueprint
        attrs = [a for a in dir(bp) if not a.startswith('_')]
        log(f"Blueprint attributes: {attrs[:20]}...")

        # Try to get generated class
        gen_class = bp.generated_class()
        if gen_class:
            log(f"Generated class: {gen_class}")

            # Get the CDO
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                log(f"CDO: {cdo}")
                log(f"CDO class: {cdo.get_class().get_name()}")

                # Try to find AllItemsCategoryEntry on CDO
                try:
                    all_items = cdo.get_editor_property('AllItemsCategoryEntry')
                    log(f"AllItemsCategoryEntry: {all_items}")
                    if all_items:
                        # Get its InventoryCategoryData
                        try:
                            data = all_items.get_editor_property('inventory_category_data')
                            log(f"  InventoryCategoryData: {data}")
                            if data:
                                cat = data.get_editor_property('category')
                                log(f"    category: {cat}")
                                icon = data.get_editor_property('category_icon')
                                log(f"    category_icon: {icon}")
                        except Exception as e2:
                            log(f"  Error getting data: {e2}")
                except Exception as e:
                    log(f"AllItemsCategoryEntry error: {e}")

                # List CDO attributes
                cdo_attrs = [a for a in dir(cdo) if not a.startswith('_')]
                log(f"CDO attributes (first 30): {cdo_attrs[:30]}")
    except Exception as e:
        log(f"Direct access error: {e}")

if __name__ == "__main__":
    extract_widget_properties()
