"""
Configure demo enemies with default attack status effects.
Uses CDO approach to set DefaultAttackStatusEffects.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/enemy_status_effect_config.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

log("=" * 70)
log("CONFIGURING ENEMY DEFAULT ATTACK STATUS EFFECTS (v2)")
log("=" * 70)

# Status effect data assets
BLEED_PATH = "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed"
BURN_PATH = "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn"

# Load status effects
bleed_effect = unreal.EditorAssetLibrary.load_asset(BLEED_PATH)
burn_effect = unreal.EditorAssetLibrary.load_asset(BURN_PATH)

if not bleed_effect:
    log(f"ERROR: Could not load bleed effect from {BLEED_PATH}")
if not burn_effect:
    log(f"ERROR: Could not load burn effect from {BURN_PATH}")

log(f"\nLoaded status effects:")
log(f"  Bleed: {bleed_effect.get_name() if bleed_effect else 'NOT FOUND'}")
log(f"  Burn: {burn_effect.get_name() if burn_effect else 'NOT FOUND'}")

# Enemy Blueprints to configure
ENEMY_CONFIG = {
    # Regular enemies get bleed
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy": {
        "effect": bleed_effect,
        "rank": 1,
        "buildup": 30.0
    },
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard": {
        "effect": bleed_effect,
        "rank": 1,
        "buildup": 40.0  # Guard hits harder
    },
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase": {
        "effect": bleed_effect,
        "rank": 1,
        "buildup": 30.0
    },
}

success_count = 0
for bp_path, config in ENEMY_CONFIG.items():
    log(f"\n--- {bp_path.split('/')[-1]} ---")

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
            log(f"  ERROR: No CDO")
            continue

        log(f"  Got CDO: {cdo.get_name()}")

        # List all ActorComponents on the CDO to find the combat manager
        actor_comps = cdo.get_components_by_class(unreal.ActorComponent)
        log(f"  Components on CDO: {len(actor_comps)}")

        found_combat_manager = None
        for comp in actor_comps:
            comp_class = comp.get_class()
            comp_name = comp_class.get_name()
            log(f"    - {comp_name}")
            if "AI_CombatManager" in comp_name or "AICombatManager" in comp_name:
                found_combat_manager = comp
                break

        if found_combat_manager:
            log(f"  Found AI Combat Manager: {found_combat_manager.get_name()}")

            # Try to set the property
            try:
                effect = config["effect"]
                if effect:
                    # Check if we can access the property
                    current = found_combat_manager.get_editor_property("default_attack_status_effects")
                    log(f"    Current: {current}")

                    # Create FSLFStatusEffectApplication
                    app = unreal.SLFStatusEffectApplication()
                    app.rank = config["rank"]
                    app.buildup_amount = config["buildup"]

                    # Set the new map
                    new_map = {effect: app}
                    found_combat_manager.set_editor_property("default_attack_status_effects", new_map)
                    log(f"    Set to: {effect.get_name()} (Rank={config['rank']}, Buildup={config['buildup']})")

                    # Verify
                    verify = found_combat_manager.get_editor_property("default_attack_status_effects")
                    log(f"    Verified: {verify}")

                    # Save
                    unreal.EditorAssetLibrary.save_asset(bp_path)
                    log(f"  SAVED")
                    success_count += 1
            except Exception as e:
                log(f"    Error: {e}")
                import traceback
                log(f"    {traceback.format_exc()}")
        else:
            log(f"  WARNING: No AI Combat Manager found")
            log(f"  Trying to find via class iteration...")

            # Try a different approach - check if the character class has a CombatManager property
            try:
                # Check for component by name
                for comp in actor_comps:
                    comp_type = type(comp).__name__
                    log(f"    Type: {comp_type}")
            except Exception as e:
                log(f"    {e}")

    except Exception as e:
        log(f"  ERROR: {e}")
        import traceback
        log(f"  {traceback.format_exc()}")

log(f"\n" + "=" * 70)
log(f"RESULTS: {success_count}/{len(ENEMY_CONFIG)} configured successfully")
log("=" * 70)

# Save log
with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog saved to: {OUTPUT_FILE}")
