# extract_bponly_container.py
# Extract B_Container SCS components and CDO settings from bp_only project

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("EXTRACTING B_Container FROM BP_ONLY")
    unreal.log_warning("=" * 70)

    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")

    # Get parent class
    try:
        parent = bp.get_editor_property("parent_class")
        unreal.log_warning(f"Parent class: {parent.get_name() if parent else 'None'}")
    except Exception as e:
        unreal.log_warning(f"Parent class error: {e}")

    # Get generated class
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("No generated class")
        return

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    unreal.log_warning(f"\nCDO: {cdo.get_name()}")

    # Check CDO components
    unreal.log_warning("\n--- CDO Components ---")
    try:
        components = cdo.get_components_by_class(unreal.ActorComponent)
        unreal.log_warning(f"Total CDO components: {len(components)}")
        for comp in components:
            comp_name = comp.get_name()
            comp_class = comp.get_class().get_name()
            unreal.log_warning(f"  {comp_name} ({comp_class})")

            if isinstance(comp, unreal.SceneComponent):
                try:
                    rel_loc = comp.get_editor_property("relative_location")
                    rel_rot = comp.get_editor_property("relative_rotation")
                    rel_scale = comp.get_editor_property("relative_scale3d")
                    unreal.log_warning(f"    Loc: {rel_loc}")
                    unreal.log_warning(f"    Rot: {rel_rot}")
                    unreal.log_warning(f"    Scale: {rel_scale}")
                except:
                    pass

            # Check if it's a mesh component
            if isinstance(comp, unreal.StaticMeshComponent):
                try:
                    mesh = comp.get_editor_property("static_mesh")
                    unreal.log_warning(f"    Mesh: {mesh.get_name() if mesh else 'None'}")
                except:
                    pass
    except Exception as e:
        unreal.log_warning(f"CDO components error: {e}")

    # Check SimpleConstructionScript
    unreal.log_warning("\n--- Blueprint SCS (SimpleConstructionScript) ---")
    try:
        # Access SCS through the Blueprint's generated class
        # In UE5, we need to access SCS differently

        # Try to get SCS nodes by looking at the Blueprint class hierarchy
        all_nodes = []

        # Use EditorUtilityLibrary to get SCS info
        editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)

        # Export the Blueprint to text to see SCS
        unreal.log_warning("Exporting Blueprint to text...")

        # Get all default subobjects
        unreal.log_warning("\n--- Default Subobjects ---")

        # Try to enumerate components from the class
        class_obj = gen_class
        unreal.log_warning(f"Class: {class_obj.get_name()}")

    except Exception as e:
        unreal.log_warning(f"SCS error: {e}")

    # Check key properties
    unreal.log_warning("\n--- Key CDO Properties ---")
    props_to_check = [
        "open_montage", "open_vfx", "move_distance", "speed_multiplier",
        "cached_intensity", "interactable_display_name", "interaction_text"
    ]
    for prop in props_to_check:
        try:
            val = cdo.get_editor_property(prop)
            unreal.log_warning(f"  {prop}: {val}")
        except Exception as e:
            unreal.log_warning(f"  {prop}: ERROR - {e}")

    # Export to T3D to see full structure
    unreal.log_warning("\n--- Exporting to T3D ---")
    try:
        t3d = unreal.EditorAssetLibrary.export_asset_to_string(bp_path)
        if t3d:
            # Save to file for analysis
            with open("C:/scripts/SLFConversion/migration_cache/b_container_bponly.t3d", "w") as f:
                f.write(t3d)
            unreal.log_warning("Exported to migration_cache/b_container_bponly.t3d")

            # Print first 2000 chars
            unreal.log_warning("\nT3D Preview (first 2000 chars):")
            unreal.log_warning(t3d[:2000])
    except Exception as e:
        unreal.log_warning(f"T3D export error: {e}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
