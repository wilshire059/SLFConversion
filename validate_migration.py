#!/usr/bin/env python3
"""
validate_migration.py
Compare post-migration Blueprint state against pre-migration snapshot

Usage:
  Run via UnrealEditor-Cmd.exe -run=pythonscript -script=validate_migration.py

Requires:
  migration_snapshot.json from snapshot_blueprint_state.py
"""

import unreal
import json
import os

# Snapshot path
SNAPSHOT_PATH = "C:/scripts/SLFConversion/migration_snapshot.json"
VALIDATION_REPORT_PATH = "C:/scripts/SLFConversion/validation_report.md"


class ValidationResult:
    def __init__(self):
        self.passed = 0
        self.failed = 0
        self.warnings = 0
        self.errors = []
        self.warnings_list = []
        self.passes = []


def get_property_value_safe(obj, prop_name):
    """Safely get a property value"""
    try:
        return obj.get_editor_property(prop_name)
    except Exception as e:
        return None


def compare_values(expected, actual, path=""):
    """Compare expected vs actual values, return list of differences"""
    differences = []

    if expected is None and actual is None:
        return differences

    if expected is None:
        differences.append(f"{path}: Expected None, got {type(actual).__name__}")
        return differences

    if actual is None:
        differences.append(f"{path}: Expected {type(expected).__name__}, got None")
        return differences

    # Handle TMap comparison (dict in snapshot)
    if isinstance(expected, dict) and expected.get("type") == "TMap":
        expected_count = expected.get("count", 0)

        # Get actual count
        if isinstance(actual, dict):
            actual_count = len(actual)
        elif hasattr(actual, '__len__'):
            actual_count = len(actual)
        else:
            actual_count = 0

        if expected_count != actual_count:
            differences.append(f"{path}: TMap count mismatch - expected {expected_count}, got {actual_count}")

        # Compare entries
        expected_entries = expected.get("entries", {})
        for key, expected_val in expected_entries.items():
            if key not in str(actual) if actual else True:
                differences.append(f"{path}[{key}]: Key missing in actual")

        return differences

    # Handle TArray comparison
    if isinstance(expected, dict) and expected.get("type") == "TArray":
        expected_count = expected.get("count", 0)

        if isinstance(actual, (list, tuple)):
            actual_count = len(actual)
        elif hasattr(actual, '__len__'):
            actual_count = len(actual)
        else:
            actual_count = 0

        if expected_count != actual_count:
            differences.append(f"{path}: TArray count mismatch - expected {expected_count}, got {actual_count}")

        return differences

    # Handle object comparison
    if isinstance(expected, dict) and expected.get("type") == "object":
        expected_path = expected.get("path")
        if actual:
            actual_path = actual.get_path_name() if hasattr(actual, 'get_path_name') else str(actual)
            if expected_path != actual_path:
                differences.append(f"{path}: Object path mismatch - expected {expected_path}, got {actual_path}")
        else:
            differences.append(f"{path}: Expected object at {expected_path}, got None")

        return differences

    # Simple value comparison
    if expected != actual:
        differences.append(f"{path}: Value mismatch - expected {expected}, got {actual}")

    return differences


def validate_component(component_name, snapshot_data, result):
    """Validate a single component against its snapshot"""
    unreal.log_warning(f"[Validate] Checking: {component_name}")

    if "error" in snapshot_data:
        result.errors.append(f"{component_name}: Snapshot error - {snapshot_data['error']}")
        result.failed += 1
        return

    bp_path = snapshot_data.get("path")
    if not bp_path:
        result.errors.append(f"{component_name}: No path in snapshot")
        result.failed += 1
        return

    # Load current Blueprint
    try:
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            result.errors.append(f"{component_name}: Could not load Blueprint: {bp_path}")
            result.failed += 1
            return

        gen_class = bp.generated_class()
        if not gen_class:
            result.errors.append(f"{component_name}: No generated class")
            result.failed += 1
            return

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            result.errors.append(f"{component_name}: Could not get CDO")
            result.failed += 1
            return

    except Exception as e:
        result.errors.append(f"{component_name}: Exception loading - {str(e)}")
        result.failed += 1
        return

    # Check parent class
    expected_parent = snapshot_data.get("parent_class")
    actual_parent = gen_class.get_super_class().get_name() if gen_class.get_super_class() else None

    if expected_parent and actual_parent:
        # Parent class name might change during migration (e.g., ActorComponent -> UAC_ActionManager)
        # This is expected, so just log it
        if expected_parent != actual_parent:
            result.warnings_list.append(f"{component_name}: Parent class changed: {expected_parent} -> {actual_parent}")
            result.warnings += 1

    # Validate each property
    snapshot_props = snapshot_data.get("properties", {})
    for prop_name, expected_value in snapshot_props.items():
        actual_value = get_property_value_safe(cdo, prop_name)

        if isinstance(expected_value, dict) and "error" in expected_value:
            result.warnings_list.append(f"{component_name}.{prop_name}: Could not read in snapshot")
            result.warnings += 1
            continue

        differences = compare_values(expected_value, actual_value, f"{component_name}.{prop_name}")

        if differences:
            for diff in differences:
                result.errors.append(diff)
            result.failed += 1
        else:
            result.passes.append(f"{component_name}.{prop_name}: MATCH")
            result.passed += 1


def run_validation():
    """Run full validation against snapshot"""
    unreal.log_warning("═══════════════════════════════════════════════════════════════")
    unreal.log_warning("[Validation] Post-Migration Blueprint Validation")
    unreal.log_warning("═══════════════════════════════════════════════════════════════")

    # Load snapshot
    if not os.path.exists(SNAPSHOT_PATH):
        unreal.log_error(f"[Validation] Snapshot not found: {SNAPSHOT_PATH}")
        unreal.log_error("[Validation] Run snapshot_blueprint_state.py first!")
        return None

    with open(SNAPSHOT_PATH, 'r', encoding='utf-8') as f:
        snapshot = json.load(f)

    result = ValidationResult()

    # Validate each component
    for component_name, component_data in snapshot.get("components", {}).items():
        validate_component(component_name, component_data, result)

    # Generate report
    report_lines = [
        "# Migration Validation Report",
        "",
        f"## Summary",
        f"- **Passed**: {result.passed}",
        f"- **Failed**: {result.failed}",
        f"- **Warnings**: {result.warnings}",
        "",
    ]

    if result.errors:
        report_lines.append("## Errors (Must Fix)")
        for error in result.errors:
            report_lines.append(f"- ❌ {error}")
        report_lines.append("")

    if result.warnings_list:
        report_lines.append("## Warnings")
        for warning in result.warnings_list:
            report_lines.append(f"- ⚠️ {warning}")
        report_lines.append("")

    if result.passes:
        report_lines.append("## Passed")
        for p in result.passes:
            report_lines.append(f"- ✅ {p}")
        report_lines.append("")

    # Write report
    with open(VALIDATION_REPORT_PATH, 'w', encoding='utf-8') as f:
        f.write('\n'.join(report_lines))

    # Log summary
    unreal.log_warning("═══════════════════════════════════════════════════════════════")
    unreal.log_warning(f"[Validation] PASSED: {result.passed} | FAILED: {result.failed} | WARNINGS: {result.warnings}")
    unreal.log_warning(f"[Validation] Report saved to: {VALIDATION_REPORT_PATH}")
    unreal.log_warning("═══════════════════════════════════════════════════════════════")

    return result


# Entry point
if __name__ == "__main__":
    run_validation()
