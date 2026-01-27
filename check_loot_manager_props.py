# check_loot_manager_props.py
# List all properties of AC_LootDropManager component

import unreal

def main():
    # Load the demo level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    unreal.EditorLoadingAndSavingUtils.load_map(level_path)

    world = unreal.EditorLevelLibrary.get_editor_world()
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)

    for actor in all_actors:
        if "container" in actor.get_class().get_name().lower():
            unreal.log_warning(f"\nContainer: {actor.get_name()}")

            components = actor.get_components_by_class(unreal.ActorComponent)
            for comp in components:
                if "lootdropmanager" in comp.get_class().get_name().lower():
                    unreal.log_warning(f"  Component: {comp.get_name()} ({comp.get_class().get_name()})")
                    unreal.log_warning(f"  All accessible properties:")

                    # Try to get properties via dir()
                    for attr in dir(comp):
                        if not attr.startswith('_'):
                            try:
                                val = comp.get_editor_property(attr)
                                unreal.log_warning(f"    {attr}: {val}")
                            except:
                                pass
            break  # Just check first container

if __name__ == "__main__":
    main()
