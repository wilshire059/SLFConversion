import unreal

# Migrate W_Resources: Clear graphs (removes problematic Create Widget calls)
# but KEEP variables and widget tree (preserves visual styling)

bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"
cpp_path = "/Script/SLFConversion.W_Resources"

unreal.log_warning("=== W_Resources Migration (preserve widget tree) ===")

# Load asset
bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_error("ERROR: Could not load " + bp_path)
else:
    # Step 1: Clear ONLY graphs, keep variables (this preserves widget tree)
    unreal.log_warning("Step 1: Clearing graphs (keeping variables and widget tree)")
    unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)

    # Save immediately to persist the clear
    unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.log_warning("Saved after clearing graphs")

    # Reload the asset
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error("ERROR: Could not reload asset")
    else:
        # Step 2: Reparent to C++
        unreal.log_warning("Step 2: Reparenting to C++ parent")
        ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_path)
        unreal.log_warning(f"Reparent result: {ok}")

        # Step 3: Compile and save
        unreal.log_warning("Step 3: Compile and save")
        result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        unreal.log_warning(f"Compile result: {result}")

        if result:
            unreal.log_warning("SUCCESS: W_Resources migrated with widget tree preserved!")
        else:
            unreal.log_error("FAILED: Compile failed")

    unreal.log_warning("=== Done ===")
