"""
Extract weapon stat data from bp_only using T3D export
This captures actual property values, not just type definitions
"""
import unreal
import os
import re

def extract_weapon_stats_t3d():
    """Export weapons to T3D format to get actual stat values"""

    # Weapon data assets in bp_only
    weapon_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_Katana",
        "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
        "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
        "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
        "/Game/SoulslikeFramework/Data/Items/DA_BossMace",
    ]

    output_dir = "C:/scripts/SLFConversion/migration_cache/weapon_t3d/"
    os.makedirs(output_dir, exist_ok=True)

    for asset_path in weapon_paths:
        asset = unreal.load_asset(asset_path)
        if not asset:
            print(f"ERROR: Could not load {asset_path}")
            continue

        asset_name = asset.get_name()
        print(f"\n=== Exporting {asset_name} ===")

        # Create export task
        task = unreal.AssetExportTask()
        task.set_editor_property('object', asset)
        task.set_editor_property('filename', f"{output_dir}{asset_name}.t3d")
        task.set_editor_property('exporter', None)  # Auto-select exporter
        task.set_editor_property('automated', True)
        task.set_editor_property('prompt', False)
        task.set_editor_property('replace_identical', True)

        # Try to export
        try:
            success = unreal.Exporter.run_asset_export_task(task)
            if success:
                print(f"  Exported to {output_dir}{asset_name}.t3d")
            else:
                print(f"  Export failed - trying COPY export")
                # Try using EditorAssetLibrary to get raw data
        except Exception as e:
            print(f"  Export error: {e}")

    # Also try direct property inspection via class inspection
    print("\n\n=== Direct Property Inspection ===")

    for asset_path in weapon_paths:
        asset = unreal.load_asset(asset_path)
        if not asset:
            continue

        asset_name = asset.get_name()
        asset_class = asset.get_class()
        print(f"\n{asset_name}:")
        print(f"  Class: {asset_class.get_name()}")
        print(f"  Parent: {asset_class.get_super_class().get_name() if asset_class.get_super_class() else 'None'}")

        # Get all properties of this class
        output_file = f"{output_dir}{asset_name}_props.txt"
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(f"=== {asset_name} Properties ===\n\n")

            # Iterate through class hierarchy to find all properties
            current_class = asset_class
            while current_class:
                class_name = current_class.get_name()
                f.write(f"\n--- {class_name} ---\n")

                # Try to get properties via reflection
                try:
                    # Python dir() shows all Python-accessible attributes
                    for attr_name in dir(asset):
                        if not attr_name.startswith('_') and not callable(getattr(asset, attr_name, None)):
                            try:
                                val = asset.get_editor_property(attr_name)
                                f.write(f"  {attr_name} = {val}\n")
                            except:
                                pass
                except Exception as e:
                    f.write(f"  Error: {e}\n")

                current_class = current_class.get_super_class()

        print(f"  Written to {output_file}")

    print("\n\nDone!")

if __name__ == "__main__":
    extract_weapon_stats_t3d()
