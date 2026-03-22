"""
Fix B_SkyManager instance - set the sky sphere mesh and material
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_mesh_fix.txt"

def log(msg):
    unreal.log_warning(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def main():
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("=" * 70)
    log("FIXING SKYMANAGER SKY SPHERE MESH")
    log("=" * 70)

    # Load the mesh and material
    sky_mesh_path = "/Game/SoulslikeFramework/Blueprints/Sky/SM_SkySphere"
    sky_mat_path = "/Game/SoulslikeFramework/Materials/M_SkySphere"

    log(f"\nLoading mesh: {sky_mesh_path}")
    sky_mesh = unreal.EditorAssetLibrary.load_asset(sky_mesh_path)
    if not sky_mesh:
        log(f"ERROR: Could not load mesh")
        return
    log(f"  Loaded: {sky_mesh}")

    log(f"\nLoading material: {sky_mat_path}")
    sky_mat = unreal.EditorAssetLibrary.load_asset(sky_mat_path)
    if not sky_mat:
        log(f"ERROR: Could not load material")
        return
    log(f"  Loaded: {sky_mat}")

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

            # Find SM_SkySphere component
            all_comps = actor.get_components_by_class(unreal.StaticMeshComponent)
            for comp in all_comps:
                if "SkySphere" in comp.get_name():
                    log(f"\n  Found SM_SkySphere component: {comp.get_name()}")

                    # Check current state
                    try:
                        current_mesh = comp.get_editor_property('static_mesh')
                        log(f"    Current mesh: {current_mesh}")
                    except:
                        log(f"    Current mesh: (could not read)")

                    # Set the mesh
                    log(f"\n  Setting static mesh...")
                    try:
                        comp.set_editor_property('static_mesh', sky_mesh)
                        log(f"    Set static_mesh to: {sky_mesh}")
                    except Exception as e:
                        log(f"    ERROR setting mesh: {e}")

                    # Set the material
                    log(f"\n  Setting material...")
                    try:
                        comp.set_material(0, sky_mat)
                        log(f"    Set material[0] to: {sky_mat}")
                    except Exception as e:
                        log(f"    ERROR setting material: {e}")

                    # Verify
                    log(f"\n  Verifying...")
                    try:
                        new_mesh = comp.get_editor_property('static_mesh')
                        log(f"    Mesh after fix: {new_mesh.get_name() if new_mesh else 'None'}")
                        num_mats = comp.get_num_materials()
                        log(f"    Materials after fix: {num_mats}")
                        for i in range(num_mats):
                            mat = comp.get_material(i)
                            log(f"      [{i}]: {mat.get_name() if mat else 'None'}")
                    except Exception as e:
                        log(f"    Verify error: {e}")

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
    else:
        log("\nNo SkyManager found to fix")

    log("\n" + "=" * 70)
    log("FIX COMPLETE")
    log("=" * 70)
    log(f"\nResults: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
