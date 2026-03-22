"""
Verify Settings widgets have C++ properties (proves reparent worked)
"""
import unreal

paths = [
    ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry", "resolutions"),  # Our TMap<FName, FIntPoint>
    ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry", "window_modes"),  # Our TMap<FName, EWindowMode>
    ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry", "custom_settings_asset"),  # UPDA_CustomSettings*
    ("/Game/SoulslikeFramework/Data/PDA_CustomSettings", "invert_cam_x"),  # bool
    ("/Game/SoulslikeFramework/Data/PDA_CustomSettings", "invert_cam_y"),  # bool
    ("/Game/SoulslikeFramework/Data/PDA_CustomSettings", "cam_speed"),  # double
]

for bp_path, prop_name in paths:
    name = bp_path.split('/')[-1]
    try:
        bp = unreal.load_asset(bp_path)
        if bp is None:
            unreal.log_warning(f"VERIFY: {name}.{prop_name}: BP NOT FOUND")
            continue

        gen_class = bp.generated_class() if hasattr(bp, 'generated_class') else None
        if gen_class is None:
            unreal.log_warning(f"VERIFY: {name}.{prop_name}: No generated_class")
            continue

        cdo = unreal.get_default_object(gen_class)
        if cdo is None:
            unreal.log_warning(f"VERIFY: {name}.{prop_name}: No CDO")
            continue

        # Check if property exists
        try:
            value = cdo.get_editor_property(prop_name)
            unreal.log_warning(f"VERIFY: {name}.{prop_name}: EXISTS (type={type(value).__name__})")
        except Exception as e:
            unreal.log_warning(f"VERIFY: {name}.{prop_name}: NOT FOUND ({e})")

    except Exception as e:
        unreal.log_warning(f"VERIFY: {name}.{prop_name}: Error - {e}")

unreal.log_warning("VERIFY: Done")
