# check_enemy_abilities.py
# Check if enemies have abilities configured in their AC_AI_CombatManager

import unreal

def log(msg):
    unreal.log_warning(f"[ABILITY_CHECK] {msg}")

ENEMY_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Demo",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
]

def check_enemy(path):
    log(f"")
    log(f"=== Checking {path.split('/')[-1]} ===")

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

    log(f"  CDO: {cdo.get_name()}")

    # Find AC_AI_CombatManager component
    combat_mgr = None
    components = cdo.get_components_by_class(unreal.ActorComponent)
    for comp in components:
        comp_name = comp.get_name()
        comp_class = comp.get_class().get_name()
        if "CombatManager" in comp_name or "CombatManager" in comp_class:
            log(f"  Found component: {comp_name} ({comp_class})")
            combat_mgr = comp
            break

    if not combat_mgr:
        log(f"  WARNING: No CombatManager component found!")
        return

    # Check for Abilities property
    try:
        abilities = combat_mgr.get_editor_property("abilities")
        log(f"  Abilities array length: {len(abilities) if abilities else 0}")
        if abilities:
            for i, ability in enumerate(abilities):
                log(f"    [{i}] {ability}")
        else:
            log(f"  WARNING: No abilities configured!")
    except Exception as e:
        log(f"  Could not get abilities: {e}")

    # Also check if using AICombatManagerComponent (C++)
    try:
        abilities_cpp = combat_mgr.get_editor_property("Abilities")  # Capital A
        log(f"  Abilities (C++) array length: {len(abilities_cpp) if abilities_cpp else 0}")
    except:
        pass

def main():
    log("=" * 70)
    log("Enemy Ability Configuration Check")
    log("=" * 70)

    for path in ENEMY_PATHS:
        check_enemy(path)

    log("")
    log("=" * 70)
    log("Done!")
    log("=" * 70)

if __name__ == "__main__":
    main()
