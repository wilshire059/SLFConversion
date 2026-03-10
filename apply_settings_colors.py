"""
Apply bp_only color values to settings widgets.
"""
import unreal

# Values from bp_only extraction
BP_ONLY_DATA = {
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry": {
        "UnhoveredColor": unreal.LinearColor(r=0.047059, g=0.047059, b=0.047059, a=0.901961),
        "HoveredColor": unreal.LinearColor(r=0.065, g=0.065, b=0.065, a=0.901961),
        "SettingName": "Setting Entry",
        "SettingDescription": "Setting Entry",
        # EntryType: E_SettingEntry.DROP_DOWN = 0
    },
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry": {
        "SelectedColor": unreal.LinearColor(r=0.859375, g=0.744195, b=0.61834, a=1.0),
        "DeselectedColor": unreal.LinearColor(r=0.494118, g=0.392157, b=0.282353, a=1.0),
    }
}

def main():
    unreal.log_warning("=== APPLYING BP_ONLY SETTINGS COLORS ===")

    for path, props in BP_ONLY_DATA.items():
        name = path.split('/')[-1]
        unreal.log_warning(f"\n--- {name} ---")

        bp = unreal.load_asset(path)
        if not bp:
            unreal.log_warning(f"  FAILED to load {path}")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            unreal.log_warning(f"  No generated_class")
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            unreal.log_warning(f"  No CDO")
            continue

        for prop_name, value in props.items():
            try:
                # Try both snake_case and PascalCase
                snake_name = ''.join(['_'+c.lower() if c.isupper() else c for c in prop_name]).lstrip('_')

                try:
                    cdo.set_editor_property(snake_name, value)
                    unreal.log_warning(f"  Set {snake_name}: OK")
                except:
                    try:
                        cdo.set_editor_property(prop_name, value)
                        unreal.log_warning(f"  Set {prop_name}: OK")
                    except Exception as e:
                        unreal.log_warning(f"  Set {prop_name}: FAILED - {e}")
            except Exception as e:
                unreal.log_warning(f"  {prop_name}: ERROR - {e}")

        # Save the asset
        unreal.EditorAssetLibrary.save_asset(path, False)
        unreal.log_warning(f"  Saved {name}")

    unreal.log_warning("\n=== DONE ===")

main()
