"""Check if W_Dialog widget is reparented to C++ class - simple version."""
import unreal

widget_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Dialog"

# Load the widget Blueprint
widget_bp = unreal.EditorAssetLibrary.load_asset(widget_path)
if not widget_bp:
    unreal.log_error(f"[ERROR] Could not load W_Dialog at {widget_path}")
else:
    unreal.log_warning(f"[OK] Loaded W_Dialog")

    # Get the generated class
    gen_class = widget_bp.generated_class()
    if gen_class:
        gen_class_name = gen_class.get_name()
        gen_class_path = gen_class.get_path_name()
        unreal.log_warning(f"  Generated class: {gen_class_name}")
        unreal.log_warning(f"  Full path: {gen_class_path}")

        # Get parent class via Blueprint property
        parent_class = widget_bp.get_editor_property("parent_class")
        if parent_class:
            parent_name = parent_class.get_name()
            parent_path = parent_class.get_path_name()
            unreal.log_warning(f"  Parent class (from property): {parent_name}")
            unreal.log_warning(f"  Parent path: {parent_path}")

            if "W_Dialog" in parent_name and "SLFConversion" in parent_path:
                unreal.log_warning(f"  [OK] Reparented to C++ UW_Dialog")
            elif "UserWidget" in parent_name:
                unreal.log_warning(f"  [WARNING] Still using UUserWidget - NOT reparented")
            else:
                unreal.log_warning(f"  [?] Unknown parent type: {parent_name}")
        else:
            unreal.log_warning("  Could not get parent_class property")
    else:
        unreal.log_error(f"  [ERROR] No generated class found")
