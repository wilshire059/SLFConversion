# fix_property_access_paths.py
# Fix Property Access node paths that still reference "IsGuarding?" instead of "IsGuarding"
#
# Property Access nodes store their path as a serialized string array that doesn't
# get automatically updated when the target property is renamed.

import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/property_access_fix.txt"
os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)

def log(msg):
    print(f"[PropAccessFix] {msg}")
    unreal.log(f"[PropAccessFix] {msg}")
    with open(OUTPUT_FILE, "a") as f:
        f.write(f"{msg}\n")

# Clear output
with open(OUTPUT_FILE, "w") as f:
    f.write("")

log("=" * 60)
log("FIX PROPERTY ACCESS PATHS")
log("=" * 60)

# The AnimBP with the broken Property Access paths
ABP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

# Old -> New path mappings (for ? suffix fix)
PATH_FIXES = {
    "IsGuarding?": "IsGuarding",
}

abp = unreal.EditorAssetLibrary.load_asset(ABP_PATH)
if not abp:
    log(f"ERROR: Could not load {ABP_PATH}")
else:
    log(f"Loaded: {abp.get_name()}")
    log(f"Class: {abp.get_class().get_name()}")

    # Try to access the generated class to find Property Access nodes
    try:
        # Get all graphs in the AnimBP
        blueprint = unreal.EditorAssetLibrary.load_blueprint_class(ABP_PATH)
        log(f"Blueprint class: {blueprint}")
    except Exception as e:
        log(f"Blueprint class error: {e}")

    # Try using asset registry to find the asset
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Export the asset to text to examine its structure
    try:
        # Get package path
        package_path = ABP_PATH
        log(f"\nExporting asset to examine Property Access paths...")

        # Use EditorAssetLibrary.find_package_referencers_for_asset to explore
        export_text = unreal.EditorAssetLibrary.export_text(abp)

        # Search for IsGuarding? in the export
        if "IsGuarding?" in export_text:
            log(f"  Found 'IsGuarding?' references in export text!")

            # Count occurrences
            count = export_text.count("IsGuarding?")
            log(f"  Occurrences: {count}")

            # Show context around each occurrence
            import re
            for match in re.finditer(r'.{0,50}IsGuarding\?.{0,50}', export_text):
                log(f"  Context: ...{match.group()}...")
        else:
            log(f"  'IsGuarding?' NOT found in export text (may be fixed)")

        # Check for the correct name
        if "IsGuarding" in export_text and "IsGuarding?" not in export_text:
            log(f"  Found 'IsGuarding' (correct) without '?' suffix")

    except Exception as e:
        log(f"Export error: {e}")

    # Try to modify using string replacement on the raw asset
    # This is hacky but may work since the path is stored as text
    log("\n" + "=" * 60)
    log("ATTEMPTING DIRECT TEXT MODIFICATION")
    log("=" * 60)

    # Get the package file path
    package_path = unreal.PackageName.get_local_full_path(ABP_PATH)
    log(f"Package path: {package_path}")

    # We need a different approach - use the USLFAutomationLibrary to expose a fix function
    log("\nNote: Property Access paths require C++ modification.")
    log("Adding a new C++ function to fix these paths...")

log("\n" + "=" * 60)
log("COMPLETE")
log("=" * 60)
