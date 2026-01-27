# test_animation_variables_pie.py
# Test if animation variables are being set during PIE
# This checks if Blueprint logic is working even though C++ NativeUpdateAnimation isn't called

import unreal
import time

def log(msg):
    unreal.log_warning(f"[AnimVarTest] {msg}")

def main():
    log("=" * 60)
    log("Animation Variable PIE Test")
    log("=" * 60)

    # Start PIE
    log("Starting PIE...")
    success = unreal.SLFInputSimulatorLibrary.start_pie()
    if not success:
        log("[ERROR] Failed to start PIE")
        return False

    log("PIE started, waiting for initialization...")
    time.sleep(3.0)

    if not unreal.SLFInputSimulatorLibrary.is_pie_running():
        log("[ERROR] PIE not running")
        return False

    # Get PIE world
    pie_world = unreal.SLFInputSimulatorLibrary.get_pie_world()
    if not pie_world:
        log("[WARNING] Could not get PIE world")
    else:
        log(f"PIE World: {pie_world.get_name()}")

        # Try to find player pawn and its AnimInstance
        try:
            # Get all actors of type Character
            actors = unreal.GameplayStatics.get_all_actors_of_class(pie_world, unreal.Character)
            log(f"Found {len(actors)} Character actors in PIE world")

            for actor in actors:
                log(f"  Character: {actor.get_name()}")

                # Try to get skeletal mesh component
                mesh = actor.get_component_by_class(unreal.SkeletalMeshComponent)
                if mesh:
                    anim_instance = mesh.get_anim_instance()
                    if anim_instance:
                        log(f"    AnimInstance: {anim_instance.get_class().get_name()}")

                        # Try to read some variables
                        try:
                            speed = anim_instance.get_editor_property("speed") if hasattr(anim_instance, "speed") else "N/A"
                            log(f"    Speed: {speed}")
                        except:
                            log("    Speed: Could not read")

                        try:
                            velocity = anim_instance.get_editor_property("velocity") if hasattr(anim_instance, "velocity") else "N/A"
                            log(f"    Velocity: {velocity}")
                        except:
                            log("    Velocity: Could not read")
                    else:
                        log("    AnimInstance: None")
                else:
                    log("    SkeletalMeshComponent: None")
        except Exception as e:
            log(f"[ERROR] Exception finding characters: {e}")

    log("Letting animations tick for 3 seconds...")
    time.sleep(3.0)

    log("--- Stopping PIE ---")
    unreal.SLFInputSimulatorLibrary.stop_pie()
    log("PIE stopped")

    log("=" * 60)
    log("Animation Variable Test Complete")
    log("=" * 60)

    return True

if __name__ == "__main__":
    result = main()
    if result:
        unreal.log_warning("[AnimVarTest] TEST COMPLETE")
    else:
        unreal.log_error("[AnimVarTest] TEST FAILED")
