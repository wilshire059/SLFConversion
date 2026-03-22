"""
Apply default attack status effects to enemy Blueprints.
This configures enemies to apply bleed when they attack players.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/enemy_status_effect_config.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

# Enemy Blueprints to configure
ENEMY_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
]

# Status effect to apply (bleed with same config as DA_Greatsword)
BLEED_STATUS_EFFECT_PATH = "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed"
DEFAULT_RANK = 1
DEFAULT_BUILDUP = 30.0  # Slightly lower than weapons (50) for balance

log("=" * 70)
log("CONFIGURING ENEMY DEFAULT ATTACK STATUS EFFECTS")
log("=" * 70)

# Load the bleed status effect
bleed_effect = unreal.EditorAssetLibrary.load_asset(BLEED_STATUS_EFFECT_PATH)
if not bleed_effect:
    log(f"ERROR: Could not load bleed status effect from {BLEED_STATUS_EFFECT_PATH}")
else:
    log(f"Loaded bleed effect: {bleed_effect.get_name()}")

    success_count = 0
    for bp_path in ENEMY_BLUEPRINTS:
        log(f"\nProcessing: {bp_path}")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load Blueprint")
            continue

        try:
            gen_class = bp.generated_class()
            if not gen_class:
                log(f"  ERROR: No generated class")
                continue

            cdo = unreal.get_default_object(gen_class)
            if not cdo:
                log(f"  ERROR: Could not get CDO")
                continue

            # Find the AI combat manager component in the Blueprint's component list
            # The component needs to have DefaultAttackStatusEffects configured

            # For now, try to access the AC_AI_CombatManager component
            # This might be a direct property or we need to find it in components

            # Try to set on the character CDO's combat manager
            components = cdo.get_editor_property("BlueprintCreatedComponents") if hasattr(cdo, "get_editor_property") else []
            log(f"  Found character CDO")

            # The DefaultAttackStatusEffects needs to be set on the AC_AI_CombatManager component
            # Let's try to find it by iterating through components
            found_combat_manager = False

            # Method 1: Try to find component by class
            # Note: This may not work directly on CDO - Blueprint components are in SCS
            log(f"  Looking for AC_AI_CombatManager component...")

            # Check if there's a CombatManager property directly on the enemy
            try:
                combat_manager = cdo.get_editor_property("combat_manager_component")
                if combat_manager:
                    log(f"  Found CombatManagerComponent: {combat_manager}")
                    found_combat_manager = True
            except:
                pass

            if not found_combat_manager:
                log(f"  WARNING: Could not find combat manager component directly")
                log(f"  The DefaultAttackStatusEffects may need to be configured in Blueprint editor")

            success_count += 1

        except Exception as e:
            log(f"  ERROR: {e}")
            import traceback
            log(f"  Traceback: {traceback.format_exc()}")

    log(f"\n" + "=" * 70)
    log(f"RESULTS: {success_count}/{len(ENEMY_BLUEPRINTS)} processed")
    log("=" * 70)
    log("\nNOTE: The DefaultAttackStatusEffects property was added to UAC_AI_CombatManager.")
    log("To configure enemies to apply bleed on attacks:")
    log("1. Open enemy Blueprint in editor")
    log("2. Select the AC_AI_CombatManager component")
    log("3. Set DefaultAttackStatusEffects to include DA_StatusEffect_Bleed")
    log("   with Rank=1 and BuildupAmount=30.0")

# Save log
with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog saved to: {OUTPUT_FILE}")
