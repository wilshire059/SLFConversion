"""Diagnose W_Dialog widget reparenting and W_HUD widget tree."""
import unreal

def diagnose_w_dialog():
    """Check W_Dialog parent class and W_HUD widget tree."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("[Dialog Widget Diagnosis] Starting...")
    unreal.log_warning("=" * 70)

    # Check 1: W_Dialog Blueprint parent class
    unreal.log_warning("\n=== Check 1: W_Dialog Blueprint Parent ===")
    w_dialog_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Dialog"
    w_dialog_bp = unreal.EditorAssetLibrary.load_asset(w_dialog_path)

    if not w_dialog_bp:
        unreal.log_error(f"  [FAIL] Could not load W_Dialog at {w_dialog_path}")
        return

    unreal.log_warning(f"  [OK] Loaded W_Dialog Blueprint")

    # Get parent class via Blueprint property
    try:
        parent_class = w_dialog_bp.get_editor_property("parent_class")
        if parent_class:
            parent_name = parent_class.get_name()
            parent_path = parent_class.get_path_name()
            unreal.log_warning(f"  Parent class: {parent_name}")
            unreal.log_warning(f"  Parent path: {parent_path}")

            # Check if parent is C++ UW_Dialog
            if "W_Dialog" in parent_name and "/Script/SLFConversion" in parent_path:
                unreal.log_warning(f"  [PASS] Parent is C++ UW_Dialog")
            elif "UserWidget" in parent_name:
                unreal.log_error(f"  [FAIL] Parent is UUserWidget - NOT REPARENTED!")
                unreal.log_error(f"         Need to reparent to /Script/SLFConversion.W_Dialog")
            else:
                unreal.log_warning(f"  [INFO] Parent: {parent_name} at {parent_path}")
        else:
            unreal.log_error(f"  [FAIL] No parent class found")
    except Exception as e:
        unreal.log_error(f"  Error getting parent_class: {e}")

    # Check 2: W_HUD parent class
    unreal.log_warning("\n=== Check 2: W_HUD Blueprint Parent ===")
    w_hud_path = "/Game/SoulslikeFramework/Widgets/HUD/W_HUD"
    w_hud_bp = unreal.EditorAssetLibrary.load_asset(w_hud_path)

    if not w_hud_bp:
        unreal.log_error(f"  [FAIL] Could not load W_HUD at {w_hud_path}")
        return

    unreal.log_warning(f"  [OK] Loaded W_HUD Blueprint")

    try:
        parent_class = w_hud_bp.get_editor_property("parent_class")
        if parent_class:
            parent_name = parent_class.get_name()
            parent_path = parent_class.get_path_name()
            unreal.log_warning(f"  W_HUD Parent: {parent_name}")
            unreal.log_warning(f"  W_HUD Parent path: {parent_path}")
    except Exception as e:
        unreal.log_error(f"  Error getting W_HUD parent_class: {e}")

    # Check 3: UW_Dialog C++ class existence
    unreal.log_warning("\n=== Check 3: C++ UW_Dialog Class ===")
    try:
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.W_Dialog")
        if cpp_class:
            unreal.log_warning(f"  [PASS] C++ UW_Dialog class exists: {cpp_class.get_name()}")
        else:
            unreal.log_error(f"  [FAIL] C++ UW_Dialog class not found!")
    except Exception as e:
        unreal.log_error(f"  [FAIL] Error loading C++ class: {e}")

    # Check 4: Export text to see the REAL parent
    unreal.log_warning("\n=== Check 4: Export Text Analysis ===")
    try:
        export_text = unreal.Exporter.export_to_file_as_string(w_dialog_bp)
        if export_text:
            # Look for ParentClass in export
            lines = export_text.split('\n')
            for line in lines[:100]:  # Only first 100 lines
                if 'ParentClass' in line or 'SuperStruct' in line or 'Parent=' in line:
                    unreal.log_warning(f"  {line.strip()}")
    except Exception as e:
        unreal.log_warning(f"  Could not export text: {e}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("[Dialog Widget Diagnosis] Complete")
    unreal.log_warning("=" * 70)

# Run diagnosis
diagnose_w_dialog()
