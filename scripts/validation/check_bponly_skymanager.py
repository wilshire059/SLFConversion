"""
Check bp_only B_SkyManager for SCS components
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING bp_only B_SkyManager SCS COMPONENTS")
    unreal.log_warning("=" * 70)

    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load {BP_PATH}")
        return

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    # Check CDO for components
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log_warning(f"\nCDO: {cdo.get_name()}")

            # Try all component types
            for comp_class in [unreal.ActorComponent, unreal.SceneComponent, unreal.LightComponent]:
                try:
                    comps = cdo.get_components_by_class(comp_class)
                    if comps:
                        unreal.log_warning(f"\n{comp_class.get_name()} components: {len(comps)}")
                        for comp in comps:
                            unreal.log_warning(f"  - {comp.get_name()}: {comp.get_class().get_name()}")
                except Exception as e:
                    pass

            # List all attributes on CDO
            unreal.log_warning("\nCDO attributes with 'light' or 'sun' or 'moon':")
            try:
                props = dir(cdo)
                for p in props:
                    if any(x in p.lower() for x in ['light', 'sun', 'moon', 'sky']):
                        unreal.log_warning(f"  {p}")
            except:
                pass

    unreal.log_warning("\n" + "=" * 70)

main()
