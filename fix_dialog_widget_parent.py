"""Ensure W_Dialog widget is reparented to C++ UW_Dialog class."""
import unreal

widget_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Dialog"
new_parent = "/Script/SLFConversion.W_Dialog"

# Load the widget Blueprint
widget_bp = unreal.EditorAssetLibrary.load_asset(widget_path)
if not widget_bp:
    unreal.log_error(f"[ERROR] Could not load W_Dialog at {widget_path}")
else:
    unreal.log_warning(f"[OK] Loaded W_Dialog")

    # Try to reparent
    try:
        # Load the new parent class
        new_parent_class = unreal.load_class(None, new_parent)
        if not new_parent_class:
            unreal.log_error(f"[ERROR] Could not load parent class: {new_parent}")
        else:
            unreal.log_warning(f"[OK] Loaded parent class: {new_parent_class.get_name()}")

            # Reparent
            success = unreal.BlueprintEditorLibrary.reparent_blueprint(widget_bp, new_parent_class)
            if success:
                unreal.log_warning(f"[OK] Reparented W_Dialog to {new_parent}")

                # Compile
                unreal.BlueprintEditorLibrary.compile_blueprint(widget_bp)
                unreal.log_warning(f"[OK] Compiled W_Dialog")

                # Save
                unreal.EditorAssetLibrary.save_asset(widget_path)
                unreal.log_warning(f"[OK] Saved W_Dialog")
            else:
                unreal.log_warning(f"[INFO] Reparent returned false - widget may already have correct parent")
    except Exception as e:
        unreal.log_error(f"[ERROR] Exception during reparenting: {e}")
