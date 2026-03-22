# export_container_instance_data.py
# Export B_Container level instances as text to extract loot configuration
# Run on bp_only project

import unreal
import json
import re

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("EXPORTING CONTAINER INSTANCE DATA FROM LEVEL")
    unreal.log_warning("=" * 70)

    # Load the demo level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    unreal.log_warning(f"Loading level: {level_path}")

    success = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
    if not success:
        unreal.log_error(f"Failed to load level: {level_path}")
        return

    unreal.log_warning("Level loaded successfully")

    # Get editor world
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
            unreal.log_warning(f"  Class: {actor_class}")
            unreal.log_warning(f"  Location: X={actor_location.x:.1f} Y={actor_location.y:.1f} Z={actor_location.z:.1f}")

            # Export the entire actor as text
            try:
                exported = actor.export_text()

                # Parse the exported text for loot configuration
                loot_info = {
                    "name": actor_name,
                    "class": actor_class,
                    "location": {
                        "x": float(actor_location.x),
                        "y": float(actor_location.y),
                        "z": float(actor_location.z)
                    },
                    "override_item": None,
                    "loot_table": None,
                    "raw_export": exported[:5000] if exported else None
                }

                if exported:
                    # Look for Override Item pattern
                    # Pattern: "Override Item"=(Item=...,MinAmount=...,MaxAmount=...)
                    override_match = re.search(r'"Override Item"\s*=\s*\((.*?)\)', exported, re.IGNORECASE | re.DOTALL)
                    if override_match:
                        override_text = override_match.group(1)
                        unreal.log_warning(f"  Override Item found: {override_text[:200]}")
                        loot_info["override_item"] = override_text

                    # Look for Loot Table pattern
                    table_match = re.search(r'"Loot Table"\s*=\s*([^\s,\)]+)', exported, re.IGNORECASE)
                    if table_match:
                        table_ref = table_match.group(1)
                        unreal.log_warning(f"  Loot Table found: {table_ref}")
                        loot_info["loot_table"] = table_ref

                    # Also check for Item= patterns within the export
                    item_matches = re.findall(r'Item=([^,\)]+)', exported)
                    if item_matches:
                        unreal.log_warning(f"  Item references found: {item_matches[:5]}")
                        loot_info["item_references"] = item_matches[:10]

                container_data[actor_name] = loot_info

            except Exception as e:
                unreal.log_warning(f"  Export error: {e}")

            # Also try to get components and export them
            try:
                components = actor.get_components_by_class(unreal.ActorComponent)
                for comp in components:
                    comp_class = comp.get_class().get_name()
                    if "lootdropmanager" in comp_class.lower():
                        unreal.log_warning(f"  LootDropManager component: {comp.get_name()}")

                        try:
                            comp_export = comp.export_text()
                            if comp_export:
                                unreal.log_warning(f"  Component export (first 500 chars):")
                                unreal.log_warning(comp_export[:500])

                                if actor_name in container_data:
                                    container_data[actor_name]["component_export"] = comp_export[:2000]
                        except Exception as e:
                            unreal.log_warning(f"  Component export error: {e}")
            except Exception as e:
                unreal.log_warning(f"  Component iteration error: {e}")

    # Save to JSON
    output_path = "C:/scripts/SLFConversion/migration_cache/container_instance_export.json"
    with open(output_path, "w") as f:
        json.dump(container_data, f, indent=2)

    unreal.log_warning(f"\n" + "=" * 70)
    unreal.log_warning(f"Saved {len(container_data)} container exports to: {output_path}")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
