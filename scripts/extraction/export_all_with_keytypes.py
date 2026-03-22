"""
Export All Blueprints with TMap KeyTypes - Using Enhanced PythonBridge
======================================================================
Run in Unreal Editor: File > Execute Python Script > Browse to this file

This script uses the new PythonBridge module which properly exports TMap key types.
The exported JSON files will have a "KeyType" field for all TMap variables.

Prerequisites:
1. Compile the C++ module with PythonBridge.h/.cpp
2. Open Unreal Editor
3. Run this script
"""

import unreal
import json
import os
from pathlib import Path

# Output directory - separate from old exports to compare
OUTPUT_DIR = Path(r"C:\scripts\SLFConversion\Exports\BlueprintDNA_v2")
OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

# Source folder to scan
BLUEPRINT_FOLDER = "/Game/SoulslikeFramework"

def get_category(asset_name, asset_class):
    """Determine category from asset name prefix or class"""
    if asset_name.startswith("W_") or asset_name.startswith("WBP_"):
        return "WidgetBlueprint"
    elif asset_name.startswith("ABP_"):
        return "AnimBlueprint"
    elif asset_name.startswith("CR_"):
        return "ControlRig"
    elif asset_name.startswith("AC_"):
        return "Component"
    elif asset_name.startswith("PDA_") or asset_name.startswith("DA_"):
        return "DataAsset"
    elif asset_name.startswith("E_") or asset_class == "UserDefinedEnum":
        return "Enum"
    elif asset_name.startswith("F_") or asset_name.startswith("S_") or asset_class == "UserDefinedStruct":
        return "Struct"
    elif asset_name.startswith("BPI_"):
        return "Interface"
    elif asset_name.startswith("BT_") or asset_class == "BehaviorTree":
        return "BehaviorTree"
    elif asset_name.startswith("BB_") or asset_class == "BlackboardData":
        return "Blackboard"
    elif asset_name.startswith("BTT_") or asset_name.startswith("BTS_") or asset_name.startswith("BTD_"):
        return "AI"
    elif asset_name.startswith("EQS_") or asset_class == "EnvQuery":
        return "EQS"
    elif asset_name.startswith("AN_") or asset_name.startswith("ANS_"):
        return "Animation"
    elif asset_name.startswith("PC_") or asset_name.startswith("GM_") or asset_name.startswith("GI_"):
        return "GameFramework"
    elif asset_name.startswith("BP_") or asset_name.startswith("B_"):
        return "Blueprint"
    elif asset_name.startswith("DT_") or asset_class == "DataTable":
        return "DataTable"
    elif asset_class in ["CurveFloat", "CurveVector", "CurveLinearColor"]:
        return "Curve"
    else:
        return asset_class if asset_class else "Other"


def export_all():
    """Export all assets using the enhanced PythonBridge"""

    unreal.log("=" * 70)
    unreal.log("ENHANCED EXPORT WITH TMAP KEY TYPES")
    unreal.log(f"Output directory: {OUTPUT_DIR}")
    unreal.log("=" * 70)

    # Check if PythonBridge is available
    try:
        bridge = unreal.PythonBridge
        unreal.log("PythonBridge module found!")
    except AttributeError:
        unreal.log_error("ERROR: PythonBridge module not found!")
        unreal.log_error("Make sure the C++ module is compiled and the editor is restarted.")
        return

    # Get asset registry
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Get all assets
    all_assets = asset_registry.get_assets_by_path(BLUEPRINT_FOLDER, recursive=True)
    unreal.log(f"Found {len(all_assets)} total assets")

    total_exported = 0
    total_failed = 0
    export_summary = {}
    failed_assets = []

    for i, asset_data in enumerate(all_assets):
        asset_name = str(asset_data.asset_name)

        try:
            asset_class = str(asset_data.asset_class_path.asset_name)
        except:
            asset_class = str(asset_data.asset_class) if hasattr(asset_data, 'asset_class') else ""

        package_path = str(asset_data.package_name)

        try:
            # Load the asset
            asset = unreal.load_asset(package_path)
            if not asset:
                failed_assets.append((asset_name, asset_class, "Could not load"))
                total_failed += 1
                continue

            # Determine category
            category = get_category(asset_name, asset_class)

            # Export using PythonBridge
            dna = None

            try:
                dna = bridge.export_asset_dna(asset)
            except Exception as e:
                failed_assets.append((asset_name, asset_class, f"Export error: {e}"))
                total_failed += 1
                continue

            if dna and dna != "{}":
                try:
                    data = json.loads(dna)

                    # Create category subdirectory
                    category_dir = OUTPUT_DIR / category
                    category_dir.mkdir(exist_ok=True)

                    # Save to file
                    output_file = category_dir / f"{asset_name}.json"
                    with open(output_file, 'w', encoding='utf-8') as f:
                        json.dump(data, f, indent=2)

                    total_exported += 1
                    export_summary[category] = export_summary.get(category, 0) + 1

                    # Log progress
                    if total_exported % 50 == 0:
                        unreal.log(f"  Exported {total_exported} assets...")

                except json.JSONDecodeError as e:
                    failed_assets.append((asset_name, asset_class, f"JSON parse error: {e}"))
                    total_failed += 1
            else:
                failed_assets.append((asset_name, asset_class, "Empty export"))
                total_failed += 1

        except Exception as e:
            failed_assets.append((asset_name, asset_class, str(e)))
            total_failed += 1

    # Print summary
    unreal.log("\n" + "=" * 70)
    unreal.log("EXPORT COMPLETE")
    unreal.log("=" * 70)
    unreal.log(f"Total Exported: {total_exported}")
    unreal.log(f"Total Failed: {total_failed}")
    unreal.log("\nBy Category:")
    for category, count in sorted(export_summary.items(), key=lambda x: -x[1]):
        unreal.log(f"  {category}: {count}")

    # Save manifest
    manifest = {
        "total_exported": total_exported,
        "total_failed": total_failed,
        "categories": export_summary,
        "has_keytype_support": True,
        "exporter": "PythonBridge v2 with TMap KeyType support"
    }
    manifest_path = OUTPUT_DIR / "_manifest.json"
    with open(manifest_path, 'w', encoding='utf-8') as f:
        json.dump(manifest, f, indent=2)
    unreal.log(f"\nManifest saved to: {manifest_path}")

    # Save failures
    if failed_assets:
        failures_path = OUTPUT_DIR / "_failures.json"
        with open(failures_path, 'w', encoding='utf-8') as f:
            json.dump([{"name": n, "class": c, "reason": r} for n, c, r in failed_assets], f, indent=2)
        unreal.log(f"Failure details saved to: {failures_path}")


def export_single(blueprint_path):
    """Export a single Blueprint for testing"""
    try:
        bridge = unreal.PythonBridge
    except AttributeError:
        unreal.log_error("ERROR: PythonBridge module not found!")
        return None

    asset = unreal.load_asset(blueprint_path)
    if not asset:
        unreal.log_error(f"Could not load: {blueprint_path}")
        return None

    dna = bridge.export_asset_dna(asset)

    # Parse and pretty print
    data = json.loads(dna)

    # Check for TMap variables with KeyType
    if "Variables" in data and "List" in data["Variables"]:
        for var in data["Variables"]["List"]:
            if var.get("Type", {}).get("IsMap"):
                name = var.get("Name", "Unknown")
                key_type = var.get("Type", {}).get("KeyType", {})
                value_cat = var.get("Type", {}).get("Category", "unknown")
                value_obj = var.get("Type", {}).get("SubCategoryObject", "")

                if key_type:
                    key_cat = key_type.get("Category", "unknown")
                    key_obj = key_type.get("SubCategoryObject", "")
                    unreal.log(f"TMap: {name} = TMap<{key_cat}/{key_obj}, {value_cat}/{value_obj}>")
                else:
                    unreal.log(f"TMap: {name} = TMap<MISSING_KEY, {value_cat}/{value_obj}>")

    return data


def validate_keytype_export():
    """Validate that TMap key types are properly exported"""
    unreal.log("=" * 70)
    unreal.log("VALIDATING TMAP KEY TYPE EXPORT")
    unreal.log("=" * 70)

    # Test with AC_StatManager which has TMap variables
    test_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"

    data = export_single(test_path)
    if not data:
        return False

    # Check each TMap variable
    success = True
    for var in data.get("Variables", {}).get("List", []):
        if var.get("Type", {}).get("IsMap"):
            name = var.get("Name", "Unknown")
            key_type = var.get("Type", {}).get("KeyType")

            if key_type and key_type.get("Category"):
                unreal.log(f"  [OK] {name} has KeyType: {key_type.get('Category')}")
            else:
                unreal.log_warning(f"  [MISSING] {name} is missing KeyType!")
                success = False

    if success:
        unreal.log("\nVALIDATION PASSED: All TMap variables have KeyType!")
    else:
        unreal.log_warning("\nVALIDATION FAILED: Some TMap variables missing KeyType")

    return success


# Auto-run validation when script is loaded
if __name__ == "__main__":
    unreal.log("\n" + "=" * 70)
    unreal.log("ENHANCED BLUEPRINT EXPORTER v2")
    unreal.log("With TMap KeyType support")
    unreal.log("=" * 70)
    unreal.log("\nAvailable functions:")
    unreal.log("  export_all() - Export all assets to BlueprintDNA_v2/")
    unreal.log("  export_single('/Game/Path/To/BP') - Export single asset")
    unreal.log("  validate_keytype_export() - Test TMap KeyType export")
    unreal.log("=" * 70 + "\n")

    # Run validation
    validate_keytype_export()
