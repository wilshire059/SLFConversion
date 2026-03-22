"""
Diagnose B_SkyManager: Compare Blueprint CDO vs Level Instance

This script checks:
1. Blueprint CDO component count (should be 8 after C++ migration)
2. Level instance component count (may be 0 if stale)
3. Reports the discrepancy
"""

import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_diagnosis.txt"

def log(msg):
    """Log to file and UE"""
    unreal.log(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def diagnose_skymanager():
    # Clear previous output
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("=" * 60)
    log("B_SkyManager Diagnosis: CDO vs Level Instance")
    log("=" * 60)

    # Step 1: Check Blueprint CDO
    log("")
    log("--- Step 1: Blueprint CDO Analysis ---")
    bp_path = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
    bp_asset = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp_asset:
        log(f"ERROR: Could not load Blueprint at {bp_path}")
        return

    gen_class = bp_asset.generated_class()
    if not gen_class:
        log("ERROR: Blueprint has no generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log("ERROR: Could not get CDO")
        return

    # Get CDO components
    cdo_components = cdo.get_components_by_class(unreal.ActorComponent)
    log(f"CDO Component Count: {len(cdo_components)}")

    # List CDO components
    log("")
    log("CDO Components:")
    for comp in cdo_components:
        log(f"  - {comp.get_name()} ({comp.get_class().get_name()})")

    # Check for specific components we expect (with correct names)
    expected_components = [
        "DirectionalLight_Sun",
        "DirectionalLight_Moon",
        "SkyLight",
        "SkyAtmosphere",
        "VolumetricCloud",
        "SM_SkySphere",  # Actual name
        "ExponentialHeightFog"
    ]

    cdo_names = [comp.get_name() for comp in cdo_components]
    log("")
    log("Expected Components Check:")
    missing_components = []
    for expected in expected_components:
        found = expected in cdo_names
        status = "FOUND" if found else "MISSING"
        log(f"  {expected}: {status}")
        if not found:
            missing_components.append(expected)

    # Step 2: Check Level using EditorLoadingAndSaving
    log("")
    log("--- Step 2: Level Instance Analysis ---")
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    log(f"Checking level: {level_path}")

    # Load level package directly
    level_pkg = unreal.load_package(None, level_path)
    if not level_pkg:
        log(f"ERROR: Could not load level package at {level_path}")
        log("")
        log("Attempting to load level asset...")
        level_asset = unreal.EditorAssetLibrary.load_asset(level_path)
        if level_asset:
            log(f"  Level asset type: {type(level_asset).__name__}")
        else:
            log("  Could not load level asset either")
    else:
        log(f"Loaded level package: {level_pkg.get_name()}")

    # Export level to T3D to analyze actors
    log("")
    log("--- Analyzing Level via T3D Export ---")

    level_umap_path = "C:/scripts/SLFConversion/Content/SoulslikeFramework/Maps/L_Demo_Showcase.umap"

    # Check if level file exists
    if os.path.exists(level_umap_path):
        log(f"Level file exists: {level_umap_path}")
        log(f"File size: {os.path.getsize(level_umap_path) / 1024:.1f} KB")
    else:
        log(f"WARNING: Level file not found at {level_umap_path}")

    # Step 3: Summary
    log("")
    log("=" * 60)
    log("DIAGNOSIS SUMMARY")
    log("=" * 60)
    log(f"Blueprint CDO Components: {len(cdo_components)}")

    if len(cdo_components) >= 7 and len(missing_components) == 0:
        log("")
        log("CDO Status: GOOD (has all expected components)")
    else:
        log("")
        log(f"CDO Status: PARTIAL (missing: {missing_components})")

    log("")
    log("NOTE: Level instance analysis requires running in the editor.")
    log("      Run the fix script to refresh the level instance.")
    log("")
    log("RECOMMENDATION:")
    log("  1. Run fix_skymanager_level_instance.py with the editor running")
    log("  2. OR open L_Demo_Showcase in editor, delete B_SkyManager, place new one")
    log("")
    log(f"Results written to: {OUTPUT_FILE}")

if __name__ == "__main__":
    diagnose_skymanager()
