"""
Check sky sphere transform and light priority settings
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skysphere_transform.txt"

def log(msg):
    unreal.log_warning(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def check_level(level_path, label):
    log(f"\n{'='*70}")
    log(f"{label}")
    log(f"{'='*70}")

    # Load level
    log(f"\nLoading level: {level_path}")
    try:
        loaded_world = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
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
            log(f"\nFound: {actor.get_name()}")
            log(f"  Actor Location: {actor.get_actor_location()}")
            log(f"  Actor Scale: {actor.get_actor_scale3d()}")

            # Check all components
            all_comps = actor.get_components_by_class(unreal.ActorComponent)

            for comp in all_comps:
                comp_name = comp.get_name()

                # Sky sphere details
                if "SkySphere" in comp_name and isinstance(comp, unreal.StaticMeshComponent):
                    log(f"\n  SM_SkySphere Component:")
                    try:
                        rel_loc = comp.get_editor_property('relative_location')
                        log(f"    RelativeLocation: {rel_loc}")
                    except Exception as e:
                        log(f"    RelativeLocation error: {e}")

                    try:
                        rel_rot = comp.get_editor_property('relative_rotation')
                        log(f"    RelativeRotation: {rel_rot}")
                    except Exception as e:
                        log(f"    RelativeRotation error: {e}")

                    try:
                        rel_scale = comp.get_editor_property('relative_scale3d')
                        log(f"    RelativeScale3D: {rel_scale}")
                    except Exception as e:
                        log(f"    RelativeScale3D error: {e}")

                    try:
                        world_loc = comp.get_world_location()
                        log(f"    WorldLocation: {world_loc}")
                    except:
                        pass

                    try:
                        world_scale = comp.get_world_scale()
                        log(f"    WorldScale: {world_scale}")
                    except:
                        pass

                    try:
                        bounds = comp.get_local_bounds()
                        log(f"    LocalBounds: {bounds}")
                    except:
                        pass

                # Directional light details
                if isinstance(comp, unreal.DirectionalLightComponent):
                    log(f"\n  {comp_name}:")
                    log(f"    Intensity: {comp.intensity}")

                    try:
                        is_atmo = comp.get_editor_property('atmosphere_sun_light')
                        log(f"    AtmosphereSunLight: {is_atmo}")
                    except:
                        pass

                    # Check forward shading priority
                    try:
                        priority = comp.get_editor_property('forward_shading_priority')
                        log(f"    ForwardShadingPriority: {priority}")
                    except Exception as e:
                        log(f"    ForwardShadingPriority: (error: {e})")

                    # Check if it affects world
                    try:
                        affects_world = comp.get_editor_property('affects_world')
                        log(f"    AffectsWorld: {affects_world}")
                    except:
                        pass

                    # Check visibility
                    try:
                        visible = comp.is_visible()
                        log(f"    Visible: {visible}")
                    except:
                        pass

                    # Check cast shadows
                    try:
                        cast_shadows = comp.get_editor_property('cast_shadows')
                        log(f"    CastShadows: {cast_shadows}")
                    except:
                        pass

def main():
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("SKYSPHERE TRANSFORM AND LIGHT PRIORITY CHECK")
    log("=" * 70)

    check_level("/Game/SoulslikeFramework/Maps/L_Demo_Showcase", "CURRENT PROJECT")

    log("\n\n" + "=" * 70)
    log("CHECK COMPLETE")
    log("=" * 70)
    log(f"\nResults: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
