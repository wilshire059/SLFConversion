"""
Test Option A: Extract struct data from backup, migrate, re-apply.
Testing with FSLFSkeletalMeshData struct used in PDA_DefaultMeshData.
"""
import unreal
import os
import re

print("=" * 70)
print("STRUCT MIGRATION TEST: FSLFSkeletalMeshData")
print("=" * 70)

eal = unreal.EditorAssetLibrary
bel = unreal.BlueprintEditorLibrary

# Paths
BACKUP_PATH = "C:/scripts/bp_only/Content/SoulslikeFramework/Data/PDA_DefaultMeshData.uasset"
GAME_ASSET_PATH = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
CPP_CLASS_PATH = "/Script/SLFConversion.PDA_DefaultMeshData"

# Step 1: Check if backup exists
print("\n=== STEP 1: Check Backup ===")
if os.path.exists(BACKUP_PATH):
    print(f"Backup exists: {BACKUP_PATH}")
else:
    print(f"ERROR: Backup not found: {BACKUP_PATH}")
    exit(1)

# Step 2: Extract data from backup using export_text
print("\n=== STEP 2: Extract Data from Backup ===")

# We need to use the asset registry to get the backup data
# But first, let's read the raw text export to understand the structure

# Use unreal's export functionality on the CURRENT asset first to see what we're working with
if eal.does_asset_exist(GAME_ASSET_PATH):
    asset = eal.load_asset(GAME_ASSET_PATH)
    if asset:
        print(f"Loaded current asset: {asset.get_name()}")
        print(f"Class: {asset.get_class().get_name()}")

        # Try to get the MeshData property
        try:
            mesh_data = asset.get_editor_property('mesh_data')
            print(f"MeshData property exists: {mesh_data is not None}")
            if mesh_data:
                # Try to access nested properties
                try:
                    head = mesh_data.get_editor_property('head_mesh')
                    print(f"  HeadMesh: {head}")
                except Exception as e:
                    print(f"  HeadMesh access error: {e}")
        except Exception as e:
            print(f"MeshData access error: {e}")

# Step 3: Read backup file and extract struct data using text export
print("\n=== STEP 3: Parse Backup Data ===")

# Copy backup to a temp location and load it
import shutil
TEMP_ASSET_PATH = "/Game/SoulslikeFramework/Data/_TEMP_PDA_DefaultMeshData"
TEMP_FILE_PATH = "C:/scripts/slfconversion/Content/SoulslikeFramework/Data/_TEMP_PDA_DefaultMeshData.uasset"

# Copy backup to temp location
os.makedirs(os.path.dirname(TEMP_FILE_PATH), exist_ok=True)
shutil.copy2(BACKUP_PATH, TEMP_FILE_PATH)
print(f"Copied backup to temp: {TEMP_FILE_PATH}")

# Force asset registry refresh
asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
asset_registry.scan_paths_synchronous(["/Game/SoulslikeFramework/Data"], force_rescan=True)

# Load the temp asset (this is the original Blueprint version)
temp_asset = eal.load_asset(TEMP_ASSET_PATH)
if temp_asset:
    print(f"Loaded backup asset: {temp_asset.get_name()}")
    print(f"Backup class: {temp_asset.get_class().get_name()}")

    # Export to text to see the raw data
    export_path = "C:/scripts/slfconversion/temp_export.t3d"

    # Use AssetExportTask
    task = unreal.AssetExportTask()
    task.object = temp_asset
    task.filename = export_path
    task.exporter = None
    task.automated = True
    task.prompt = False
    task.write_empty_files = False

    if unreal.Exporter.run_asset_export_task(task):
        print(f"Exported to: {export_path}")

        # Read and parse the export
        with open(export_path, 'r', encoding='utf-8', errors='ignore') as f:
            export_text = f.read()

        print(f"Export size: {len(export_text)} bytes")

        # Look for MeshData structure
        if 'MeshData' in export_text:
            print("Found MeshData in export")
            # Print relevant section
            lines = export_text.split('\n')
            for i, line in enumerate(lines):
                if 'MeshData' in line or 'HeadMesh' in line or 'UpperBodyMesh' in line:
                    print(f"  Line {i}: {line[:100]}")
    else:
        print("Export failed")

    # Try direct property access on backup
    print("\n=== STEP 4: Direct Property Access on Backup ===")
    try:
        # The backup should have Blueprint struct, let's see what properties exist
        for prop_name in ['mesh_data', 'MeshData', 'default_mesh_data', 'DefaultMeshData']:
            try:
                val = temp_asset.get_editor_property(prop_name)
                print(f"  {prop_name}: {val}")
            except:
                pass
    except Exception as e:
        print(f"Property access error: {e}")

else:
    print("ERROR: Could not load backup asset")

# Cleanup temp file
print("\n=== CLEANUP ===")
if os.path.exists(TEMP_FILE_PATH):
    os.remove(TEMP_FILE_PATH)
    print("Removed temp file")

print("\n" + "=" * 70)
print("TEST COMPLETE")
print("=" * 70)
