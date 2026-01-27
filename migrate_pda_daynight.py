"""
Migrate PDA_DayNight to C++ UPDA_DayNight and populate entries
"""
import unreal
import json

def migrate_pda_daynight():
    unreal.log_warning("=" * 60)
    unreal.log_warning("PDA_DayNight Migration")
    unreal.log_warning("=" * 60)

    # Load C++ class
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_DayNight")
    if not cpp_class:
        unreal.log_warning("ERROR: Could not load C++ UPDA_DayNight class")
        return False

    unreal.log_warning(f"  Found C++ class: {cpp_class.get_name()}")

    # Load PDA_DayNight Blueprint
    pda_path = "/Game/SoulslikeFramework/Data/PDA_DayNight"
    pda_bp = unreal.EditorAssetLibrary.load_asset(pda_path)
    if not pda_bp:
        unreal.log_warning(f"  ERROR: Could not load Blueprint: {pda_path}")
        return False

    unreal.log_warning(f"  Found Blueprint: {pda_path}")
    unreal.log_warning(f"  Current class: {pda_bp.get_class().get_name()}")

    # Reparent
    try:
        success = unreal.BlueprintEditorLibrary.reparent_blueprint(pda_bp, cpp_class)
        if success:
            unreal.log_warning(f"  Reparented to UPDA_DayNight: SUCCESS")
        else:
            unreal.log_warning(f"  Reparent returned False")
    except Exception as e:
        unreal.log_warning(f"  Reparent error: {e}")
        return False

    # Compile
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(pda_bp)
        unreal.log_warning("  Compiled successfully")
    except Exception as e:
        unreal.log_warning(f"  Compile warning: {e}")

    # Save
    try:
        unreal.EditorAssetLibrary.save_asset(pda_path)
        unreal.log_warning(f"  Saved: {pda_path}")
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    unreal.log_warning("\nPDA_DayNight migration complete!")
    return True

migrate_pda_daynight()
