"""
Compare B_SkyManager SCS between bp_only and SLFConversion
by exporting the Blueprint to T3D format
"""
import unreal
import os

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
OUTPUT_DIR = "C:/scripts/SLFConversion/migration_cache/"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("B_SkyManager SCS EXPORT")
    unreal.log_warning("=" * 70)

    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load {BP_PATH}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")

    # Get the generated class
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"Generated class: {gen_class.get_name()}")

        # Try to find parent class
        try:
            parent = gen_class.get_super_class()
            if parent:
                unreal.log_warning(f"Parent class: {parent.get_name()}")
        except:
            pass

    # Export the blueprint to T3D
    output_file = OUTPUT_DIR + "skymanager_export.t3d"
    try:
        success = unreal.EditorAssetLibrary.export_asset(BP_PATH, output_file)
        unreal.log_warning(f"\nExport to T3D: {success}")
        unreal.log_warning(f"Output: {output_file}")

        if success and os.path.exists(output_file):
            with open(output_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()

            # Find component sections
            unreal.log_warning(f"\nT3D file size: {len(content)} bytes")

            # Search for component definitions
            import re
            component_pattern = r'Begin Object Class=(\w+) Name="?(\w+)"?'
            components = re.findall(component_pattern, content)
            unreal.log_warning(f"\nComponents found in T3D:")
            for class_name, name in components:
                unreal.log_warning(f"  {name} ({class_name})")

            # Look specifically for DirectionalLight
            if 'DirectionalLight' in content:
                unreal.log_warning("\n** DirectionalLight IS present in T3D **")

                # Find DirectionalLight sections
                light_pattern = r'Begin Object Class=DirectionalLightComponent Name="?(\w+)"?'
                lights = re.findall(light_pattern, content)
                for name in lights:
                    unreal.log_warning(f"  Found DirectionalLightComponent: {name}")
            else:
                unreal.log_warning("\n** DirectionalLight NOT in T3D **")

    except Exception as e:
        unreal.log_warning(f"Export error: {e}")

    unreal.log_warning("\n" + "=" * 70)

main()
