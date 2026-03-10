import unreal

paths = [
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry",
    "/Game/SoulslikeFramework/Data/PDA_CustomSettings",
]

def get_class_hierarchy(gen_class):
    """Walk up the class hierarchy and return class names."""
    hierarchy = []
    current = gen_class
    max_depth = 10  # prevent infinite loops

    while current and max_depth > 0:
        class_name = current.get_name()
        hierarchy.append(class_name)

        # Check if we've reached a known base class
        if class_name in ['UserWidget', 'PrimaryDataAsset', 'Object']:
            break

        # Get parent class
        try:
            # Use static method
            current = unreal.SystemLibrary.get_super_class(current)
        except:
            break
        max_depth -= 1

    return hierarchy

for bp_path in paths:
    name = bp_path.split('/')[-1]
    try:
        bp = unreal.load_asset(bp_path)
        if bp is None:
            unreal.log_warning(f"CHECK: {name}: NOT FOUND")
            continue

        gen_class = bp.generated_class() if hasattr(bp, 'generated_class') else None
        if gen_class is None:
            unreal.log_warning(f"CHECK: {name}: No generated_class()")
            continue

        hierarchy = get_class_hierarchy(gen_class)
        unreal.log_warning(f"CHECK: {name}: Hierarchy = {' -> '.join(hierarchy)}")

    except Exception as e:
        unreal.log_warning(f"CHECK: {name}: Error - {e}")

unreal.log_warning("CHECK: Done")
