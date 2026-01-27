# apply_override_item_to_containers.py
# Set OverrideItem directly on container LootDropManager components

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("APPLYING OVERRIDE ITEM TO CONTAINERS")
    unreal.log_warning("=" * 70)

    # Load a sample item to use as the override
    item_path = "/Game/SoulslikeFramework/Data/Items/DA_Sword01"
    item = unreal.load_asset(item_path)
    if not item:
        unreal.log_warning(f"Could not load item: {item_path}")
        # Try another item
        item_path = "/Game/SoulslikeFramework/Data/Items/DA_ExampleTalisman"
        item = unreal.load_asset(item_path)

    if item:
        unreal.log_warning(f"Using item: {item.get_name()}")
    else:
        unreal.log_error("Could not load any items!")
        return

    # Load B_PickupItem class
    pickup_class_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem"
    pickup_bp = unreal.load_asset(pickup_class_path)
    pickup_class = None
    if pickup_bp:
        pickup_class = pickup_bp.generated_class()
        unreal.log_warning(f"Pickup class: {pickup_class.get_name() if pickup_class else 'None'}")

    # Get all container actors in level
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        unreal.log_error("No editor world")
        return

    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
    unreal.log_warning(f"Total actors: {len(all_actors)}")

    container_count = 0
    for actor in all_actors:
        actor_class = actor.get_class().get_name()
        if "container" in actor_class.lower():
            unreal.log_warning(f"\nFound container: {actor.get_name()}")
            container_count += 1

            # Find LootDropManager component
            components = actor.get_components_by_class(unreal.ActorComponent)
            for comp in components:
                comp_class = comp.get_class().get_name()
                if "lootdropmanager" in comp_class.lower():
                    unreal.log_warning(f"  LootDropManager: {comp.get_name()}")

                    # Try to get/set override_item struct
                    try:
                        override_item = comp.get_editor_property("override_item")
                        unreal.log_warning(f"  Current override_item: {override_item}")

                        # Try to set the Item property on the struct
                        if override_item:
                            try:
                                override_item.set_editor_property("item", item)
                                unreal.log_warning(f"  Set item on override_item!")
                            except Exception as e:
                                unreal.log_warning(f"  Error setting item: {e}")

                            # Also try item_class
                            if pickup_class:
                                try:
                                    override_item.set_editor_property("item_class", pickup_class)
                                    unreal.log_warning(f"  Set item_class on override_item!")
                                except Exception as e:
                                    unreal.log_warning(f"  Error setting item_class: {e}")

                            # Save back
                            comp.set_editor_property("override_item", override_item)
                            unreal.log_warning(f"  Saved override_item back to component")

                    except Exception as e:
                        unreal.log_warning(f"  Error with override_item: {e}")

    unreal.log_warning(f"\nTotal containers: {container_count}")

    # Save level
    if container_count > 0:
        unreal.log_warning("\n--- Saving level ---")
        current_level = unreal.EditorLevelLibrary.get_editor_world().get_outer()
        if current_level:
            unreal.log_warning(f"Level: {current_level.get_name()}")
            # Mark dirty
            unreal.EditorLevelLibrary.mark_actors_dirty_for_cook()

    unreal.log_warning("\n" + "=" * 70)

if __name__ == "__main__":
    main()
