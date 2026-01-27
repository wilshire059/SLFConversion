"""
Check B_SkyManager SCS components in detail
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("B_SkyManager SCS COMPONENT CHECK")
    unreal.log_warning("=" * 70)

    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load {BP_PATH}")
        return

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    # Check parent class chain
    unreal.log_warning("\nPARENT CLASS CHAIN:")
    try:
        gen_class = bp.generated_class()
        if gen_class:
            current = gen_class
            depth = 0
            while current:
                unreal.log_warning(f"  {'  ' * depth}{current.get_name()}")
                try:
                    current = current.get_super_class()
                except:
                    break
                depth += 1
    except Exception as e:
        unreal.log_warning(f"  Error: {e}")

    # Check SCS
    unreal.log_warning("\nSCS (SimpleConstructionScript):")
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            unreal.log_warning(f"  SCS exists: {scs}")

            # Get all nodes
            all_nodes = scs.get_all_nodes()
            unreal.log_warning(f"  Total nodes: {len(all_nodes)}")

            for i, node in enumerate(all_nodes):
                comp_template = node.get_editor_property('component_template')
                if comp_template:
                    comp_name = comp_template.get_name()
                    comp_class = comp_template.get_class().get_name() if comp_template.get_class() else "Unknown"
                    unreal.log_warning(f"\n  [{i}] {comp_name} ({comp_class})")

                    # Check if it's a DirectionalLightComponent
                    if "Light" in comp_class:
                        try:
                            intensity = comp_template.get_editor_property('intensity')
                            unreal.log_warning(f"      Intensity: {intensity}")
                        except Exception as e:
                            unreal.log_warning(f"      Intensity error: {e}")

                        try:
                            mobility = comp_template.get_editor_property('mobility')
                            unreal.log_warning(f"      Mobility: {mobility}")
                        except Exception as e:
                            unreal.log_warning(f"      Mobility error: {e}")
                else:
                    unreal.log_warning(f"  [{i}] (no component template)")
        else:
            unreal.log_warning("  SCS is None!")
    except Exception as e:
        unreal.log_warning(f"  Error: {e}")

    # Also check inherited components from C++ parent
    unreal.log_warning("\nC++ PARENT DEFAULT COMPONENTS:")
    try:
        # Load the C++ class
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.SLFSkyManager")
        if cpp_class:
            cdo = unreal.get_default_object(cpp_class)
            if cdo:
                components = cdo.get_components_by_class(unreal.ActorComponent)
                for comp in components:
                    unreal.log_warning(f"  {comp.get_name()} ({comp.get_class().get_name()})")
                if len(components) == 0:
                    unreal.log_warning("  (no components)")
        else:
            unreal.log_warning("  Could not load SLFSkyManager class")
    except Exception as e:
        unreal.log_warning(f"  Error: {e}")

    unreal.log_warning("\n" + "=" * 70)

main()
