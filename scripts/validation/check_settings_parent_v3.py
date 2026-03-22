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

        gen_class = None
        if hasattr(bp, 'generated_class'):
            gen_class = bp.generated_class()

        if gen_class is None:
            unreal.log_warning(f"CHECK: {name}: No generated_class()")
            continue

        # Walk up the class hierarchy
        current_class = gen_class
        hierarchy = []
        while current_class:
            class_name = current_class.get_name()
            hierarchy.append(class_name)

            # Check if this is a native C++ class (ends without _C)
            if not class_name.endswith('_C'):
                break

            # Try to get parent
            parent = current_class.get_super_class() if hasattr(current_class, 'get_super_class') else None
            if parent is None:
                break
            current_class = parent

        unreal.log_warning(f"CHECK: {name}: Hierarchy = {' -> '.join(hierarchy)}")

    except Exception as e:
        unreal.log_warning(f"CHECK: {name}: Error - {e}")

unreal.log_warning("CHECK: Done")
