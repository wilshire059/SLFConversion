# diagnose_container_components.py
# Check B_Container component hierarchy and CDO values

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("DIAGNOSING B_Container COMPONENTS")
    unreal.log_warning("=" * 70)

    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("No generated class")
        return

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    unreal.log_warning(f"\nCDO: {cdo.get_name()}")

    # Check key properties
    unreal.log_warning("\n--- CDO Properties ---")
    try:
        open_vfx = cdo.get_editor_property("open_vfx")
        unreal.log_warning(f"OpenVFX: {open_vfx}")
    except Exception as e:
        unreal.log_warning(f"OpenVFX error: {e}")

    try:
        open_montage = cdo.get_editor_property("open_montage")
        unreal.log_warning(f"OpenMontage: {open_montage}")
    except Exception as e:
        unreal.log_warning(f"OpenMontage error: {e}")

    try:
        lid_angle = cdo.get_editor_property("lid_open_angle")
        unreal.log_warning(f"LidOpenAngle: {lid_angle}")
    except Exception as e:
        unreal.log_warning(f"LidOpenAngle error: {e}")

    # Check components on CDO
    unreal.log_warning("\n--- CDO Components ---")
    try:
        components = cdo.get_components_by_class(unreal.ActorComponent)
        unreal.log_warning(f"Total components: {len(components)}")
        for comp in components:
            comp_name = comp.get_name()
            comp_class = comp.get_class().get_name()
            unreal.log_warning(f"  {comp_name} ({comp_class})")

            # Check relative location/rotation for scene components
            if isinstance(comp, unreal.SceneComponent):
                try:
                    rel_loc = comp.get_editor_property("relative_location")
                    rel_rot = comp.get_editor_property("relative_rotation")
                    unreal.log_warning(f"    RelLoc: {rel_loc}")
                    unreal.log_warning(f"    RelRot: {rel_rot}")
                except:
                    pass
    except Exception as e:
        unreal.log_warning(f"Components error: {e}")

    # Check SimpleConstructionScript
    unreal.log_warning("\n--- Blueprint SCS (SimpleConstructionScript) ---")
    try:
        scs = bp.get_editor_property("simple_construction_script")
        if scs:
            root_nodes = scs.get_editor_property("root_nodes")
            unreal.log_warning(f"Root nodes: {len(root_nodes) if root_nodes else 0}")

            def print_scs_node(node, indent=0):
                if not node:
                    return
                template = node.get_editor_property("component_template")
                if template:
                    name = template.get_name()
                    cls = template.get_class().get_name()
                    unreal.log_warning(f"{'  ' * indent}{name} ({cls})")

                    # Print relative transform
                    if isinstance(template, unreal.SceneComponent):
                        try:
                            rel_loc = template.get_editor_property("relative_location")
                            rel_rot = template.get_editor_property("relative_rotation")
                            unreal.log_warning(f"{'  ' * indent}  Loc: {rel_loc}")
                            unreal.log_warning(f"{'  ' * indent}  Rot: {rel_rot}")
                        except:
                            pass

                # Child nodes
                children = node.get_editor_property("child_nodes")
                if children:
                    for child in children:
                        print_scs_node(child, indent + 1)

            if root_nodes:
                for node in root_nodes:
                    print_scs_node(node)
        else:
            unreal.log_warning("No SCS found")
    except Exception as e:
        unreal.log_warning(f"SCS error: {e}")

    # Also check bp_only version for comparison
    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("CHECKING BP_ONLY VERSION FOR COMPARISON")
    unreal.log_warning("=" * 70)

    # Load from backup project to compare
    # (Can't directly load from different project, but we can note what to compare)
    unreal.log_warning("Compare with bp_only/B_Container to see original hierarchy")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
