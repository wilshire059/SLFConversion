"""
Fix B_SkyManager level instance - scale, mesh, material, and light priorities
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_level_fix.txt"

def log(msg):
    unreal.log_warning(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def main():
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("=" * 70)
    log("COMPLETE SKYMANAGER LEVEL INSTANCE FIX")
    log("=" * 70)

    # Load level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
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
    fixed = False

    # Find SkyManager
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "SkyManager" in class_name:
            log(f"\nFound: {actor.get_name()} ({class_name})")

            all_comps = actor.get_components_by_class(unreal.ActorComponent)

            for comp in all_comps:
                comp_name = comp.get_name()

                # Fix SM_SkySphere scale
                if "SkySphere" in comp_name and isinstance(comp, unreal.StaticMeshComponent):
                    log(f"\n  Fixing SM_SkySphere:")

                    # Check current scale
                    try:
                        current_scale = comp.get_editor_property('relative_scale3d')
                        log(f"    Current scale: ({current_scale.x}, {current_scale.y}, {current_scale.z})")
                    except:
                        pass

                    # Set scale to 100x
                    try:
                        new_scale = unreal.Vector(100.0, 100.0, 100.0)
                        comp.set_editor_property('relative_scale3d', new_scale)
                        log(f"    Set scale to: (100, 100, 100)")
                    except Exception as e:
                        log(f"    Scale error: {e}")

                    # Verify mesh is set
                    try:
                        mesh = comp.get_editor_property('static_mesh')
                        if mesh:
                            log(f"    Mesh: {mesh.get_name()}")
                        else:
                            log(f"    Mesh: None - setting it now...")
                            sky_mesh = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Blueprints/Sky/SM_SkySphere")
                            if sky_mesh:
                                comp.set_editor_property('static_mesh', sky_mesh)
                                log(f"    Set mesh to: SM_SkySphere")
                    except Exception as e:
                        log(f"    Mesh error: {e}")

                    # Verify material is set
                    try:
                        num_mats = comp.get_num_materials()
                        if num_mats > 0:
                            mat = comp.get_material(0)
                            log(f"    Material[0]: {mat.get_name() if mat else 'None'}")
                        else:
                            log(f"    No materials - setting it now...")
                            sky_mat = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Materials/M_SkySphere")
                            if sky_mat:
                                comp.set_material(0, sky_mat)
                                log(f"    Set material to: M_SkySphere")
                    except Exception as e:
                        log(f"    Material error: {e}")

                    fixed = True

                # Fix directional light priorities
                if isinstance(comp, unreal.DirectionalLightComponent):
                    log(f"\n  Fixing {comp_name}:")

                    try:
                        current_priority = comp.get_editor_property('forward_shading_priority')
                        log(f"    Current ForwardShadingPriority: {current_priority}")
                    except:
                        pass

                    if "Sun" in comp_name:
                        # Sun should have priority 0 (highest)
                        try:
                            comp.set_editor_property('forward_shading_priority', 0)
                            log(f"    Set ForwardShadingPriority to: 0 (highest)")
                        except Exception as e:
                            log(f"    Priority error: {e}")

                    elif "Moon" in comp_name:
                        # Moon should have priority 1 (lower than sun)
                        try:
                            comp.set_editor_property('forward_shading_priority', 1)
                            log(f"    Set ForwardShadingPriority to: 1 (lower than sun)")
                        except Exception as e:
                            log(f"    Priority error: {e}")

                    fixed = True

    if fixed:
        # Save the level
        log("\n" + "=" * 70)
        log("SAVING LEVEL")
        log("=" * 70)
        try:
            unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
            log("Level saved!")
        except Exception as e:
            log(f"Save error: {e}")

        # Verify fixes
        log("\n" + "=" * 70)
        log("VERIFYING FIXES")
        log("=" * 70)

        for actor in all_actors:
            class_name = actor.get_class().get_name()
            if "SkyManager" in class_name:
                all_comps = actor.get_components_by_class(unreal.ActorComponent)
                for comp in all_comps:
                    comp_name = comp.get_name()

                    if "SkySphere" in comp_name and isinstance(comp, unreal.StaticMeshComponent):
                        try:
                            scale = comp.get_editor_property('relative_scale3d')
                            log(f"\n  SM_SkySphere final scale: ({scale.x}, {scale.y}, {scale.z})")
                        except:
                            pass

                    if isinstance(comp, unreal.DirectionalLightComponent):
                        try:
                            priority = comp.get_editor_property('forward_shading_priority')
                            log(f"  {comp_name} ForwardShadingPriority: {priority}")
                        except:
                            pass
    else:
        log("\nNo SkyManager found to fix")

    log("\n" + "=" * 70)
    log("FIX COMPLETE")
    log("=" * 70)
    log(f"\nResults: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
