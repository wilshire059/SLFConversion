"""
Check the actual class hierarchy of boss doors in the level.
This will reveal which C++ class the Blueprint inherits from.
"""

import unreal

def check_door_hierarchy():
    """Check boss door class hierarchy."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING BOSS DOOR CLASS HIERARCHY")
    unreal.log_warning("=" * 70)

    # Check B_BossDoor Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
    bp = unreal.load_asset(bp_path)

    if bp:
        unreal.log_warning(f"\nB_BossDoor Blueprint:")
        unreal.log_warning(f"  Path: {bp_path}")

        gen_class = bp.generated_class()
        if gen_class:
            unreal.log_warning(f"  Generated Class: {gen_class.get_name()}")

            # Walk the class hierarchy
            current = gen_class
            level = 0
            while current:
                class_name = current.get_name()
                class_path = current.get_path_name()
                unreal.log_warning(f"  {'  ' * level}-> {class_name}")
                unreal.log_warning(f"  {'  ' * level}   Path: {class_path}")

                # Check if this is a native C++ class
                is_native = "Blueprint" not in class_path and "/Script/" in class_path
                unreal.log_warning(f"  {'  ' * level}   Native C++: {is_native}")

                # Get parent
                try:
                    parent = unreal.get_type_from_class(current)
                    if parent and hasattr(current, 'static_class'):
                        parent_class = current.static_class()
                        if parent_class != current:
                            current = parent_class
                        else:
                            break
                    else:
                        break
                except:
                    break

                level += 1
                if level > 10:
                    break

    # Check B_Door Blueprint (parent of B_BossDoor)
    bp_path2 = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
    bp2 = unreal.load_asset(bp_path2)

    if bp2:
        unreal.log_warning(f"\nB_Door Blueprint:")
        gen_class2 = bp2.generated_class()
        if gen_class2:
            unreal.log_warning(f"  Generated Class: {gen_class2.get_name()}")

    # Check all actors in level named "Boss"
    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("CHECKING LEVEL ACTORS")
    unreal.log_warning("=" * 70)

    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        all_actors = unreal.EditorLevelLibrary.get_all_level_actors()
        for actor in all_actors:
            if actor and "Boss" in actor.get_name():
                actor_class = actor.get_class()
                unreal.log_warning(f"\nActor: {actor.get_name()}")
                unreal.log_warning(f"  Class: {actor_class.get_name() if actor_class else 'Unknown'}")

                if actor_class:
                    class_path = actor_class.get_path_name()
                    unreal.log_warning(f"  Class Path: {class_path}")

                    # Check if it implements specific interfaces
                    try:
                        # Try to find OnInteract method
                        has_slf_interface = hasattr(actor, 'on_interact')
                        unreal.log_warning(f"  Has on_interact: {has_slf_interface}")
                    except:
                        pass

                    # Get components
                    components = actor.get_components_by_class(unreal.ActorComponent)
                    unreal.log_warning(f"  Components ({len(components)}):")
                    for comp in components[:10]:
                        if comp:
                            comp_class = comp.get_class()
                            unreal.log_warning(f"    - {comp.get_name()} ({comp_class.get_name() if comp_class else 'Unknown'})")

    # Also check what AB_BossDoor and ASLFBossDoor C++ classes exist
    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("CHECKING C++ CLASSES")
    unreal.log_warning("=" * 70)

    try:
        # Try to load AB_BossDoor
        ab_boss = unreal.load_class(None, "/Script/SLFConversion.B_BossDoor")
        if ab_boss:
            unreal.log_warning(f"AB_BossDoor C++ class found: {ab_boss.get_name()}")
    except Exception as e:
        unreal.log_warning(f"AB_BossDoor: {e}")

    try:
        # Try to load ASLFBossDoor
        aslf_boss = unreal.load_class(None, "/Script/SLFConversion.SLFBossDoor")
        if aslf_boss:
            unreal.log_warning(f"ASLFBossDoor C++ class found: {aslf_boss.get_name()}")
    except Exception as e:
        unreal.log_warning(f"ASLFBossDoor: {e}")

    try:
        # Try to load AB_Door
        ab_door = unreal.load_class(None, "/Script/SLFConversion.B_Door")
        if ab_door:
            unreal.log_warning(f"AB_Door C++ class found: {ab_door.get_name()}")
    except Exception as e:
        unreal.log_warning(f"AB_Door: {e}")

    unreal.log_warning("\n" + "=" * 70)

if __name__ == "__main__":
    check_door_hierarchy()
