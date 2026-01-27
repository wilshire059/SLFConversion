"""
Fix ALL SkyManager settings to match bp_only exactly
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_all_fixes.txt"

def log(msg):
    unreal.log_warning(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def main():
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("=" * 70)
    log("FIXING ALL SKYMANAGER SETTINGS TO MATCH BP_ONLY")
    log("=" * 70)

    # Load level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    log(f"\nLoading level: {level_path}")
    try:
        loaded_world = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
    except Exception as e:
        log(f"Load error: {e}")
        return

    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_subsystem.get_editor_world() if editor_subsystem else None

    if not world:
        log("ERROR: Could not get world")
        return

    # Load required assets
    log("\nLoading required assets...")
    cloud_mat = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Materials/MI_VolumetricCloud")
    if cloud_mat:
        log(f"  VolumetricCloud material: {cloud_mat}")
    else:
        log("  WARNING: Could not load MI_VolumetricCloud")

    # Find all actors
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)

    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "SkyManager" not in class_name:
            continue

        log(f"\nFound: {actor.get_name()}")

        # Fix actor properties
        log("\n--- FIXING ACTOR PROPERTIES ---")
        try:
            # TimeDilation should be 100.0 like bp_only
            actor.set_editor_property('time_dilation', 100.0)
            log("  TimeDilation: 100.0")
        except Exception as e:
            log(f"  TimeDilation error: {e}")

        all_comps = actor.get_components_by_class(unreal.ActorComponent)

        for comp in all_comps:
            comp_name = comp.get_name()

            # Fix DirectionalLight_Sun
            if "Sun" in comp_name and isinstance(comp, unreal.DirectionalLightComponent):
                log(f"\n--- FIXING {comp_name} ---")
                try:
                    # RelativeLocation from bp_only
                    comp.set_editor_property('relative_location', unreal.Vector(-1.8189894e-12, -50.0, 1.3313024e-07))
                    log("  RelativeLocation: (-1.8e-12, -50.0, 1.3e-07)")
                except Exception as e:
                    log(f"  RelativeLocation error: {e}")

                try:
                    # RelativeRotation from bp_only (time-based)
                    comp.set_editor_property('relative_rotation', unreal.Rotator(-44.64, -180.0, 180.0))
                    log("  RelativeRotation: (-44.64, -180.0, 180.0)")
                except Exception as e:
                    log(f"  RelativeRotation error: {e}")

                try:
                    comp.set_editor_property('intensity', 4.0)
                    log("  Intensity: 4.0")
                except Exception as e:
                    log(f"  Intensity error: {e}")

                try:
                    # White light color like bp_only
                    comp.set_editor_property('light_color', unreal.Color(r=255, g=255, b=255, a=255))
                    log("  LightColor: (255, 255, 255)")
                except Exception as e:
                    log(f"  LightColor error: {e}")

            # Fix DirectionalLight_Moon
            if "Moon" in comp_name and isinstance(comp, unreal.DirectionalLightComponent):
                log(f"\n--- FIXING {comp_name} ---")
                try:
                    comp.set_editor_property('relative_location', unreal.Vector(0.0, 50.0, 0.0))
                    log("  RelativeLocation: (0.0, 50.0, 0.0)")
                except Exception as e:
                    log(f"  RelativeLocation error: {e}")

                try:
                    # RelativeRotation from bp_only
                    comp.set_editor_property('relative_rotation', unreal.Rotator(44.64, 0.0, 0.0))
                    log("  RelativeRotation: (44.64, 0.0, 0.0)")
                except Exception as e:
                    log(f"  RelativeRotation error: {e}")

                try:
                    comp.set_editor_property('intensity', 1.0)
                    log("  Intensity: 1.0")
                except Exception as e:
                    log(f"  Intensity error: {e}")

                try:
                    # CRITICAL: Moon needs AtmosphereSunLight=True like bp_only
                    comp.set_editor_property('atmosphere_sun_light', True)
                    log("  AtmosphereSunLight: True")
                except Exception as e:
                    log(f"  AtmosphereSunLight error: {e}")

                try:
                    # Moon is index 1
                    comp.set_editor_property('atmosphere_sun_light_index', 1)
                    log("  AtmosphereSunLightIndex: 1")
                except Exception as e:
                    log(f"  AtmosphereSunLightIndex error: {e}")

                try:
                    comp.set_editor_property('forward_shading_priority', 1)
                    log("  ForwardShadingPriority: 1")
                except Exception as e:
                    log(f"  ForwardShadingPriority error: {e}")

            # Fix SkyLight
            if isinstance(comp, unreal.SkyLightComponent):
                log(f"\n--- FIXING {comp_name} ---")
                try:
                    comp.set_editor_property('relative_location', unreal.Vector(50.0, 0.0, 0.0))
                    log("  RelativeLocation: (50.0, 0.0, 0.0)")
                except Exception as e:
                    log(f"  RelativeLocation error: {e}")

                try:
                    # CRITICAL: RealTimeCapture must be True
                    comp.set_editor_property('real_time_capture', True)
                    log("  RealTimeCapture: True")
                except Exception as e:
                    log(f"  RealTimeCapture error: {e}")

            # Fix SkyAtmosphere
            if isinstance(comp, unreal.SkyAtmosphereComponent):
                log(f"\n--- FIXING {comp_name} ---")
                try:
                    # Warmer sky luminance factor from bp_only
                    sky_lum = unreal.LinearColor(r=1.0, g=0.950935, b=0.606228, a=1.0)
                    comp.set_editor_property('sky_luminance_factor', sky_lum)
                    log("  SkyLuminanceFactor: (1.0, 0.95, 0.61, 1.0)")
                except Exception as e:
                    log(f"  SkyLuminanceFactor error: {e}")

            # Fix VolumetricCloud
            if isinstance(comp, unreal.VolumetricCloudComponent):
                log(f"\n--- FIXING {comp_name} ---")
                try:
                    comp.set_editor_property('layer_bottom_altitude', 0.404084)
                    log("  LayerBottomAltitude: 0.404084")
                except Exception as e:
                    log(f"  LayerBottomAltitude error: {e}")

                try:
                    comp.set_editor_property('layer_height', 20.0)
                    log("  LayerHeight: 20.0")
                except Exception as e:
                    log(f"  LayerHeight error: {e}")

                if cloud_mat:
                    try:
                        comp.set_editor_property('material', cloud_mat)
                        log("  Material: MI_VolumetricCloud")
                    except Exception as e:
                        log(f"  Material error: {e}")

            # Fix ExponentialHeightFog
            if isinstance(comp, unreal.ExponentialHeightFogComponent):
                log(f"\n--- FIXING {comp_name} ---")
                try:
                    comp.set_editor_property('start_distance', 10000.0)
                    log("  StartDistance: 10000.0")
                except Exception as e:
                    log(f"  StartDistance error: {e}")

    # Save the level
    log("\n" + "=" * 70)
    log("SAVING LEVEL")
    log("=" * 70)
    try:
        unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
        log("Level saved!")
    except Exception as e:
        log(f"Save error: {e}")

    log("\n" + "=" * 70)
    log("ALL FIXES APPLIED")
    log("=" * 70)
    log(f"\nResults: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
