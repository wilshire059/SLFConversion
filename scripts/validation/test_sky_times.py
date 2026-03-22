"""
Test sky rotation at different times to verify the formula matches bp_only
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/sky_time_test.txt"

def log(msg):
    unreal.log_warning(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def main():
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("=" * 70)
    log("SKY ROTATION TEST - VERIFYING FORMULA")
    log("=" * 70)
    log("")
    log("Formula: SunPitch = (Time / 24.0) * 360.0")
    log("         MoonPitch = SunPitch + 180.0")
    log("")

    # Test cases: time -> expected sun pitch
    test_times = [
        (0.0, "Midnight"),
        (6.0, "Sunrise"),
        (12.0, "Noon"),
        (14.976, "bp_only snapshot time"),
        (18.0, "Sunset"),
        (21.0, "Night"),
    ]

    log("Expected values (from formula):")
    log("-" * 50)
    for time, label in test_times:
        sun_pitch = (time / 24.0) * 360.0
        moon_pitch = sun_pitch + 180.0
        log(f"  {label} (Time={time:.2f}):")
        log(f"    Sun Pitch: {sun_pitch:.2f}°")
        log(f"    Moon Pitch: {moon_pitch:.2f}° (normalized: {moon_pitch % 360:.2f}°)")
        log("")

    # Load level and check actual instance
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

    # Find SkyManager
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)

    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "SkyManager" not in class_name:
            continue

        log(f"\nFound: {actor.get_name()}")

        # Get current time
        try:
            time_val = actor.get_editor_property('time')
            log(f"  Current Time: {time_val}")
        except Exception as e:
            log(f"  Time error: {e}")

        # Check light component settings
        all_comps = actor.get_components_by_class(unreal.ActorComponent)

        for comp in all_comps:
            comp_name = comp.get_name()

            if isinstance(comp, unreal.DirectionalLightComponent):
                log(f"\n  {comp_name}:")
                try:
                    rot = comp.get_editor_property('relative_rotation')
                    log(f"    RelativeRotation: (P={rot.pitch:.2f}, Y={rot.yaw:.2f}, R={rot.roll:.2f})")
                except Exception as e:
                    log(f"    RelativeRotation error: {e}")

                try:
                    atmo = comp.get_editor_property('atmosphere_sun_light')
                    idx = comp.get_editor_property('atmosphere_sun_light_index')
                    log(f"    AtmosphereSunLight: {atmo}, Index: {idx}")
                except Exception as e:
                    log(f"    AtmosphereSunLight error: {e}")

                try:
                    intensity = comp.get_editor_property('intensity')
                    log(f"    Intensity: {intensity}")
                except Exception as e:
                    log(f"    Intensity error: {e}")

            if isinstance(comp, unreal.SkyLightComponent):
                log(f"\n  {comp_name}:")
                try:
                    rtc = comp.get_editor_property('real_time_capture')
                    log(f"    RealTimeCapture: {rtc}")
                except Exception as e:
                    log(f"    RealTimeCapture error: {e}")

    log("\n" + "=" * 70)
    log("TEST COMPLETE")
    log("=" * 70)
    log(f"\nResults: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
