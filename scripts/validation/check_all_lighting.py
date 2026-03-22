"""
Comprehensive check of ALL lighting actors and components in L_Demo_Showcase
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/all_lighting_check.txt"

def log(msg):
    unreal.log_warning(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def main():
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("=" * 70)
    log("COMPREHENSIVE LIGHTING CHECK - L_Demo_Showcase")
    log("=" * 70)

    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    # Load level
    log(f"\nLoading level: {level_path}")
    try:
        loaded_world = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
        log(f"Loaded: {loaded_world}")
    except Exception as e:
        log(f"Load error: {e}")

    # Get world
    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_subsystem.get_editor_world() if editor_subsystem else None

    if not world:
        log("ERROR: Could not get world")
        return

    log(f"World: {world.get_name()}")

    # Find all actors
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
    log(f"Total actors: {len(all_actors)}")

    # ============ DIRECTIONAL LIGHTS ============
    log("\n" + "=" * 70)
    log("1. DIRECTIONAL LIGHT ACTORS (standalone)")
    log("=" * 70)

    dir_light_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.DirectionalLight)
    log(f"Found {len(dir_light_actors)} DirectionalLight actors")
    for actor in dir_light_actors:
        log(f"\n  Actor: {actor.get_name()}")
        log(f"    Class: {actor.get_class().get_name()}")
        log(f"    Location: {actor.get_actor_location()}")
        try:
            light_comp = actor.get_component_by_class(unreal.DirectionalLightComponent)
            if light_comp:
                log(f"    Intensity: {light_comp.intensity}")
                log(f"    Mobility: {light_comp.mobility}")
                try:
                    is_atmo = light_comp.get_editor_property('atmosphere_sun_light')
                    log(f"    AtmosphereSunLight: {is_atmo}")
                except:
                    try:
                        is_atmo = light_comp.get_editor_property('bAtmosphereSunLight')
                        log(f"    AtmosphereSunLight: {is_atmo}")
                    except:
                        log(f"    AtmosphereSunLight: (could not read)")
        except Exception as e:
            log(f"    Error: {e}")

    # ============ SKY MANAGER ACTORS ============
    log("\n" + "=" * 70)
    log("2. SKY MANAGER ACTORS")
    log("=" * 70)

    sky_managers = []
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "SkyManager" in class_name:
            sky_managers.append(actor)

    log(f"Found {len(sky_managers)} SkyManager actors")
    for actor in sky_managers:
        log(f"\n  Actor: {actor.get_name()}")
        log(f"    Class: {actor.get_class().get_name()}")
        log(f"    Location: {actor.get_actor_location()}")

        # Get ALL components
        try:
            all_comps = actor.get_components_by_class(unreal.ActorComponent)
            log(f"    Total Components: {len(all_comps)}")
            for comp in all_comps:
                comp_class = comp.get_class().get_name()
                log(f"      - {comp.get_name()} ({comp_class})")

                # Check directional light components specifically
                if isinstance(comp, unreal.DirectionalLightComponent):
                    log(f"          Intensity: {comp.intensity}")
                    try:
                        is_atmo = comp.get_editor_property('atmosphere_sun_light')
                        log(f"          AtmosphereSunLight: {is_atmo}")
                    except:
                        try:
                            is_atmo = comp.get_editor_property('bAtmosphereSunLight')
                            log(f"          AtmosphereSunLight: {is_atmo}")
                        except:
                            log(f"          AtmosphereSunLight: (could not read)")
        except Exception as e:
            log(f"    Components error: {e}")

    # ============ SKY ATMOSPHERE ACTORS ============
    log("\n" + "=" * 70)
    log("3. SKY ATMOSPHERE ACTORS (standalone)")
    log("=" * 70)

    sky_atmo_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.SkyAtmosphere)
    log(f"Found {len(sky_atmo_actors)} SkyAtmosphere actors")
    for actor in sky_atmo_actors:
        log(f"\n  Actor: {actor.get_name()}")
        log(f"    Class: {actor.get_class().get_name()}")
        log(f"    Location: {actor.get_actor_location()}")

    # ============ SKY LIGHT ACTORS ============
    log("\n" + "=" * 70)
    log("4. SKY LIGHT ACTORS (standalone)")
    log("=" * 70)

    sky_light_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.SkyLight)
    log(f"Found {len(sky_light_actors)} SkyLight actors")
    for actor in sky_light_actors:
        log(f"\n  Actor: {actor.get_name()}")
        log(f"    Class: {actor.get_class().get_name()}")
        log(f"    Location: {actor.get_actor_location()}")

    # ============ EXPONENTIAL HEIGHT FOG ============
    log("\n" + "=" * 70)
    log("5. EXPONENTIAL HEIGHT FOG ACTORS (standalone)")
    log("=" * 70)

    fog_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.ExponentialHeightFog)
    log(f"Found {len(fog_actors)} ExponentialHeightFog actors")
    for actor in fog_actors:
        log(f"\n  Actor: {actor.get_name()}")
        log(f"    Class: {actor.get_class().get_name()}")
        log(f"    Location: {actor.get_actor_location()}")

    # ============ VOLUMETRIC CLOUD ============
    log("\n" + "=" * 70)
    log("6. VOLUMETRIC CLOUD ACTORS (standalone)")
    log("=" * 70)

    cloud_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.VolumetricCloud)
    log(f"Found {len(cloud_actors)} VolumetricCloud actors")
    for actor in cloud_actors:
        log(f"\n  Actor: {actor.get_name()}")
        log(f"    Class: {actor.get_class().get_name()}")
        log(f"    Location: {actor.get_actor_location()}")

    # ============ ALL DIRECTIONAL LIGHT COMPONENTS ============
    log("\n" + "=" * 70)
    log("7. ALL DIRECTIONAL LIGHT COMPONENTS (on any actor)")
    log("=" * 70)

    total_dir_lights = 0
    atmo_lights = []
    for actor in all_actors:
        try:
            dir_comps = actor.get_components_by_class(unreal.DirectionalLightComponent)
            for comp in dir_comps:
                total_dir_lights += 1
                owner = actor.get_name()
                comp_name = comp.get_name()
                intensity = comp.intensity

                is_atmo = False
                try:
                    is_atmo = comp.get_editor_property('atmosphere_sun_light')
                except:
                    try:
                        is_atmo = comp.get_editor_property('bAtmosphereSunLight')
                    except:
                        pass

                log(f"  {owner}.{comp_name}: Intensity={intensity}, AtmosphereSunLight={is_atmo}")

                if is_atmo:
                    atmo_lights.append(f"{owner}.{comp_name}")
        except:
            pass

    log(f"\nTotal DirectionalLightComponents: {total_dir_lights}")
    log(f"Components with AtmosphereSunLight=True: {len(atmo_lights)}")
    for light in atmo_lights:
        log(f"  - {light}")

    # ============ ALL SKY ATMOSPHERE COMPONENTS ============
    log("\n" + "=" * 70)
    log("8. ALL SKY ATMOSPHERE COMPONENTS (on any actor)")
    log("=" * 70)

    total_sky_atmo = 0
    for actor in all_actors:
        try:
            sky_comps = actor.get_components_by_class(unreal.SkyAtmosphereComponent)
            for comp in sky_comps:
                total_sky_atmo += 1
                owner = actor.get_name()
                comp_name = comp.get_name()
                log(f"  {owner}.{comp_name}")
        except:
            pass

    log(f"\nTotal SkyAtmosphereComponents: {total_sky_atmo}")

    # ============ SUMMARY ============
    log("\n" + "=" * 70)
    log("SUMMARY")
    log("=" * 70)
    log(f"DirectionalLight Actors: {len(dir_light_actors)}")
    log(f"SkyManager Actors: {len(sky_managers)}")
    log(f"SkyAtmosphere Actors: {len(sky_atmo_actors)}")
    log(f"SkyLight Actors: {len(sky_light_actors)}")
    log(f"ExponentialHeightFog Actors: {len(fog_actors)}")
    log(f"VolumetricCloud Actors: {len(cloud_actors)}")
    log(f"Total DirectionalLightComponents: {total_dir_lights}")
    log(f"Lights with AtmosphereSunLight=True: {len(atmo_lights)}")

    if len(atmo_lights) > 1:
        log("\n*** WARNING: Multiple lights have AtmosphereSunLight=True! ***")
        log("This causes 'Multiple directional lights are competing' error!")
        for light in atmo_lights:
            log(f"  - {light}")

    log("\n" + "=" * 70)
    log("CHECK COMPLETE")
    log("=" * 70)
    log(f"\nResults: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
