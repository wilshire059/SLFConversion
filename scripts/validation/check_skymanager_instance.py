"""
Check B_SkyManager instance in level for mesh/material data
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_instance.txt"

def log(msg):
    unreal.log_warning(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def main():
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("=" * 70)
    log("SKYMANAGER INSTANCE CHECK")
    log("=" * 70)

    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    # Load level
    log(f"\nLoading level: {level_path}")
    try:
        loaded_world = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
        log(f"Loaded: {loaded_world}")
    except Exception as e:
        log(f"Load error: {e}")
        return

    # Get world
    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_subsystem.get_editor_world() if editor_subsystem else None

    if not world:
        log("ERROR: Could not get world")
        return

    # Find all actors
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)

    # Find SkyManager
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "SkyManager" in class_name:
            log(f"\n{'='*60}")
            log(f"Found: {actor.get_name()} ({class_name})")
            log(f"{'='*60}")

            # Get ALL components with details
            all_comps = actor.get_components_by_class(unreal.ActorComponent)
            log(f"Total Components: {len(all_comps)}")

            for comp in all_comps:
                comp_name = comp.get_name()
                comp_class = comp.get_class().get_name()
                log(f"\n  {comp_name} ({comp_class})")

                # Static mesh details
                if isinstance(comp, unreal.StaticMeshComponent):
                    try:
                        mesh = comp.get_editor_property('static_mesh')
                        if mesh:
                            log(f"    StaticMesh: {mesh.get_path_name()}")
                        else:
                            log(f"    StaticMesh: None")
                    except Exception as e:
                        log(f"    StaticMesh error: {e}")

                    try:
                        num_materials = comp.get_num_materials()
                        log(f"    Materials: {num_materials}")
                        for i in range(num_materials):
                            mat = comp.get_material(i)
                            if mat:
                                log(f"      [{i}]: {mat.get_path_name()}")
                            else:
                                log(f"      [{i}]: None")
                    except Exception as e:
                        log(f"    Materials error: {e}")

                    try:
                        vis = comp.is_visible()
                        log(f"    Visible: {vis}")
                    except:
                        pass

                # Directional light details
                if isinstance(comp, unreal.DirectionalLightComponent):
                    log(f"    Intensity: {comp.intensity}")
                    try:
                        is_atmo = comp.get_editor_property('atmosphere_sun_light')
                        log(f"    AtmosphereSunLight: {is_atmo}")
                    except:
                        pass

                # Sky light details
                if isinstance(comp, unreal.SkyLightComponent):
                    log(f"    Intensity: {comp.intensity}")
                    try:
                        source_type = comp.source_type
                        log(f"    SourceType: {source_type}")
                    except:
                        pass
                    try:
                        vis = comp.is_visible()
                        log(f"    Visible: {vis}")
                    except:
                        pass

                # Sky atmosphere
                if isinstance(comp, unreal.SkyAtmosphereComponent):
                    try:
                        vis = comp.is_visible()
                        log(f"    Visible: {vis}")
                    except:
                        pass

    log("\n" + "=" * 70)
    log("CHECK COMPLETE")
    log("=" * 70)
    log(f"\nResults: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
