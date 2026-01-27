import unreal
import os

# Migrate W_Resources using FULL CLEAR like migrate_execution_widget.py
# This clears ALL logic including variables, then reparents
# This might preserve the widget tree styling better

bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"
cpp_path = "/Script/SLFConversion.W_Resources"

# Load Blueprint
bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_error("ERROR: Could not load " + bp_path)
else:
    unreal.log("Loaded: " + bp.get_name())

    # Get file size before
    size_before = os.path.getsize("C:/scripts/SLFConversion/Content/SoulslikeFramework/Widgets/HUD/W_Resources.uasset")
    unreal.log("File size before: " + str(size_before) + " bytes")

    # Step 1: Clear ALL Blueprint logic (like execution widget migration)
    unreal.log("Step 1: Clearing ALL Blueprint logic (including variables)...")
    try:
        unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
        unreal.log("  Done")
    except Exception as e:
        unreal.log_warning("  Warning: " + str(e))

    # Save after clearing
    unreal.log("Saving after clear...")
    unreal.EditorAssetLibrary.save_asset(bp_path)

    # Reload after save
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error("Failed to reload after save!")
    else:
        unreal.log("Reloaded after save")

        # Step 2: Reparent to C++
        unreal.log("Step 2: Reparenting to C++...")
        try:
            ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_path)
            unreal.log("  Result: " + str(ok))
        except Exception as e:
            unreal.log_error("  Error: " + str(e))
            ok = False

        if ok:
            # Step 3: Compile and save
            unreal.log("Step 3: Compile and save...")
            try:
                result = unreal.SLFAutomationLibrary.compile_and_save(bp)
                unreal.log("  Result: " + str(result))
            except Exception as e:
                unreal.log_error("  Error: " + str(e))

    # Get file size after
    size_after = os.path.getsize("C:/scripts/SLFConversion/Content/SoulslikeFramework/Widgets/HUD/W_Resources.uasset")
    unreal.log("File size after: " + str(size_after) + " bytes")
    reduction = size_before - size_after
    percent = 100 - (size_after * 100 / size_before)
    unreal.log("Size reduction: " + str(reduction) + " bytes (" + str(round(percent, 1)) + "%)")

    unreal.log("Done!")
