"""Check parent classes of W_Settings_Entry and W_Settings_CategoryEntry."""
import unreal

def check_parent(asset_path, asset_name):
    bp = unreal.load_asset(asset_path)
    if not bp:
        unreal.log_warning(f'{asset_name} not found')
        return

    unreal.log_warning(f'{asset_name} asset class: {bp.get_class().get_name()}')

    # For WidgetBlueprint, use parent_class property
    try:
        parent_class = bp.get_editor_property('parent_class')
        if parent_class:
            unreal.log_warning(f'{asset_name} parent_class: {parent_class.get_name()}')
            unreal.log_warning(f'{asset_name} parent_class path: {parent_class.get_path_name()}')
        else:
            unreal.log_warning(f'{asset_name} parent_class is None')
    except Exception as e:
        unreal.log_warning(f'{asset_name} error getting parent_class: {e}')

    # Also try generated_class approach
    try:
        gen = bp.generated_class()
        if gen:
            unreal.log_warning(f'{asset_name} generated_class: {gen.get_name()}')
            # Get parent class name from the class hierarchy
            class_path = gen.get_path_name()
            unreal.log_warning(f'{asset_name} generated_class path: {class_path}')
    except Exception as e:
        unreal.log_warning(f'{asset_name} error getting generated_class: {e}')

def main():
    unreal.log_warning('=== Checking Settings Widget Parents ===')
    check_parent('/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry', 'W_Settings_Entry')
    unreal.log_warning('')
    check_parent('/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry', 'W_Settings_CategoryEntry')
    unreal.log_warning('')
    check_parent('/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings', 'W_Settings')
    unreal.log_warning('=== Done ===')

main()
