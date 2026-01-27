# apply_loot_to_level_containers.py
# Apply loot table to container instances in the level

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("APPLYING LOOT TABLE TO LEVEL CONTAINER INSTANCES")
    unreal.log_warning("=" * 70)

    # Load the loot table
    loot_table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleChestTier"
    loot_table = unreal.load_asset(loot_table_path)
    if not loot_table:
        unreal.log_error(f"Could not load loot table: {loot_table_path}")
        return

    unreal.log_warning(f"Loot table: {loot_table.get_name()}")

    # Get all actors in the level that are B_Container
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
            unreal.log_warning(f"\nFound container: {actor.get_name()} ({actor_class})")
            container_count += 1

            # Get its components
            components = actor.get_components_by_class(unreal.ActorComponent)
            for comp in components:
                comp_class = comp.get_class().get_name()
                if "lootdropmanager" in comp_class.lower():
                    unreal.log_warning(f"  Found LootDropManager: {comp.get_name()} ({comp_class})")

                    # Try to set the loot table
                    try:
                        # Try as soft object ptr
                        soft_ref = unreal.SoftObjectPath(loot_table_path)
                        comp.set_editor_property("loot_table", soft_ref)
                        unreal.log_warning(f"  Set loot_table as SoftObjectPath!")
                    except Exception as e:
                        unreal.log_warning(f"  SoftObjectPath error: {e}")
                        try:
                            # Try direct
                            comp.set_editor_property("loot_table", loot_table)
                            unreal.log_warning(f"  Set loot_table directly!")
                        except Exception as e2:
                            unreal.log_warning(f"  Direct error: {e2}")

                    # Verify
                    try:
                        current = comp.get_editor_property("loot_table")
                        unreal.log_warning(f"  Current loot_table: {current}")
                    except Exception as e:
                        unreal.log_warning(f"  Verify error: {e}")

    unreal.log_warning(f"\nTotal containers found: {container_count}")

    # Mark level dirty and save if we made changes
    if container_count > 0:
        unreal.log_warning("\n--- Marking level dirty ---")
        unreal.EditorLevelLibrary.mark_actors_dirty_for_cook()

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
