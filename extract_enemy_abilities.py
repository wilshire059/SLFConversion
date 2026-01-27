# extract_enemy_abilities.py
# Extract enemy abilities from backup project

import unreal

def log(msg):
    unreal.log_warning(f"[EXTRACT_ABILITIES] {msg}")

ENEMY_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
]

def extract_abilities(path):
    log(f"")
    log(f"=== Extracting from {path.split('/')[-1]} ===")

    bp = unreal.load_asset(path)
    if not bp:
        log(f"  ERROR: Could not load")
        return

    # Get generated class
    gen_class = bp.generated_class()
    if not gen_class:
        log(f"  ERROR: No generated class")
        return

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log(f"  ERROR: No CDO")
        return

    # Find AC_AI_CombatManager component
    components = cdo.get_components_by_class(unreal.ActorComponent)
    for comp in components:
        comp_name = comp.get_name()
        comp_class = comp.get_class().get_name()
        if "CombatManager" in comp_name or "Combat" in comp_class:
            log(f"  Component: {comp_name} ({comp_class})")

            # Try to get abilities
            try:
                abilities = comp.get_editor_property("Abilities")
                log(f"  Abilities count: {len(abilities) if abilities else 0}")
                if abilities:
                    for i, ability in enumerate(abilities):
                        log(f"    [{i}] Tag: {ability.get_editor_property('ability_tag')}")
                        asset = ability.get_editor_property('ability_asset')
                        log(f"        Asset: {asset.get_path_name() if asset else 'None'}")
                        log(f"        Weight: {ability.get_editor_property('weight')}")
                        log(f"        Cooldown: {ability.get_editor_property('cooldown')}")
            except Exception as e:
                log(f"  Error getting abilities: {e}")

def main():
    log("=" * 70)
    log("Extracting Enemy Abilities from Backup")
    log("=" * 70)

    for path in ENEMY_PATHS:
        extract_abilities(path)

    log("")
    log("=" * 70)
    log("Done!")
    log("=" * 70)

if __name__ == "__main__":
    main()
