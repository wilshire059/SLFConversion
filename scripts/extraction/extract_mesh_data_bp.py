# extract_mesh_data_bp.py
# Extract mesh data from Blueprint struct in bp_only

import unreal
import re
import json

def log(msg):
    print(f"[Extract] {msg}")
    unreal.log_warning(f"[Extract] {msg}")

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/mesh_data_export.json"

def extract_mesh_paths_from_export(asset_path):
    """Export asset to text and parse skeletal mesh references"""
    log(f"Exporting: {asset_path}")

    asset = unreal.load_asset(asset_path)
    if not asset:
        log(f"  [ERROR] Could not load asset")
        return {}

    # Export to text
    export_text = unreal.EditorAssetLibrary.get_metadata_tag(asset_path, "None")  # Just to test

    # Use export_text_internal
    exporter_options = unreal.AssetExportTask()
    exporter_options.set_editor_property('object', asset)
    exporter_options.set_editor_property('automated', True)
    exporter_options.set_editor_property('prompt', False)
    exporter_options.set_editor_property('exporter', None)

    # Fallback: get asset registry info
    log(f"  Asset class: {asset.get_class().get_name()}")

    # Try to access raw properties through reflection
    meshes = {}

    # Try different property name patterns used in Blueprint structs
    bp_prop_names = [
        'SkeletalMeshData',     # Common Blueprint struct name
        'MeshData',
        'DefaultMeshData',
        'SkeletalMeshInfo',
    ]

    for prop_name in bp_prop_names:
        try:
            val = asset.get_editor_property(prop_name.lower())
            if val:
                log(f"  Found {prop_name}: {val}")
                meshes[prop_name] = str(val)
        except:
            pass

    # Check if there's an export_text function
    if hasattr(asset, 'export_text'):
        try:
            text = asset.export_text()
            log(f"  Export text length: {len(text)}")

            # Parse for SkeletalMesh references
            skm_pattern = r'/Game/[^"\']+SKM[^"\']*'
            matches = re.findall(skm_pattern, text)
            if matches:
                log(f"  Found SKM references: {matches}")
                meshes['skm_refs'] = matches
        except Exception as e:
            log(f"  export_text error: {e}")

    return meshes

def main():
    log("=" * 80)
    log("MESH DATA EXTRACTION")
    log("=" * 80)

    results = {}

    # Extract from QuinnMeshDefault
    results['DA_QuinnMeshDefault'] = extract_mesh_paths_from_export(
        "/Game/SoulslikeFramework/Data/BaseCharacters/DA_QuinnMeshDefault")

    # Extract from MannyMeshDefault
    results['DA_MannyMeshDefault'] = extract_mesh_paths_from_export(
        "/Game/SoulslikeFramework/Data/BaseCharacters/DA_MannyMeshDefault")

    # Save results
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(results, f, indent=2)
    log(f"\nSaved to: {OUTPUT_PATH}")

    log("=" * 80)

if __name__ == "__main__":
    main()
