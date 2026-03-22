"""
Test extracting struct data from BACKUP assets (before any C++ migration).
This script should be run AFTER restoring from backup, BEFORE running migration.
"""
import unreal

# Use unreal.log to ensure output is captured
def log(msg):
    unreal.log(msg)
    print(msg)

log("=" * 70)
log("BACKUP DATA EXTRACTION TEST")
log("=" * 70)

eal = unreal.EditorAssetLibrary

# Test asset - PDA_DefaultMeshData contains FSLFSkeletalMeshData
ASSET_PATH = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"

log(f"\nTesting: {ASSET_PATH}")

if eal.does_asset_exist(ASSET_PATH):
    asset = eal.load_asset(ASSET_PATH)
    if asset:
        log(f"Loaded: {asset.get_name()}")
        log(f"Class: {asset.get_class().get_name()}")
        log(f"Parent Class: {asset.get_class().get_super_class().get_name() if asset.get_class().get_super_class() else 'None'}")

        # Try various property access methods
        log("\n--- Property Access Attempts ---")

        # Method 1: Direct property access with snake_case
        for prop_name in ['mesh_data', 'default_mesh_data', 'MeshData', 'DefaultMeshData']:
            try:
                val = asset.get_editor_property(prop_name)
                log(f"  get_editor_property('{prop_name}'): {val}")
                if val is not None:
                    # Try to access nested struct fields
                    for field in ['head_mesh', 'HeadMesh', 'upper_body_mesh', 'UpperBodyMesh']:
                        try:
                            nested = val.get_editor_property(field)
                            log(f"    .{field}: {nested}")
                        except:
                            pass
            except Exception as e:
                log(f"  get_editor_property('{prop_name}'): Error - {type(e).__name__}")

        # Method 2: Export to text and parse
        log("\n--- Export to Text ---")
        export_path = "C:/scripts/slfconversion/backup_export.copy"

        task = unreal.AssetExportTask()
        task.object = asset
        task.filename = export_path
        task.automated = True
        task.prompt = False

        if unreal.Exporter.run_asset_export_task(task):
            log(f"Exported to: {export_path}")
            try:
                with open(export_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                log(f"Export size: {len(content)} bytes")

                # Search for mesh references
                import re
                mesh_refs = re.findall(r'SkeletalMesh[\'"]([^"\']+)["\']', content)
                if mesh_refs:
                    log(f"Found {len(mesh_refs)} mesh references:")
                    for ref in mesh_refs[:5]:
                        log(f"  - {ref}")

                # Search for struct data
                if 'MeshData' in content:
                    log("Found 'MeshData' in export")
                    # Find the section
                    idx = content.find('MeshData')
                    log(f"  Context: ...{content[max(0,idx-20):idx+100]}...")

            except Exception as e:
                log(f"Error reading export: {e}")
        else:
            log("Export failed")

    else:
        log("Failed to load asset")
else:
    log(f"Asset does not exist: {ASSET_PATH}")

# Also check character blueprint for DefaultMeshInfo
log("\n" + "=" * 70)
log("CHARACTER MESH DATA TEST")
log("=" * 70)

CHAR_PATH = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
if eal.does_asset_exist(CHAR_PATH):
    bp = eal.load_asset(CHAR_PATH)
    if bp:
        log(f"Loaded: {bp.get_name()}")
        gen_class = bp.generated_class()
        if gen_class:
            log(f"Generated class: {gen_class.get_name()}")
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                log(f"CDO: {cdo}")
                # Try to access mesh
                try:
                    mesh_comp = cdo.get_editor_property('mesh')
                    log(f"Mesh component: {mesh_comp}")
                    if mesh_comp:
                        sk_mesh = mesh_comp.get_editor_property('skeletal_mesh')
                        log(f"SkeletalMesh: {sk_mesh}")
                except Exception as e:
                    log(f"Mesh access error: {e}")

                # Try DefaultMeshInfo
                for prop in ['default_mesh_info', 'DefaultMeshInfo', 'mesh_info', 'MeshInfo']:
                    try:
                        val = cdo.get_editor_property(prop)
                        log(f"{prop}: {val}")
                    except:
                        pass

log("\n" + "=" * 70)
log("TEST COMPLETE")
log("=" * 70)
