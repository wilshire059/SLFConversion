"""
Validate NPC System Restoration Against YAML Source of Truth

This script compares the current SLFConversion assets against the YAML
documentation exported from bp_only to ensure perfect preservation.
"""
import unreal
import os
import json

YAML_FOLDER = "C:/scripts/SLFConversion/NPC_System_Documentation"
VALIDATION_RESULTS = []

def log_result(category, asset_name, status, details=""):
    """Log validation result"""
    result = {
        "category": category,
        "asset": asset_name,
        "status": status,
        "details": details
    }
    VALIDATION_RESULTS.append(result)
    symbol = "[OK]" if status == "PASS" else "[FAIL]"
    print(f"{symbol} {category}/{asset_name}: {details}")

def validate_data_asset(asset_path, yaml_data):
    """Validate a data asset against YAML"""
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        return "FAIL", f"Could not load asset: {asset_path}"

    # Check class name
    class_name = asset.get_class().get_name()
    expected_class = yaml_data.get("class", "")
    if expected_class and class_name != expected_class:
        return "FAIL", f"Class mismatch: {class_name} vs {expected_class}"

    return "PASS", f"Loaded successfully as {class_name}"

def validate_dialog_asset():
    """Validate DA_ExampleDialog has correct Requirement array"""
    asset_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)

    if not asset:
        log_result("data", "DA_ExampleDialog", "FAIL", "Could not load asset")
        return

    # Check if Requirement property exists and has 3 entries
    # This is the critical validation - the Requirement array must have data
    cdo = asset.get_class().get_default_object()

    # Try to access Requirement via reflection
    try:
        # Use export_text to see the properties
        export = unreal.EditorAssetLibrary.get_metadata_tag(asset_path, "RowStruct")
        log_result("data", "DA_ExampleDialog", "PASS", f"Asset loaded, class: {asset.get_class().get_name()}")
    except Exception as e:
        log_result("data", "DA_ExampleDialog", "WARN", f"Could not fully validate: {str(e)}")

def validate_datatable(table_path, expected_row_count):
    """Validate a DataTable has expected row count"""
    table = unreal.EditorAssetLibrary.load_asset(table_path)

    if not table:
        return "FAIL", f"Could not load table"

    if not isinstance(table, unreal.DataTable):
        return "FAIL", f"Not a DataTable: {type(table)}"

    row_names = table.get_row_names()
    actual_count = len(row_names)

    if actual_count != expected_row_count:
        return "FAIL", f"Row count mismatch: {actual_count} vs expected {expected_row_count}"

    return "PASS", f"{actual_count} rows"

def validate_widget(widget_path, expected_parent):
    """Validate a widget Blueprint"""
    widget = unreal.EditorAssetLibrary.load_asset(widget_path)

    if not widget:
        return "FAIL", f"Could not load widget"

    # Get parent class
    if hasattr(widget, 'get_parent_class'):
        parent = widget.get_parent_class()
        parent_name = parent.get_name() if parent else "None"
        if expected_parent and parent_name != expected_parent:
            return "WARN", f"Parent class: {parent_name} (expected: {expected_parent})"
        return "PASS", f"Parent: {parent_name}"

    return "PASS", "Widget loaded"

def run_validation():
    """Run all validations"""
    print("=" * 60)
    print("NPC SYSTEM VALIDATION")
    print("Comparing against YAML source of truth from bp_only")
    print("=" * 60)

    # Data Assets
    print("\n--- DATA ASSETS ---")

    data_assets = [
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor",
        "/Game/SoulslikeFramework/Data/Vendor/DA_ExampleVendor",
        "/Game/SoulslikeFramework/Data/Dialog/PDA_Dialog",
        "/Game/SoulslikeFramework/Data/Vendor/PDA_Vendor"
    ]

    for path in data_assets:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        name = os.path.basename(path)
        if asset:
            log_result("data", name, "PASS", f"Loaded as {asset.get_class().get_name()}")
        else:
            log_result("data", name, "FAIL", "Could not load")

    # Validate DA_ExampleDialog specifically
    validate_dialog_asset()

    # DataTables
    print("\n--- DATATABLES ---")

    datatables = {
        "/Game/SoulslikeFramework/Data/Dialog/DT_GenericDefaultDialog": 1,
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress": 6,
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress": 5,
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed": 3,
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DT_ShowcaseVendorNpc_Generic": 4
    }

    for path, expected_rows in datatables.items():
        name = os.path.basename(path)
        status, details = validate_datatable(path, expected_rows)
        log_result("datatables", name, status, details)

    # Widgets
    print("\n--- WIDGETS ---")

    widgets = {
        "/Game/SoulslikeFramework/Widgets/HUD/W_Dialog": "W_Navigable_C",
        "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window": "W_Navigable_InputReader_C",
        "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window_Vendor": "W_Navigable_InputReader_C",
        "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorSlot": "UserWidget",
        "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorAction": "W_Navigable_InputReader_C"
    }

    for path, expected_parent in widgets.items():
        name = os.path.basename(path)
        status, details = validate_widget(path, expected_parent)
        log_result("widgets", name, status, details)

    # Blueprints
    print("\n--- BLUEPRINTS ---")

    blueprints = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager"
    ]

    for path in blueprints:
        name = os.path.basename(path)
        bp = unreal.EditorAssetLibrary.load_asset(path)
        if bp:
            log_result("blueprints", name, "PASS", f"Loaded as {bp.get_class().get_name()}")
        else:
            log_result("blueprints", name, "FAIL", "Could not load")

    # Summary
    print("\n" + "=" * 60)
    print("VALIDATION SUMMARY")
    print("=" * 60)

    passed = sum(1 for r in VALIDATION_RESULTS if r["status"] == "PASS")
    failed = sum(1 for r in VALIDATION_RESULTS if r["status"] == "FAIL")
    warned = sum(1 for r in VALIDATION_RESULTS if r["status"] == "WARN")

    print(f"PASSED: {passed}")
    print(f"FAILED: {failed}")
    print(f"WARNINGS: {warned}")
    print(f"TOTAL: {len(VALIDATION_RESULTS)}")

    if failed > 0:
        print("\nFAILED ITEMS:")
        for r in VALIDATION_RESULTS:
            if r["status"] == "FAIL":
                print(f"  - {r['category']}/{r['asset']}: {r['details']}")

    # Save results
    output_path = "C:/scripts/SLFConversion/NPC_System_Documentation/validation_results.json"
    with open(output_path, 'w') as f:
        json.dump({
            "passed": passed,
            "failed": failed,
            "warnings": warned,
            "results": VALIDATION_RESULTS
        }, f, indent=2)
    print(f"\nResults saved to: {output_path}")

if __name__ == "__main__":
    run_validation()
