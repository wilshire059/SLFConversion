# extract_container_loot_config.py
# Extract loot configuration from bp_only container instances in the level
# Run on bp_only project to get the original loot assignments

import unreal
import json

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("EXTRACTING CONTAINER LOOT CONFIGURATION FROM LEVEL INSTANCES")
    unreal.log_warning("=" * 70)

    # Load the demo level first
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    unreal.log_warning(f"Loading level: {level_path}")

    # Use EditorLoadingAndSavingUtils to load the level
    success = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
    if not success:
        unreal.log_error(f"Failed to load level: {level_path}")
        return

    unreal.log_warning("Level loaded successfully")

    # Get all actors in the level
    world = unreal.UnrealEditorSubsystem().get_editor_world()
    if not world:
        # Fallback to deprecated method
        world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        unreal.log_error("No editor world")
        return

    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
    unreal.log_warning(f"Total actors: {len(all_actors)}")

    container_data = {}

    for actor in all_actors:
        actor_class = actor.get_class().get_name()
        if "container" in actor_class.lower():
            actor_name = actor.get_name()
            actor_location = actor.get_actor_location()

            unreal.log_warning(f"\nFound container: {actor_name}")
            unreal.log_warning(f"  Location: X={actor_location.x:.1f} Y={actor_location.y:.1f} Z={actor_location.z:.1f}")

            loot_config = {
                "name": actor_name,
                "location": {
                    "x": float(actor_location.x),
                    "y": float(actor_location.y),
                    "z": float(actor_location.z)
                },
                "override_item": None,
                "override_item_class": None,
                "loot_table": None,
                "min_amount": 1,
                "max_amount": 1
            }

            # Find LootDropManager component
            components = actor.get_components_by_class(unreal.ActorComponent)
            for comp in components:
                comp_class = comp.get_class().get_name()
                if "lootdropmanager" in comp_class.lower():
                    unreal.log_warning(f"  LootDropManager: {comp.get_name()}")

                    # Try to get override_item struct
                    try:
                        override_item = comp.get_editor_property("override_item")
                        if override_item:
                            unreal.log_warning(f"    OverrideItem struct found")

                            # Try to get the Item property from the struct
                            try:
                                item = override_item.get_editor_property("item")
                                if item:
                                    item_path = item.get_path_name()
                                    loot_config["override_item"] = item_path
                                    unreal.log_warning(f"    -> Item: {item_path}")
                            except Exception as e:
                                unreal.log_warning(f"    -> Item error: {e}")

                            # Try to get item_class
                            try:
                                item_class = override_item.get_editor_property("item_class")
                                if item_class:
                                    class_path = item_class.get_path_name()
                                    loot_config["override_item_class"] = class_path
                                    unreal.log_warning(f"    -> ItemClass: {class_path}")
                            except Exception as e:
                                unreal.log_warning(f"    -> ItemClass error: {e}")

                            # Try to get amounts
                            try:
                                min_amt = override_item.get_editor_property("min_amount")
                                max_amt = override_item.get_editor_property("max_amount")
                                loot_config["min_amount"] = int(min_amt) if min_amt else 1
                                loot_config["max_amount"] = int(max_amt) if max_amt else 1
                                unreal.log_warning(f"    -> Amount: {min_amt}-{max_amt}")
                            except:
                                pass
                    except Exception as e:
                        unreal.log_warning(f"  Error getting override_item: {e}")

                    # Try to get loot_table (soft object reference)
                    try:
                        loot_table = comp.get_editor_property("loot_table")
                        if loot_table:
                            # Convert to string - it's a soft reference
                            table_str = str(loot_table)
                            if table_str and "None" not in table_str:
                                loot_config["loot_table"] = table_str
                                unreal.log_warning(f"    -> LootTable: {table_str}")
                    except Exception as e:
                        unreal.log_warning(f"  Error getting loot_table: {e}")

            # Only add if we have some loot config
            if loot_config["override_item"] or loot_config["loot_table"]:
                container_data[actor_name] = loot_config
                unreal.log_warning(f"  => Saved loot config")
            else:
                unreal.log_warning(f"  => No loot configured")

    # Save to JSON
    output_path = "C:/scripts/SLFConversion/migration_cache/container_loot_config.json"
    with open(output_path, "w") as f:
        json.dump(container_data, f, indent=2)

    unreal.log_warning(f"\n" + "=" * 70)
    unreal.log_warning(f"Saved {len(container_data)} container configs to: {output_path}")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
