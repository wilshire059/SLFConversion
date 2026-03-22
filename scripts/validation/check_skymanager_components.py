"""
Check if B_SkyManager has SCS components after migration
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING B_SkyManager SCS COMPONENTS")
    unreal.log_warning("=" * 70)

    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load {BP_PATH}")
        return

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    # Check parent class
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"Generated class: {gen_class.get_name()}")
        is_slf = unreal.MathLibrary.class_is_child_of(
            gen_class,
            unreal.load_class(None, "/Script/SLFConversion.SLFSkyManager")
        )
        unreal.log_warning(f"Is child of SLFSkyManager: {is_slf}")

    # Check CDO for components
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log_warning(f"\nCDO: {cdo.get_name()}")

            # Try to get components
            try:
                comps = cdo.get_components_by_class(unreal.ActorComponent)
                unreal.log_warning(f"Components found: {len(comps)}")
                for comp in comps:
                    unreal.log_warning(f"  - {comp.get_name()}: {comp.get_class().get_name()}")
            except Exception as e:
                unreal.log_warning(f"Error getting components: {e}")

    # Check SCS directly
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            unreal.log_warning(f"\nSCS found: {scs}")
            nodes = scs.get_editor_property('all_nodes')
            if nodes:
                unreal.log_warning(f"SCS nodes: {len(nodes)}")
                for node in nodes:
                    template = node.get_editor_property('component_template')
                    if template:
                        unreal.log_warning(f"  - {template.get_name()}: {template.get_class().get_name()}")
    except Exception as e:
        unreal.log_warning(f"SCS error: {e}")

    unreal.log_warning("\n" + "=" * 70)

main()
