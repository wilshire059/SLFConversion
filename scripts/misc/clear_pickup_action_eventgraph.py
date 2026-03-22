import unreal

# Clear the EventGraph of B_Action_PickupItemMontage so C++ _Implementation runs
bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_PickupItemMontage"

# Load the Blueprint
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if not bp:
    unreal.log_error(f"ERROR: Could not load Blueprint: {bp_path}")
else:
    unreal.log(f"Found Blueprint: {bp.get_name()}")
    unreal.log(f"Class: {bp.get_class().get_name()}")

    # Use PythonBridge to clear EventGraph (use classmethod directly, not instance)
    try:
        nodes_removed = unreal.PythonBridge.clear_event_graph(bp)
        unreal.log(f"Cleared {nodes_removed} nodes from EventGraph")

        if nodes_removed > 0:
            # Compile the Blueprint
            unreal.KismetEditorUtilities.compile_blueprint(bp)
            unreal.log("Compiled Blueprint")

            # Save the Blueprint
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log("Saved Blueprint")
            unreal.log("SUCCESS: EventGraph cleared! C++ _Implementation will now run.")
        else:
            unreal.log_warning("EventGraph was already empty or no nodes found")

    except Exception as e:
        unreal.log_error(f"Error using PythonBridge: {e}")
        import traceback
        traceback.print_exc()
