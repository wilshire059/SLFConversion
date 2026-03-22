# extract_container_loot.py
# Extract loot/item spawning configuration from bp_only B_Container

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("EXTRACTING B_Container LOOT CONFIG FROM BP_ONLY")
    unreal.log_warning("=" * 70)

    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("No generated class")
        return

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    unreal.log_warning(f"\nCDO: {cdo.get_name()}")

    # Check all properties on the CDO
    unreal.log_warning("\n--- All CDO Properties ---")
    
    # Common loot-related property names to check
    loot_props = [
        "loot", "item", "items", "drop", "drops", "reward", "rewards",
        "loot_table", "item_to_spawn", "spawn_item", "container_item",
        "pickup", "pickups", "contents", "inventory", "loot_item"
    ]
    
    # Try to get properties via reflection
    for prop in dir(cdo):
        prop_lower = prop.lower()
        # Check if property name contains loot-related keywords
        if any(kw in prop_lower for kw in ['loot', 'item', 'drop', 'spawn', 'pickup', 'reward', 'content']):
            try:
                val = getattr(cdo, prop)
                if not callable(val) and not prop.startswith('_'):
                    unreal.log_warning(f"  {prop}: {val}")
            except:
                pass

    # Check Blueprint variables
    unreal.log_warning("\n--- Blueprint Variables ---")
    try:
        # Get all variables from the Blueprint
        for var_name in ['LootItem', 'ItemToSpawn', 'ContainerItem', 'DropItem', 
                         'SpawnItem', 'LootTable', 'Items', 'Loot']:
            try:
                val = cdo.get_editor_property(var_name.lower())
                unreal.log_warning(f"  {var_name}: {val}")
            except:
                pass
    except Exception as e:
        unreal.log_warning(f"Variable check error: {e}")

    # Spawn instance and check for any item-related components
    unreal.log_warning("\n--- Spawned Instance Check ---")
    spawn_loc = unreal.Vector(0, 0, 2000)
    spawn_rot = unreal.Rotator(0, 0, 0)
    
    try:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc, spawn_rot)
        if actor:
            # Check all properties on the spawned actor
            for prop in dir(actor):
                prop_lower = prop.lower()
                if any(kw in prop_lower for kw in ['loot', 'item', 'drop', 'spawn', 'pickup']):
                    try:
                        val = getattr(actor, prop)
                        if not callable(val) and not prop.startswith('_'):
                            unreal.log_warning(f"  Actor.{prop}: {val}")
                    except:
                        pass
            
            actor.destroy_actor()
    except Exception as e:
        unreal.log_warning(f"Spawn error: {e}")

    # Export Blueprint to see structure
    unreal.log_warning("\n--- Blueprint Export (searching for loot references) ---")
    try:
        export_text = unreal.EditorAssetLibrary.export_asset_to_string(bp_path)
        if export_text:
            # Search for loot-related keywords in the export
            lines = export_text.split('\n')
            for i, line in enumerate(lines):
                line_lower = line.lower()
                if any(kw in line_lower for kw in ['loot', 'pickup', 'spawnactor', 'item']):
                    # Print context (line and surrounding)
                    unreal.log_warning(f"  Line {i}: {line[:200]}")
    except Exception as e:
        unreal.log_warning(f"Export error: {e}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
