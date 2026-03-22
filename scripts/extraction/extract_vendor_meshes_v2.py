"""
Extract mesh assignments from B_Soulslike_NPC_ShowcaseVendor using text export.
Run on bp_only project to get original data.
"""
import unreal
import os

def extract_vendor_meshes():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"Could not load: {bp_path}")
        return

    unreal.log_warning(f"\n=== Vendor Blueprint Mesh Data ===")

    # Export to text
    output_dir = "C:/scripts/SLFConversion/migration_cache"
    os.makedirs(output_dir, exist_ok=True)
    output_file = os.path.join(output_dir, "vendor_blueprint_export.txt")

    # Try text export
    try:
        result = unreal.AssetExportTask()
        result.set_editor_property("object", bp)
        result.set_editor_property("exporter", None)
        result.set_editor_property("filename", output_file)
        result.set_editor_property("automated", True)
        result.set_editor_property("prompt", False)
        result.set_editor_property("write_empty_files", False)

        if unreal.Exporter.run_asset_export_task(result):
            unreal.log_warning(f"Exported to: {output_file}")
            # Read and find mesh references
            with open(output_file, "r", encoding="utf-8", errors="ignore") as f:
                content = f.read()
                # Look for SkeletalMesh references
                for line in content.split("\n"):
                    if "SkeletalMesh" in line or "Head" in line or "Body" in line or "Arms" in line or "Legs" in line:
                        unreal.log_warning(line.strip())
        else:
            unreal.log_warning("Export failed")
    except Exception as e:
        unreal.log_warning(f"Export error: {e}")

    # Try to get CDO defaults
    unreal.log_warning("\n=== CDO Defaults ===")
    try:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                unreal.log_warning(f"CDO: {cdo.get_name()}")
                # List all components
                components = cdo.get_components_by_class(unreal.SkeletalMeshComponent)
                unreal.log_warning(f"  SkeletalMeshComponents: {len(components)}")
                for comp in components:
                    mesh = comp.get_skeletal_mesh_asset()
                    unreal.log_warning(f"    - {comp.get_name()}: {mesh.get_path_name() if mesh else 'NONE'}")
    except Exception as e:
        unreal.log_warning(f"CDO error: {e}")

if __name__ == "__main__":
    extract_vendor_meshes()
