"""Fix W_Dialog widget reparenting to C++ UW_Dialog class."""
import unreal

# Widget path and new parent class
widget_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Dialog"
new_parent_class_path = "/Script/SLFConversion.W_Dialog"

def fix_w_dialog():
    """Reparent W_Dialog Blueprint to C++ UW_Dialog class."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("[W_Dialog Reparenting] Starting...")
    unreal.log_warning("=" * 70)

    # Load the widget Blueprint
    widget_bp = unreal.EditorAssetLibrary.load_asset(widget_path)
    if not widget_bp:
        unreal.log_error(f"[ERROR] Could not load W_Dialog at {widget_path}")
        return False

    unreal.log_warning(f"[OK] Loaded W_Dialog Blueprint")

    # Check current parent
    try:
        parent_class = widget_bp.get_editor_property("parent_class")
        if parent_class:
            parent_name = parent_class.get_name()
            parent_path = parent_class.get_path_name()
            unreal.log_warning(f"  Current parent: {parent_name}")
            unreal.log_warning(f"  Current parent path: {parent_path}")

            # Check if already reparented
            if "W_Dialog" in parent_name and "SLFConversion" in parent_path:
                unreal.log_warning(f"[OK] W_Dialog is already reparented to C++ UW_Dialog!")
                return True
    except Exception as e:
        unreal.log_warning(f"  Could not get current parent: {e}")

    # Load the new parent class
    try:
        new_parent_class = unreal.load_class(None, new_parent_class_path)
        if not new_parent_class:
            unreal.log_error(f"[ERROR] Could not load C++ parent class: {new_parent_class_path}")
            return False
        unreal.log_warning(f"[OK] Loaded C++ parent class: {new_parent_class.get_name()}")
    except Exception as e:
        unreal.log_error(f"[ERROR] Exception loading parent class: {e}")
        return False

    # Reparent the Blueprint
    try:
        success = unreal.BlueprintEditorLibrary.reparent_blueprint(widget_bp, new_parent_class)
        if success:
            unreal.log_warning(f"[OK] Reparented W_Dialog to {new_parent_class_path}")
        else:
            unreal.log_warning(f"[INFO] Reparent returned false - may already have correct parent or cannot reparent")
    except Exception as e:
        unreal.log_error(f"[ERROR] Exception during reparenting: {e}")
        return False

    # Compile the Blueprint
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(widget_bp)
        unreal.log_warning(f"[OK] Compiled W_Dialog Blueprint")
    except Exception as e:
        unreal.log_error(f"[ERROR] Exception during compilation: {e}")

    # Save the Blueprint
    try:
        unreal.EditorAssetLibrary.save_asset(widget_path)
        unreal.log_warning(f"[OK] Saved W_Dialog Blueprint")
    except Exception as e:
        unreal.log_error(f"[ERROR] Exception during save: {e}")
        return False

    # Verify reparenting
    try:
        # Reload to verify
        widget_bp = unreal.EditorAssetLibrary.load_asset(widget_path)
        parent_class = widget_bp.get_editor_property("parent_class")
        if parent_class:
            parent_name = parent_class.get_name()
            parent_path = parent_class.get_path_name()
            unreal.log_warning(f"  Final parent: {parent_name}")
            unreal.log_warning(f"  Final parent path: {parent_path}")

            if "W_Dialog" in parent_name and "SLFConversion" in parent_path:
                unreal.log_warning(f"[SUCCESS] W_Dialog is now reparented to C++ UW_Dialog!")
                return True
    except Exception as e:
        unreal.log_warning(f"  Could not verify: {e}")

    unreal.log_warning("=" * 70)
    unreal.log_warning("[W_Dialog Reparenting] Complete")
    unreal.log_warning("=" * 70)
    return True

# Run the fix
fix_w_dialog()
