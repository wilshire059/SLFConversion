"""
Quick PIE test to verify StatManager initialization
"""
import unreal

def run_pie_test():
    """Load demo level and check StatManager logs"""

    unreal.log("=== PIE Test Starting ===")

    # Find all AC_StatManager components in editor
    try:
        world = unreal.EditorLevelLibrary.get_editor_world()
        if world:
            all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
            unreal.log(f"Found {len(all_actors)} actors in editor world")

            for actor in all_actors:
                components = actor.get_components_by_class(unreal.ActorComponent)
                for comp in components:
                    class_name = comp.get_class().get_name()
                    if "StatManager" in class_name:
                        unreal.log(f"Found StatManager on {actor.get_name()}: {class_name}")
                        try:
                            stat_table = comp.get_editor_property('stat_table')
                            unreal.log(f"  StatTable: {stat_table}")
                        except Exception as e:
                            unreal.log(f"  StatTable error: {e}")
    except Exception as e:
        unreal.log(f"Error: {e}")

    unreal.log("=== PIE Test Complete ===")

if __name__ == "__main__":
    run_pie_test()
