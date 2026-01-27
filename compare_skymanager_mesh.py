"""
Compare B_SkyManager sky sphere mesh between SLFConversion and bp_only
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_mesh_compare.txt"

def log(msg):
    unreal.log_warning(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def check_skymanager_components(bp_path, label):
    log(f"\n{'='*60}")
    log(f"{label}")
    log(f"{'='*60}")

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load {bp_path}")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        log("ERROR: No generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log("ERROR: No CDO")
        return

    log(f"Blueprint: {bp_path}")
    log(f"Class: {gen_class.get_name()}")

    # Get all components
    all_comps = cdo.get_components_by_class(unreal.ActorComponent)
    log(f"\nTotal Components: {len(all_comps)}")

    for comp in all_comps:
        comp_name = comp.get_name()
        comp_class = comp.get_class().get_name()
        log(f"\n  {comp_name} ({comp_class})")

        # Check static mesh components for mesh/material
        if isinstance(comp, unreal.StaticMeshComponent):
            try:
                mesh = comp.get_editor_property('static_mesh')
                log(f"    StaticMesh: {mesh.get_name() if mesh else 'None'}")
            except Exception as e:
                log(f"    StaticMesh: ERROR - {e}")

            try:
                num_materials = comp.get_num_materials()
                log(f"    Materials: {num_materials}")
                for i in range(num_materials):
                    mat = comp.get_material(i)
                    log(f"      [{i}]: {mat.get_name() if mat else 'None'}")
            except Exception as e:
                log(f"    Materials: ERROR - {e}")

        # Check directional light components
        if isinstance(comp, unreal.DirectionalLightComponent):
            try:
                intensity = comp.intensity
                log(f"    Intensity: {intensity}")
            except:
                pass
            try:
                is_atmo = comp.get_editor_property('atmosphere_sun_light')
                log(f"    AtmosphereSunLight: {is_atmo}")
            except:
                try:
                    is_atmo = comp.get_editor_property('bAtmosphereSunLight')
                    log(f"    AtmosphereSunLight: {is_atmo}")
                except:
                    pass
            try:
                color = comp.light_color
                log(f"    LightColor: R={color.r}, G={color.g}, B={color.b}")
            except:
                pass

        # Check sky light components
        if isinstance(comp, unreal.SkyLightComponent):
            try:
                intensity = comp.intensity
                log(f"    Intensity: {intensity}")
            except:
                pass
            try:
                source_type = comp.source_type
                log(f"    SourceType: {source_type}")
            except:
                pass

        # Check sky atmosphere components
        if isinstance(comp, unreal.SkyAtmosphereComponent):
            log(f"    (SkyAtmosphereComponent present)")

        # Check exponential height fog
        if isinstance(comp, unreal.ExponentialHeightFogComponent):
            try:
                fog_density = comp.fog_density
                log(f"    FogDensity: {fog_density}")
            except:
                pass

def main():
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("SKYMANAGER MESH COMPARISON")
    log("=" * 60)

    # Check SLFConversion version
    check_skymanager_components(
        "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager",
        "SLFCONVERSION VERSION"
    )

    log("\n\n")
    log("=" * 60)
    log("COMPARISON COMPLETE")
    log("=" * 60)
    log(f"\nResults: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
