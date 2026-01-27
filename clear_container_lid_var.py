# clear_container_lid_var.py
# Remove the conflicting Lid variable from B_Container Blueprint

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CLEARING LID VARIABLE FROM B_Container")
    unreal.log_warning("=" * 70)

    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    # Get SCS and check for Lid component/variable
    try:
        scs = bp.get_editor_property("simple_construction_script")
        if scs:
            all_nodes = scs.get_all_nodes()
            unreal.log_warning(f"SCS nodes: {len(all_nodes)}")

            for node in all_nodes:
                var_name = node.get_editor_property("internal_variable_name")
                comp_template = node.get_editor_property("component_template")
                unreal.log_warning(f"  Node: {var_name}, Template: {comp_template.get_name() if comp_template else 'None'}")

                # Remove Lid node if found
                if str(var_name) == "Lid":
                    unreal.log_warning(f"  >>> Removing Lid node from SCS")
                    scs.remove_node(node)
    except Exception as e:
        unreal.log_warning(f"SCS error: {e}")

    # Use SLFAutomationLibrary to clear any remaining variable
    try:
        lib = unreal.SLFAutomationLibrary
        unreal.log_warning("Using SLFAutomationLibrary to remove Lid variable...")

        # Remove variable by name
        result = lib.remove_blueprint_variable(bp, "Lid")
        unreal.log_warning(f"Remove Lid variable result: {result}")
    except Exception as e:
        unreal.log_warning(f"SLFAutomationLibrary error: {e}")

    # Save the Blueprint
    unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.log_warning(f"Saved: {bp_path}")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
