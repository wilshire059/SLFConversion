# dump_weapon_properties.py
# Dump all properties from weapon CDO in bp_only

import unreal

def log(msg):
    unreal.log_warning(str(msg))

WEAPON = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield"

def dump_props():
    log("=" * 70)
    log(f"DUMPING ALL PROPERTIES FROM: {WEAPON.split('/')[-1]}")
    log("=" * 70)

    bp = unreal.EditorAssetLibrary.load_asset(WEAPON)
    if not bp:
        log("Could not load Blueprint")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        log("No generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log("No CDO")
        return

    # Get all properties using export_text
    try:
        export_text = cdo.export_text()
        log("\n--- EXPORT TEXT (first 5000 chars) ---")
        log(export_text[:5000])
    except Exception as e:
        log(f"export_text error: {e}")

    # Spawn and check what sockets exist
    log("\n--- CHECKING SPAWNED ACTOR ---")
    try:
        spawn_loc = unreal.Vector(-100000, -100000, -100000)
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc)
        if actor:
            # Check for variables
            log(f"Actor class: {actor.get_class().get_name()}")

            # Get all components
            components = actor.get_components_by_class(unreal.ActorComponent)
            log(f"Components ({len(components)}):")
            for comp in components:
                log(f"  - {comp.get_name()} ({comp.get_class().get_name()})")

            actor.destroy_actor()
    except Exception as e:
        log(f"Spawn error: {e}")

if __name__ == "__main__":
    dump_props()
