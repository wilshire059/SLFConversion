import unreal

paths = [
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry",
    "/Game/SoulslikeFramework/Data/PDA_CustomSettings",
]

for bp_path in paths:
    name = bp_path.split('/')[-1]
    try:
        bp = unreal.load_asset(bp_path)
        if bp is None:
            unreal.log_warning(f"CHECK: {name}: NOT FOUND")
            continue

        # Get the generated class
        gen_class = None
        if hasattr(bp, 'generated_class'):
            gen_class = bp.generated_class()

        if gen_class is None:
            unreal.log_warning(f"CHECK: {name}: No generated_class()")
            continue

        # Try to find the parent class via CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            class_name = cdo.get_class().get_name()
            outer_name = cdo.get_class().get_outer().get_name() if cdo.get_class().get_outer() else "None"
            unreal.log_warning(f"CHECK: {name}: CDO Class={class_name}, Outer={outer_name}")
        else:
            unreal.log_warning(f"CHECK: {name}: No CDO")

    except Exception as e:
        unreal.log_warning(f"CHECK: {name}: Error - {e}")

unreal.log_warning("CHECK: Done")
