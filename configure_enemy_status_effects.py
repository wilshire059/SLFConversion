"""
Configure demo enemies with default attack status effects.
This sets the DefaultAttackStatusEffects on AC_AI_CombatManager components.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/enemy_status_effect_config.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

log("=" * 70)
log("CONFIGURING ENEMY DEFAULT ATTACK STATUS EFFECTS")
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

        # We need to find the AC_AI_CombatManager in the Blueprint's SCS
        # and configure its DefaultAttackStatusEffects

        # Get the SimpleConstructionScript
        scs = bp.simple_construction_script
        if not scs:
            log(f"  ERROR: No SCS found")
            continue

        # Find AC_AI_CombatManager component node
        found_combat_manager = False
        all_nodes = scs.get_all_nodes()

        for node in all_nodes:
            if not node:
                continue

            comp_template = node.component_template
            if not comp_template:
                continue

            comp_class_name = comp_template.get_class().get_name()

            if "CombatManager" in comp_class_name or "AI_Combat" in comp_class_name:
                log(f"  Found component: {comp_class_name}")

                # Try to set DefaultAttackStatusEffects
                try:
                    # Create the status effect application struct
                    # FSLFStatusEffectApplication has Rank (int32) and BuildupAmount (double)

                    # Get current map
                    current_map = comp_template.get_editor_property("default_attack_status_effects")
                    log(f"    Current DefaultAttackStatusEffects: {current_map}")

                    # We need to create a new map entry
                    # The key is UPrimaryDataAsset*, value is FSLFStatusEffectApplication
                    effect = config["effect"]
                    if effect:
                        # Create FSLFStatusEffectApplication struct
                        app = unreal.SLFStatusEffectApplication()
                        app.set_editor_property("rank", config["rank"])
                        app.set_editor_property("buildup_amount", config["buildup"])

                        # Create new map with this entry
                        new_map = {effect: app}

                        comp_template.set_editor_property("default_attack_status_effects", new_map)
                        log(f"    Set DefaultAttackStatusEffects: {effect.get_name()} (Rank={config['rank']}, Buildup={config['buildup']})")
                        found_combat_manager = True

                except Exception as e:
                    log(f"    Error setting property: {e}")

        if not found_combat_manager:
            log(f"  WARNING: No AI Combat Manager component found in SCS")
            log(f"  Trying to find in CDO components...")

            # Try getting from CDO
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # List all components on the CDO
                try:
                    components = cdo.get_components_by_class(unreal.ActorComponent)
                    log(f"  CDO has {len(components)} components")
                    for comp in components:
                        comp_name = comp.get_class().get_name()
                        if "CombatManager" in comp_name:
                            log(f"    Found in CDO: {comp_name}")
                except Exception as e:
                    log(f"    Error listing components: {e}")

        if found_combat_manager:
            # Save the Blueprint
            unreal.EditorAssetLibrary.save_asset(bp_path)
            log(f"  SAVED Blueprint")
            success_count += 1

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
