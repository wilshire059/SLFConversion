# fix_boss_abilities.py
# Populates the boss's Abilities TArray with all boss attack abilities
#
# Run: UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/fix_boss_abilities.py" -stdout -unattended

import unreal

BOSS_BLUEPRINT = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth"

# Boss abilities with their configurations
# Format: (path, weight, cooldown, min_dist, max_dist, is_gap_closer, health_threshold, below_threshold)
BOSS_ABILITIES = [
    # Close range attacks
    ("/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_FourSwings", 1.0, 2.0, 0, 400, False, 0.0, False),
    ("/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_FiveSwings", 1.0, 2.5, 0, 400, False, 0.0, False),
    ("/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_SevenSwings", 0.8, 4.0, 0, 400, False, 0.0, False),
    ("/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_JukeSwings", 1.0, 3.0, 0, 500, False, 0.0, False),
    ("/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_FourSwings+DoublePain", 0.9, 3.0, 0, 400, False, 0.0, False),
    ("/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_HeavyAttack+Projectile", 0.8, 4.0, 0, 500, False, 0.0, False),

    # Gap closers (for when player is far)
    ("/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_JumpAttackClose", 1.2, 3.0, 400, 1000, True, 0.0, False),
    ("/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_Ranged_Charge", 1.5, 4.0, 500, 1500, True, 0.0, False),

    # Ranged attacks
    ("/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_Ranged_Fireball", 1.0, 3.0, 400, 1200, False, 0.0, False),
    ("/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_Ranged_DoublePain", 0.8, 3.5, 400, 1000, False, 0.0, False),

    # Enrage/phase 2 attack (only when HP below 50%)
    ("/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_FastLongCombo", 1.5, 5.0, 0, 500, False, 0.5, True),
]

def fix_boss_abilities():
    """Set up the boss's Abilities array with all attack abilities."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("FIXING BOSS ABILITIES ARRAY")
    unreal.log_warning("=" * 70)

    # Load the Blueprint
    bp = unreal.load_asset(BOSS_BLUEPRINT)
    if not bp:
        unreal.log_error(f"Failed to load Blueprint: {BOSS_BLUEPRINT}")
        return

    unreal.log_warning(f"Loaded Blueprint: {bp.get_name()}")

    # Get the generated class
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("No generated class!")
        return

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_error("No CDO!")
        return

    unreal.log_warning(f"CDO: {cdo.get_name()}")

    # Find the AICombatManagerComponent on the CDO
    components = cdo.get_components_by_class(unreal.ActorComponent)
    unreal.log_warning(f"Found {len(components)} components")

    combat_manager = None
    for comp in components:
        comp_class = comp.get_class().get_name()
        if "CombatManager" in comp_class:
            combat_manager = comp
            unreal.log_warning(f"Found CombatManager: {comp.get_name()} ({comp_class})")
            break

    if not combat_manager:
        unreal.log_error("Could not find AICombatManagerComponent on CDO!")
        return

    # Create the abilities array
    abilities_to_add = []

    for ability_data in BOSS_ABILITIES:
        path, weight, cooldown, min_dist, max_dist, is_gap_closer, health_thresh, below_thresh = ability_data

        # Load the ability asset
        ability_asset = unreal.load_asset(path)
        if not ability_asset:
            unreal.log_warning(f"  [SKIP] Could not load: {path}")
            continue

        # Create FSLFAIAbility struct
        try:
            ability = unreal.SLFAIAbility()
            ability.set_editor_property("ability_asset", ability_asset)
            ability.set_editor_property("weight", weight)
            ability.set_editor_property("cooldown", cooldown)
            ability.set_editor_property("min_distance", min_dist)
            ability.set_editor_property("max_distance", max_dist)
            ability.set_editor_property("is_gap_closer", is_gap_closer)
            ability.set_editor_property("health_threshold", health_thresh)
            ability.set_editor_property("use_below_threshold", below_thresh)

            abilities_to_add.append(ability)
            unreal.log_warning(f"  [OK] Added: {ability_asset.get_name()} (W={weight}, CD={cooldown}, Dist={min_dist}-{max_dist})")
        except Exception as e:
            unreal.log_error(f"  [ERROR] Failed to create ability struct: {e}")
            continue

    if len(abilities_to_add) == 0:
        unreal.log_error("No abilities created!")
        return

    # Set the abilities array on the combat manager
    try:
        combat_manager.set_editor_property("abilities", abilities_to_add)
        unreal.log_warning(f"\n[OK] Set {len(abilities_to_add)} abilities on CombatManagerComponent")

        # Save the Blueprint
        unreal.EditorAssetLibrary.save_asset(BOSS_BLUEPRINT)
        unreal.log_warning(f"[SAVED] {BOSS_BLUEPRINT}")

    except Exception as e:
        unreal.log_error(f"Failed to set abilities: {e}")
        return

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning(f"Done! Boss now has {len(abilities_to_add)} abilities configured.")

if __name__ == "__main__":
    fix_boss_abilities()
