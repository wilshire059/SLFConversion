# migrate_container_full.py
# Full migration of B_Container:
# 1. Reparent to ASLFContainerBase
# 2. Clear EventGraph
# 3. Apply ChestBoxMesh property

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("FULL MIGRATION OF B_Container")
    unreal.log_warning("=" * 70)

    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    # Step 1: Reparent to ASLFContainerBase
    unreal.log_warning("\n--- Step 1: Reparenting ---")
    cpp_class_path = "/Script/SLFConversion.SLFContainerBase"
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        unreal.log_error(f"Could not load C++ class: {cpp_class_path}")
        return

    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        unreal.log_warning(f"Reparent result: {result}")
    except Exception as e:
        unreal.log_error(f"Reparent error: {e}")
        return

    # Step 2: Clear EventGraph (keep variables and SCS)
    unreal.log_warning("\n--- Step 2: Clearing EventGraph ---")
    try:
        lib = unreal.SLFAutomationLibrary
        result = lib.clear_graphs_keep_variables_no_compile(bp)
        unreal.log_warning(f"Clear graphs result: {result}")
    except Exception as e:
        unreal.log_error(f"Clear graphs error: {e}")
        return

    # Step 3: Apply ChestBoxMesh property
    unreal.log_warning("\n--- Step 3: Applying ChestBoxMesh ---")
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("No generated class after reparent")
        return

    cdo = unreal.get_default_object(gen_class)

    chest_box_path = "/Game/SoulslikeFramework/Meshes/SM/Chest/SM_ChestBox"
    chest_box_mesh = unreal.load_asset(chest_box_path)
    if not chest_box_mesh:
        unreal.log_error(f"Could not load chest box mesh: {chest_box_path}")
    else:
        try:
            cdo.set_editor_property("chest_box_mesh", chest_box_mesh)
            unreal.log_warning(f"Set chest_box_mesh: {chest_box_mesh.get_name()}")
        except Exception as e:
            unreal.log_error(f"Failed to set chest_box_mesh: {e}")

    # Verify SCS components exist
    unreal.log_warning("\n--- Verifying SCS Components ---")
    try:
        scs = bp.get_editor_property("simple_construction_script")
        if scs:
            all_nodes = scs.get_all_nodes()
            unreal.log_warning(f"SCS nodes: {len(all_nodes)}")
            for node in all_nodes:
                var_name = node.get_editor_property("internal_variable_name")
                comp_template = node.get_editor_property("component_template")
                if comp_template:
                    comp_class = comp_template.get_class().get_name()
                    unreal.log_warning(f"  {var_name} ({comp_class})")

                    # Log mesh for StaticMeshComponents
                    if isinstance(comp_template, unreal.StaticMeshComponent):
                        try:
                            mesh = comp_template.get_editor_property("static_mesh")
                            unreal.log_warning(f"    -> Mesh: {mesh.get_name() if mesh else 'None'}")
                        except:
                            pass
    except Exception as e:
        unreal.log_warning(f"SCS verification error: {e}")

    # Save the Blueprint
    unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.log_warning(f"\nSaved: {bp_path}")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Done - B_Container migration complete")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
