"""
Add DirectionalLight components to B_SkyManager Blueprint
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("ADD COMPONENTS TO B_SkyManager")
    unreal.log_warning("=" * 70)

    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load {BP_PATH}")
        return

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    # Get the SimpleConstructionScript
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if not scs:
            unreal.log_warning("ERROR: No SimpleConstructionScript found")
            return
        unreal.log_warning(f"SCS: {scs}")

        # Add root scene component
        root_node = unreal.SubobjectDataBlueprintFunctionLibrary.add_new_subobject(
            unreal.SubobjectDataHandle(),
            bp,
            unreal.SceneComponent,
            unreal.Name("DefaultSceneRoot")
        )
        unreal.log_warning(f"Added root: {root_node}")

        # Add DirectionalLight for Sun
        sun_node = unreal.SubobjectDataBlueprintFunctionLibrary.add_new_subobject(
            root_node,
            bp,
            unreal.DirectionalLightComponent,
            unreal.Name("DirectionalLight_Sun")
        )
        unreal.log_warning(f"Added sun: {sun_node}")

        # Add DirectionalLight for Moon
        moon_node = unreal.SubobjectDataBlueprintFunctionLibrary.add_new_subobject(
            root_node,
            bp,
            unreal.DirectionalLightComponent,
            unreal.Name("DirectionalLight_Moon")
        )
        unreal.log_warning(f"Added moon: {moon_node}")

    except Exception as e:
        unreal.log_warning(f"Error adding components: {e}")
        # Alternative approach - try using BlueprintEditorLibrary
        try:
            unreal.log_warning("\nTrying alternative approach...")
            # Get the default object and add components there
            gen_class = bp.generated_class()
            if gen_class:
                cdo = gen_class.get_default_object()
                unreal.log_warning(f"CDO: {cdo}")
                # List existing components
                if hasattr(cdo, 'get_components_by_class'):
                    comps = cdo.get_components_by_class(unreal.ActorComponent)
                    unreal.log_warning(f"CDO components: {len(comps)}")
                    for c in comps:
                        unreal.log_warning(f"  - {c.get_name()}: {c.get_class().get_name()}")
        except Exception as e2:
            unreal.log_warning(f"Alternative error: {e2}")

    # Compile and save
    unreal.log_warning("\nCompiling...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("Compiled")
    except Exception as e:
        unreal.log_warning(f"Compile error: {e}")

    unreal.log_warning("\nSaving...")
    try:
        unreal.EditorAssetLibrary.save_asset(BP_PATH)
        unreal.log_warning("Saved")
    except Exception as e:
        unreal.log_warning(f"Save error: {e}")

    unreal.log_warning("=" * 70)

main()
