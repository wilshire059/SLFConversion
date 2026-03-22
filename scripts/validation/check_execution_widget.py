"""Check ExecutionWidget component in character hierarchy."""
import unreal

def check_execution_widget():
    """Check if B_BaseCharacter has ExecutionWidget by spawning and inspecting."""

    base_path = '/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter'
    enemy_path = '/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy'
    guard_path = '/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard'

    paths = [base_path, enemy_path, guard_path]

    for bp_path in paths:
        bp = unreal.load_asset(bp_path)
        if not bp:
            print(f"Could not load: {bp_path}")
            continue

        print(f"\n=== {bp_path.split('/')[-1]} ===")

        gen_class = bp.generated_class()
        if not gen_class:
            print("  No generated class")
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            print("  No CDO")
            continue

        print(f"  Class: {gen_class.get_name()}")
        print(f"  Parent: {gen_class.get_super_class().get_name() if gen_class.get_super_class() else 'None'}")

        # Check for WidgetComponent in the class hierarchy
        # Use export_text to get all component info
        export_text = unreal.EditorAssetLibrary.export_asset_to_text(bp_path)
        if export_text:
            lines = export_text.split('\n')
            widget_lines = [l for l in lines if 'Widget' in l or 'Execution' in l]
            for wl in widget_lines[:10]:
                print(f"  {wl[:100]}")
            if not widget_lines:
                print("  No Widget-related lines found")
        else:
            print("  Could not export")

if __name__ == "__main__":
    check_execution_widget()
