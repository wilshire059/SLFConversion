# apply_enemy_abilities.py
# Add default abilities to enemy AC_AI_CombatManager components

import unreal

def log(msg):
    unreal.log_warning(f"[APPLY_ABILITIES] {msg}")

# Ability data assets that exist in the project
ABILITY_ASSETS = [
    "/Game/SoulslikeFramework/Data/AI_Abilities/DA_AI_Ability_Enemy_Attack01",
    "/Game/SoulslikeFramework/Data/AI_Abilities/DA_AI_Ability_Enemy_Attack02",
    "/Game/SoulslikeFramework/Data/AI_Abilities/DA_AI_Ability_Enemy_Attack03_Fast",
]

BOSS_ABILITY_ASSETS = [
    "/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_FourSwings",
    "/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_FiveSwings",
    "/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_HeavyAttack+Projectile",
]

ENEMY_CONFIG = {
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard": ABILITY_ASSETS,
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase": ABILITY_ASSETS,
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss": BOSS_ABILITY_ASSETS,
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth": BOSS_ABILITY_ASSETS,
}

def apply_abilities(bp_path, ability_paths):
    log(f"")
    log(f"=== {bp_path.split('/')[-1]} ===")

    bp = unreal.load_asset(bp_path)
    if not bp:
        log(f"  ERROR: Could not load blueprint")
        return False

    gen_class = bp.generated_class()
    if not gen_class:
        log(f"  ERROR: No generated class")
        return False

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log(f"  ERROR: No CDO")
        return False

    # Find CombatManager component
    combat_mgr = None
    components = cdo.get_components_by_class(unreal.ActorComponent)
    for comp in components:
        comp_class = comp.get_class().get_name()
        if "CombatManager" in comp_class:
            combat_mgr = comp
            break

    if not combat_mgr:
        log(f"  ERROR: No CombatManager component")
        return False

    log(f"  Found: {combat_mgr.get_class().get_name()}")

    # Build ability array
    abilities = []
    for ability_path in ability_paths:
        ability_asset = unreal.load_asset(ability_path)
        if ability_asset:
            # Create FSLFAIAbility struct
            ability_entry = unreal.SLFAIAbility()
            ability_entry.set_editor_property("ability_asset", ability_asset)
            ability_entry.set_editor_property("weight", 1.0)
            ability_entry.set_editor_property("cooldown", 0.5)
            abilities.append(ability_entry)
            log(f"    Added: {ability_asset.get_name()}")
        else:
            log(f"    WARNING: Could not load {ability_path}")

    if not abilities:
        log(f"  ERROR: No abilities loaded")
        return False

    # Set abilities on component
    try:
        combat_mgr.set_editor_property("Abilities", abilities)
        log(f"  Set {len(abilities)} abilities")

        # Save the blueprint
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        log(f"  Saved!")
        return True
    except Exception as e:
        log(f"  ERROR setting abilities: {e}")
        return False

def main():
    log("=" * 70)
    log("Applying Abilities to Enemies")
    log("=" * 70)

    success = 0
    for bp_path, abilities in ENEMY_CONFIG.items():
        if apply_abilities(bp_path, abilities):
            success += 1

    log("")
    log("=" * 70)
    log(f"Result: {success}/{len(ENEMY_CONFIG)} enemies configured")
    log("=" * 70)

if __name__ == "__main__":
    main()
