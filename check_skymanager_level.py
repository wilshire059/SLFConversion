"""
Check B_SkyManager instance in the level
"""
import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("B_SkyManager LEVEL INSTANCE CHECK")
    unreal.log_warning("=" * 70)

    # Get all actors in the editor world
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        unreal.log_warning("ERROR: Could not get editor world")
        return

    unreal.log_warning(f"World: {world.get_name()}")

    # Find all actors
    all_actors = unreal.EditorLevelLibrary.get_all_level_actors()
    unreal.log_warning(f"Total actors in level: {len(all_actors)}")

    # Find SkyManager
    sky_managers = []
    for actor in all_actors:
        actor_name = actor.get_name()
        actor_class = actor.get_class().get_name() if actor.get_class() else "Unknown"

        if "SkyManager" in actor_name or "SkyManager" in actor_class:
            sky_managers.append(actor)
            unreal.log_warning(f"\nFound SkyManager: {actor_name}")
            unreal.log_warning(f"  Class: {actor_class}")

            # Get parent class chain
            current = actor.get_class()
            depth = 0
            while current:
                unreal.log_warning(f"  {'  ' * depth}Parent: {current.get_name()}")
                try:
                    parent = current.get_super_class() if hasattr(current, 'get_super_class') else None
                    if parent and parent != current:
                        current = parent
                    else:
                        break
                except:
                    break
                depth += 1

            # Check components
            unreal.log_warning(f"\n  Components:")
            try:
                components = actor.get_components_by_class(unreal.ActorComponent)
                unreal.log_warning(f"    Total: {len(components)}")
                for comp in components:
                    comp_name = comp.get_name()
                    comp_class = comp.get_class().get_name() if comp.get_class() else "Unknown"
                    unreal.log_warning(f"      {comp_name} ({comp_class})")

                    # Extra info for light components
                    if "Light" in comp_class:
                        try:
                            intensity = comp.get_editor_property('intensity')
                            unreal.log_warning(f"        Intensity: {intensity}")
                        except:
                            pass
            except Exception as e:
                unreal.log_warning(f"    Error getting components: {e}")

            # Check properties
            unreal.log_warning(f"\n  Properties:")
            props = ['time', 'current_time_of_day', 'day_length_minutes', 'sunrise_time', 'sunset_time']
            for prop in props:
                try:
                    val = actor.get_editor_property(prop)
                    unreal.log_warning(f"    {prop}: {val}")
                except:
                    pass

    if len(sky_managers) == 0:
        unreal.log_warning("\nNo SkyManager actors found in level!")
        unreal.log_warning("Try searching for any actor with 'Sky' in the name:")
        for actor in all_actors:
            if "Sky" in actor.get_name():
                unreal.log_warning(f"  {actor.get_name()} ({actor.get_class().get_name() if actor.get_class() else 'Unknown'})")

    unreal.log_warning("\n" + "=" * 70)

main()
