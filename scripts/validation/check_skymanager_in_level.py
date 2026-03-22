"""
Check B_SkyManager instance in the demo level
"""
import unreal

DEMO_LEVEL = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("B_SkyManager LEVEL INSTANCE CHECK")
    unreal.log_warning("=" * 70)

    # Load the demo level asset
    level_asset = unreal.EditorAssetLibrary.load_asset(DEMO_LEVEL)
    if not level_asset:
        unreal.log_warning(f"ERROR: Could not load level {DEMO_LEVEL}")
        return

    unreal.log_warning(f"Level: {level_asset.get_name()}")
    unreal.log_warning(f"Level class: {level_asset.get_class().get_name()}")

    # Try to find B_SkyManager in the level's package
    # We need to parse the level text to find actors
    export_path = "C:/scripts/SLFConversion/migration_cache/demo_level_export.txt"

    try:
        # Export level to text
        success = unreal.EditorAssetLibrary.export_asset(DEMO_LEVEL, export_path)
        unreal.log_warning(f"Export success: {success}")

        if success:
            with open(export_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()

            # Search for SkyManager references
            lines = content.split('\n')
            sky_lines = []
            for i, line in enumerate(lines):
                if 'SkyManager' in line:
                    # Get context (5 lines before and after)
                    start = max(0, i-2)
                    end = min(len(lines), i+3)
                    context = '\n'.join(lines[start:end])
                    sky_lines.append(f"Line {i}: {line.strip()}")

            unreal.log_warning(f"\nFound {len(sky_lines)} SkyManager references in level:")
            for line in sky_lines[:20]:  # Limit to first 20
                unreal.log_warning(f"  {line}")

    except Exception as e:
        unreal.log_warning(f"Export error: {e}")

    # Also check for any Blueprint references
    unreal.log_warning("\n\nChecking B_SkyManager Blueprint dependencies:")
    bp_path = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if bp:
        unreal.log_warning(f"  Blueprint: {bp.get_name()}")
        gen_class = bp.generated_class()
        if gen_class:
            unreal.log_warning(f"  Generated class: {gen_class.get_name()}")
            # Check parent
            parent = None
            try:
                # Use static class method
                parent = unreal.SystemLibrary.get_class_display_name(gen_class)
            except:
                pass
            if parent:
                unreal.log_warning(f"  Class display name: {parent}")

            # Try to get SCS
            try:
                scs = bp.get_editor_property('simple_construction_script')
                if scs:
                    nodes = scs.get_all_nodes()
                    unreal.log_warning(f"  SCS nodes: {len(nodes)}")
                    for node in nodes:
                        template = node.get_editor_property('component_template')
                        if template:
                            unreal.log_warning(f"    - {template.get_name()} ({template.get_class().get_name()})")
            except Exception as e:
                unreal.log_warning(f"  SCS error: {e}")

    unreal.log_warning("\n" + "=" * 70)

main()
