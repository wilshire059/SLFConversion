"""
Check for directional lights in L_Demo_Showcase level
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/level_lights_check.txt"

def log(msg):
    unreal.log_warning(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def main():
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("=" * 60)
    log("CHECKING DIRECTIONAL LIGHTS IN L_Demo_Showcase")
    log("=" * 60)

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

    # Find directional lights
    log("\n" + "-" * 40)
    log("DIRECTIONAL LIGHTS IN LEVEL:")
    log("-" * 40)

    directional_lights = []
    for actor in all_actors:
        class_name = actor.get_class().get_name()

        # Check for DirectionalLight actor
        if "DirectionalLight" in class_name:
            directional_lights.append(actor)
            log(f"\n  Actor: {actor.get_name()} ({class_name})")
            log(f"    Location: {actor.get_actor_location()}")

            # Try to get light component
            try:
                light_comp = actor.get_component_by_class(unreal.DirectionalLightComponent)
                if light_comp:
                    log(f"    Intensity: {light_comp.intensity}")
                    log(f"    Mobility: {light_comp.mobility}")
                    try:
                        is_atmo = light_comp.get_editor_property('bAtmosphereSunLight')
                        log(f"    AtmosphereSunLight: {is_atmo}")
                    except:
                        pass
            except Exception as e:
                log(f"    Light component error: {e}")

        # Also check SkyManager
        if "SkyManager" in class_name:
            log(f"\n  SkyManager Actor: {actor.get_name()} ({class_name})")
            log(f"    Location: {actor.get_actor_location()}")

            # Get its directional light components
            try:
                comps = actor.get_components_by_class(unreal.DirectionalLightComponent)
                log(f"    DirectionalLightComponents: {len(comps)}")
                for comp in comps:
                    log(f"      - {comp.get_name()}: Intensity={comp.intensity}")
                    try:
                        is_atmo = comp.get_editor_property('bAtmosphereSunLight')
                        log(f"        AtmosphereSunLight: {is_atmo}")
                    except:
                        pass
            except Exception as e:
                log(f"    Components error: {e}")

    # Also check for sky atmospheres
    log("\n" + "-" * 40)
    log("SKY ATMOSPHERES IN LEVEL:")
    log("-" * 40)

    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "SkyAtmosphere" in class_name:
            log(f"  {actor.get_name()} ({class_name})")

    log("\n" + "=" * 60)
    log("CHECK COMPLETE")
    log("=" * 60)
    log(f"\nResults: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
