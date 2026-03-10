"""Check if W_Dialog widget is reparented to C++ class."""
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

        # Try to get parent via the blueprint editor library
        try:
            parent_class = unreal.BlueprintEditorLibrary.get_blueprint_native_class(widget_bp)
            if parent_class:
                parent_name = parent_class.get_name()
                parent_path = parent_class.get_path_name()
                unreal.log_warning(f"  Native parent class: {parent_name}")
                unreal.log_warning(f"  Native parent path: {parent_path}")

                if "W_Dialog" in parent_path and "/Script/SLFConversion" in parent_path:
                    unreal.log_warning(f"  [OK] Reparented to C++ UW_Dialog")
                elif "UserWidget" in parent_path:
                    unreal.log_warning(f"  [WARNING] Still using UUserWidget - NOT reparented")
                else:
                    unreal.log_warning(f"  [?] Unknown parent type")
            else:
                unreal.log_warning("  Native parent: None returned")
        except Exception as e:
            unreal.log_warning(f"  Could not get native parent: {e}")

            # Fallback: check via export_text
            export_text = unreal.EditorAssetLibrary.export_text_for_objects([widget_bp])
            # Look for parent class in export
            if "/Script/SLFConversion.W_Dialog" in export_text:
                unreal.log_warning(f"  [OK] Export text shows C++ parent")
            else:
                unreal.log_warning(f"  [?] Could not determine parent")
    else:
        unreal.log_error(f"  [ERROR] No generated class found")
