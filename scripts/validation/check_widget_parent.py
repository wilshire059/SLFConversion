"""
Check parent class of settings widgets.
"""
import unreal

WIDGETS_TO_CHECK = [
    "/Game/SoulslikeFramework/Widgets/_Generic/W_Navigable",
    "/Game/SoulslikeFramework/Widgets/_Generic/W_Navigable_InputReader",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry",
]

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Checking Widget Parent Classes")
    unreal.log_warning("=" * 70)

    for path in WIDGETS_TO_CHECK:
        bp = unreal.load_asset(path)
        if not bp:
            unreal.log_warning(f"{path.split('/')[-1]}: NOT FOUND")
            continue

        # Get parent class via export_text
        export_text = bp.export_text()
        # Find NativeParentClass or ParentClass line
        for line in export_text.split('\n'):
            if 'ParentClass=' in line or 'NativeParentClass=' in line:
                unreal.log_warning(f"{path.split('/')[-1]}: {line.strip()}")
                break
        else:
            unreal.log_warning(f"{path.split('/')[-1]}: ParentClass not found in export")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
