# extract_container_data.py
# Extract B_Container Blueprint variable values from bp_only

import unreal
import json

# Paths
BP_CONTAINER_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/container_data.json"

def extract_container_data():
    """Extract B_Container Blueprint data"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("EXTRACTING B_CONTAINER DATA FROM BP_ONLY")
    unreal.log_warning("=" * 60)

    data = {}

    # Load the Blueprint asset (not the class)
    bp_asset = unreal.load_asset(BP_CONTAINER_PATH)
    if not bp_asset:
        unreal.log_error(f"Could not load Blueprint: {BP_CONTAINER_PATH}")
        return

    unreal.log_warning(f"Loaded Blueprint: {bp_asset.get_name()}")

    # Export text to get all properties
    export_text = unreal.EditorAssetLibrary.export_text(BP_CONTAINER_PATH)

    # Save export text for analysis
    with open("C:/scripts/SLFConversion/migration_cache/container_export.txt", "w") as f:
        f.write(export_text)

    unreal.log_warning("Exported Blueprint text")

    # Parse export text for key values
    lines = export_text.split("\n")

    # Look for specific patterns
    for i, line in enumerate(lines):
        # OpenMontage reference
        if "OpenMontage" in line and "SoftObjectPath" in line:
            data["OpenMontage"] = line.strip()
            unreal.log_warning(f"Found OpenMontage: {line.strip()[:100]}")

        # OpenVFX reference
        if "OpenVFX" in line and "SoftObjectPath" in line:
            data["OpenVFX"] = line.strip()
            unreal.log_warning(f"Found OpenVFX: {line.strip()[:100]}")

        # MoveDistance value
        if "MoveDistance" in line:
            data["MoveDistance_line"] = line.strip()
            unreal.log_warning(f"Found MoveDistance: {line.strip()}")

        # SpeedMultiplier value
        if "SpeedMultiplier" in line:
            data["SpeedMultiplier_line"] = line.strip()
            unreal.log_warning(f"Found SpeedMultiplier: {line.strip()}")

        # Lid mesh
        if "Lid" in line and ("StaticMesh" in line or "SM_" in line):
            data["Lid_line"] = line.strip()
            unreal.log_warning(f"Found Lid mesh: {line.strip()[:100]}")

        # PointLight intensity
        if "Intensity" in line and "PointLight" in lines[max(0,i-5):i]:
            data["PointLight_Intensity_line"] = line.strip()
            unreal.log_warning(f"Found PointLight Intensity: {line.strip()}")

        # LootTable
        if "LootTable" in line:
            data["LootTable_line"] = line.strip()
            unreal.log_warning(f"Found LootTable: {line.strip()[:100]}")

        # InteractableDisplayName
        if "InteractableDisplayName" in line:
            data["InteractableDisplayName_line"] = line.strip()
            unreal.log_warning(f"Found InteractableDisplayName: {line.strip()}")

        # InteractionText
        if "InteractionText" in line:
            data["InteractionText_line"] = line.strip()
            unreal.log_warning(f"Found InteractionText: {line.strip()}")

    # Save extracted data
    with open(OUTPUT_PATH, "w") as f:
        json.dump(data, f, indent=2)

    unreal.log_warning(f"\nSaved data to: {OUTPUT_PATH}")
    unreal.log_warning("=" * 60)
    unreal.log_warning("EXTRACTION COMPLETE")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    extract_container_data()
