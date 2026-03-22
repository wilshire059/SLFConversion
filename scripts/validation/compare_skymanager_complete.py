"""
COMPLETE comparison of B_SkyManager between bp_only and SLFConversion
Checks ALL properties, components, settings, and time-related values
"""
import unreal
import json

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_complete_comparison.txt"

def log(msg):
    unreal.log_warning(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def get_all_properties(obj, prefix=""):
    """Get all properties from an object"""
    props = {}
    try:
        # Get the class
        obj_class = obj.get_class()

        # Try to iterate through properties
        for prop in dir(obj):
            if prop.startswith('_'):
                continue
            try:
                val = getattr(obj, prop)
                if not callable(val):
                    props[prop] = str(val)
            except:
                pass
    except:
        pass
    return props

def check_actor_instance(world, label):
    """Check the SkyManager actor instance in a level"""
    log(f"\n{'='*70}")
    log(f"{label}")
    log(f"{'='*70}")

    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)

    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "SkyManager" not in class_name:
            continue

        log(f"\nActor: {actor.get_name()} ({class_name})")
        log(f"Location: {actor.get_actor_location()}")

        # Get all actor properties we can access
        log(f"\n--- ACTOR PROPERTIES ---")
        try:
            # Time-related properties (critical for day/night)
            time_props = [
                'time', 'Time', 'current_time_of_day', 'CurrentTimeOfDay',
                'sunrise_time', 'SunriseTime', 'sunset_time', 'SunsetTime',
                'day_length_minutes', 'DayLengthMinutes', 'time_dilation', 'TimeDilation',
                'time_paused', 'bTimePaused', 'is_time_paused'
            ]
            for prop in time_props:
                try:
                    val = actor.get_editor_property(prop)
                    log(f"  {prop}: {val}")
                except:
                    pass

            # Weather properties
            weather_props = [
                'current_weather', 'CurrentWeather', 'weather_intensity', 'WeatherIntensity'
            ]
            for prop in weather_props:
                try:
                    val = actor.get_editor_property(prop)
                    log(f"  {prop}: {val}")
                except:
                    pass

        except Exception as e:
            log(f"  Properties error: {e}")

        # Check all components
        all_comps = actor.get_components_by_class(unreal.ActorComponent)
        log(f"\n--- COMPONENTS ({len(all_comps)}) ---")

        for comp in all_comps:
            comp_name = comp.get_name()
            comp_class = comp.get_class().get_name()
            log(f"\n  [{comp_name}] ({comp_class})")

            # Scene component properties
            if isinstance(comp, unreal.SceneComponent):
                try:
                    loc = comp.get_editor_property('relative_location')
                    log(f"    RelativeLocation: ({loc.x}, {loc.y}, {loc.z})")
                except:
                    pass
                try:
                    rot = comp.get_editor_property('relative_rotation')
                    log(f"    RelativeRotation: (P={rot.pitch}, Y={rot.yaw}, R={rot.roll})")
                except:
                    pass
                try:
                    scale = comp.get_editor_property('relative_scale3d')
                    log(f"    RelativeScale3D: ({scale.x}, {scale.y}, {scale.z})")
                except:
                    pass

            # Static mesh component
            if isinstance(comp, unreal.StaticMeshComponent):
                try:
                    mesh = comp.get_editor_property('static_mesh')
                    log(f"    StaticMesh: {mesh.get_path_name() if mesh else 'None'}")
                except:
                    pass
                try:
                    num_mats = comp.get_num_materials()
                    log(f"    Materials: {num_mats}")
                    for i in range(num_mats):
                        mat = comp.get_material(i)
                        log(f"      [{i}]: {mat.get_path_name() if mat else 'None'}")
                except:
                    pass
                try:
                    vis = comp.is_visible()
                    log(f"    Visible: {vis}")
                except:
                    pass
                try:
                    cast_shadow = comp.get_editor_property('cast_shadow')
                    log(f"    CastShadow: {cast_shadow}")
                except:
                    pass

            # Directional light component - ALL properties
            if isinstance(comp, unreal.DirectionalLightComponent):
                light_props = [
                    ('intensity', 'Intensity'),
                    ('light_color', 'LightColor'),
                    ('temperature', 'Temperature'),
                    ('use_temperature', 'UseTemperature'),
                    ('atmosphere_sun_light', 'AtmosphereSunLight'),
                    ('atmosphere_sun_light_index', 'AtmosphereSunLightIndex'),
                    ('forward_shading_priority', 'ForwardShadingPriority'),
                    ('affects_world', 'AffectsWorld'),
                    ('cast_shadows', 'CastShadows'),
                    ('cast_static_shadows', 'CastStaticShadows'),
                    ('cast_dynamic_shadows', 'CastDynamicShadows'),
                    ('indirect_lighting_intensity', 'IndirectLightingIntensity'),
                    ('volumetric_scattering_intensity', 'VolumetricScatteringIntensity'),
                    ('light_source_angle', 'LightSourceAngle'),
                    ('light_source_soft_angle', 'LightSourceSoftAngle'),
                    ('shadow_amount', 'ShadowAmount'),
                    ('mobility', 'Mobility'),
                ]
                for prop_snake, prop_camel in light_props:
                    try:
                        val = comp.get_editor_property(prop_snake)
                        log(f"    {prop_camel}: {val}")
                    except:
                        pass

            # Sky light component - ALL properties
            if isinstance(comp, unreal.SkyLightComponent):
                sky_props = [
                    ('intensity', 'Intensity'),
                    ('source_type', 'SourceType'),
                    ('cubemap', 'Cubemap'),
                    ('lower_hemisphere_color', 'LowerHemisphereColor'),
                    ('sky_distance_threshold', 'SkyDistanceThreshold'),
                    ('capture_emissive_only', 'CaptureEmissiveOnly'),
                    ('indirect_lighting_intensity', 'IndirectLightingIntensity'),
                    ('volumetric_scattering_intensity', 'VolumetricScatteringIntensity'),
                    ('occlusion_max_distance', 'OcclusionMaxDistance'),
                    ('contrast', 'Contrast'),
                    ('occlusion_exponent', 'OcclusionExponent'),
                    ('min_occlusion', 'MinOcclusion'),
                    ('occlusion_tint', 'OcclusionTint'),
                    ('mobility', 'Mobility'),
                    ('real_time_capture', 'RealTimeCapture'),
                ]
                for prop_snake, prop_camel in sky_props:
                    try:
                        val = comp.get_editor_property(prop_snake)
                        log(f"    {prop_camel}: {val}")
                    except:
                        pass

            # Sky atmosphere component - ALL properties
            if isinstance(comp, unreal.SkyAtmosphereComponent):
                atmo_props = [
                    ('transform_mode', 'TransformMode'),
                    ('bottom_radius', 'BottomRadius'),
                    ('ground_albedo', 'GroundAlbedo'),
                    ('atmosphere_height', 'AtmosphereHeight'),
                    ('multi_scattering_factor', 'MultiScatteringFactor'),
                    ('rayleigh_scattering_scale', 'RayleighScatteringScale'),
                    ('rayleigh_scattering', 'RayleighScattering'),
                    ('rayleigh_exponential_distribution', 'RayleighExponentialDistribution'),
                    ('mie_scattering_scale', 'MieScatteringScale'),
                    ('mie_scattering', 'MieScattering'),
                    ('mie_absorption_scale', 'MieAbsorptionScale'),
                    ('mie_absorption', 'MieAbsorption'),
                    ('mie_anisotropy', 'MieAnisotropy'),
                    ('mie_exponential_distribution', 'MieExponentialDistribution'),
                    ('other_absorption_scale', 'OtherAbsorptionScale'),
                    ('other_absorption', 'OtherAbsorption'),
                    ('sky_luminance_factor', 'SkyLuminanceFactor'),
                    ('aerial_perspective_view_distance_scale', 'AerialPerspectiveViewDistanceScale'),
                    ('height_fog_contribution', 'HeightFogContribution'),
                    ('transmittance_min_light_elevation_angle', 'TransmittanceMinLightElevationAngle'),
                    ('aerial_perspective_start_depth', 'AerialPerspectiveStartDepth'),
                ]
                for prop_snake, prop_camel in atmo_props:
                    try:
                        val = comp.get_editor_property(prop_snake)
                        log(f"    {prop_camel}: {val}")
                    except:
                        pass

            # Exponential height fog - ALL properties
            if isinstance(comp, unreal.ExponentialHeightFogComponent):
                fog_props = [
                    ('fog_density', 'FogDensity'),
                    ('fog_height_falloff', 'FogHeightFalloff'),
                    ('second_fog_density', 'SecondFogDensity'),
                    ('second_fog_height_falloff', 'SecondFogHeightFalloff'),
                    ('second_fog_height_offset', 'SecondFogHeightOffset'),
                    ('fog_inscattering_color', 'FogInscatteringColor'),
                    ('fog_max_opacity', 'FogMaxOpacity'),
                    ('start_distance', 'StartDistance'),
                    ('fog_cutoff_distance', 'FogCutoffDistance'),
                    ('volumetric_fog', 'VolumetricFog'),
                    ('volumetric_fog_scattering_distribution', 'VolumetricFogScatteringDistribution'),
                    ('volumetric_fog_albedo', 'VolumetricFogAlbedo'),
                    ('volumetric_fog_emissive', 'VolumetricFogEmissive'),
                    ('volumetric_fog_extinction_scale', 'VolumetricFogExtinctionScale'),
                    ('volumetric_fog_distance', 'VolumetricFogDistance'),
                    ('volumetric_fog_start_distance', 'VolumetricFogStartDistance'),
                    ('volumetric_fog_near_fade_in_distance', 'VolumetricFogNearFadeInDistance'),
                ]
                for prop_snake, prop_camel in fog_props:
                    try:
                        val = comp.get_editor_property(prop_snake)
                        log(f"    {prop_camel}: {val}")
                    except:
                        pass

            # Volumetric cloud - ALL properties
            if isinstance(comp, unreal.VolumetricCloudComponent):
                cloud_props = [
                    ('layer_bottom_altitude', 'LayerBottomAltitude'),
                    ('layer_height', 'LayerHeight'),
                    ('tracing_start_max_distance', 'TracingStartMaxDistance'),
                    ('tracing_max_distance', 'TracingMaxDistance'),
                    ('planet_radius', 'PlanetRadius'),
                    ('ground_albedo', 'GroundAlbedo'),
                    ('material', 'Material'),
                    ('bUsePerSampleAtmosphericLightTransmittance', 'UsePerSampleAtmosphericLightTransmittance'),
                    ('sky_light_cloud_bottom_occlusion', 'SkyLightCloudBottomOcclusion'),
                    ('view_sample_count_scale', 'ViewSampleCountScale'),
                    ('reflection_view_sample_count_scale', 'ReflectionViewSampleCountScale'),
                    ('shadow_view_sample_count_scale', 'ShadowViewSampleCountScale'),
                    ('shadow_reflection_view_sample_count_scale', 'ShadowReflectionViewSampleCountScale'),
                    ('shadow_tracing_distance', 'ShadowTracingDistance'),
                    ('stop_tracing_transmittance_threshold', 'StopTracingTransmittanceThreshold'),
                ]
                for prop_snake, prop_camel in cloud_props:
                    try:
                        val = comp.get_editor_property(prop_snake)
                        log(f"    {prop_camel}: {val}")
                    except:
                        pass

def main():
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("COMPLETE SKYMANAGER COMPARISON")
    log("bp_only vs SLFConversion")
    log("=" * 70)

    # Load SLFConversion level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    log(f"\nLoading level: {level_path}")
    try:
        loaded_world = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
    except Exception as e:
        log(f"Load error: {e}")
        return

    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_subsystem.get_editor_world() if editor_subsystem else None

    if world:
        check_actor_instance(world, "CURRENT PROJECT (SLFConversion)")
    else:
        log("ERROR: Could not get world")

    log("\n\n" + "=" * 70)
    log("COMPARISON COMPLETE")
    log("=" * 70)
    log(f"\nResults: {OUTPUT_FILE}")
    log("\nNOTE: Run this same script on bp_only project to get comparison data")

if __name__ == "__main__":
    main()
