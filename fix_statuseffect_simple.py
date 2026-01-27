"""
Simple fix for B_StatusEffectArea - just check properties and apply config.
"""

import unreal

LOG_FILE = "C:/scripts/SLFConversion/migration_cache/statuseffect_fix_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

def fix():
    bp_path = "/Game/SoulslikeFramework/Data/StatusEffects/B_StatusEffectArea"
    cpp_class_path = "/Script/SLFConversion.B_StatusEffectArea"
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    log("=" * 60)
    log("B_StatusEffectArea Fix")
    log("=" * 60)

    # Load C++ class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if cpp_class:
        log(f"C++ class: {cpp_class.get_name()}")

        # Get CDO and check properties
        cdo = unreal.get_default_object(cpp_class)
        if cdo:
            log(f"C++ CDO: {cdo.get_name()}")
            for prop in ["status_effect_to_apply", "effect_rank"]:
                try:
                    val = cdo.get_editor_property(prop)
                    log(f"  CDO.{prop} = {val}")
                except Exception as e:
                    log(f"  CDO.{prop} FAILED: {e}")
    else:
        log(f"ERROR: C++ class not found: {cpp_class_path}")
        return

    # Load Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if bp:
        log(f"\nBlueprint: {bp.get_name()}")

        # Try to reparent
        if cpp_class:
            log("Attempting reparent to C++...")
            try:
                result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
                log(f"Reparent result: {result}")
                unreal.BlueprintEditorLibrary.compile_blueprint(bp)
                log("Compiled")
                unreal.EditorAssetLibrary.save_asset(bp_path)
                log("Saved")
            except Exception as e:
                log(f"Reparent error: {e}")

        # Get generated class and check properties
        gen_class = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
        if gen_class:
            log(f"\nGenerated class: {gen_class.get_name()}")
            gen_cdo = unreal.get_default_object(gen_class)
            if gen_cdo:
                log("Generated CDO properties:")
                for prop in ["status_effect_to_apply", "effect_rank"]:
                    try:
                        val = gen_cdo.get_editor_property(prop)
                        log(f"  {prop} = {val}")
                    except Exception as e:
                        log(f"  {prop} FAILED: {e}")

    # Load level
    log(f"\nLoading level: {level_path}")
    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if level_subsystem:
        level_subsystem.load_level(level_path)

    # Find status effect area actors
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()

    status_actors = []
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "StatusEffectArea" in class_name:
            status_actors.append(actor)

    log(f"\nFound {len(status_actors)} B_StatusEffectArea instances")

    # Status effects to assign
    STATUS_EFFECTS = [
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Frostbite",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Corruption",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Madness",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Plague",
    ]

    # Sort by X position
    sorted_actors = sorted(status_actors, key=lambda a: a.get_actor_location().x)

    # Check first actor in detail
    if sorted_actors:
        actor = sorted_actors[0]
        log(f"\nFirst actor: {actor.get_name()}")
        log(f"Class: {actor.get_class().get_name()}")
        log(f"Class path: {actor.get_class().get_path_name()}")

        # List ALL properties available
        log("\nAll properties (filtered):")
        for attr in sorted(dir(actor)):
            if not attr.startswith('_') and not attr.startswith('set_') and not attr.startswith('get_'):
                if any(kw in attr.lower() for kw in ['effect', 'status', 'rank', 'apply', 'buildup']):
                    log(f"  {attr}")

        # Try multiple property names
        log("\nTrying property access:")
        test_props = [
            "status_effect_to_apply", "StatusEffectToApply",
            "effect_rank", "EffectRank",
            "status_effect_tag", "StatusEffectTag",
            "area_effect", "AreaEffect",
        ]
        for prop in test_props:
            try:
                val = actor.get_editor_property(prop)
                log(f"  {prop} = {val}")
            except Exception as e:
                log(f"  {prop} = FAILED")

    # Try to apply configuration
    modified = 0
    for idx, actor in enumerate(sorted_actors):
        if idx >= len(STATUS_EFFECTS):
            break

        effect_path = STATUS_EFFECTS[idx]
        effect_asset = unreal.EditorAssetLibrary.load_asset(effect_path)

        if not effect_asset:
            log(f"Could not load: {effect_path}")
            continue

        log(f"\n{actor.get_name()}: Trying to set {effect_asset.get_name()}")

        # Try different property names
        for prop_name in ["status_effect_to_apply", "StatusEffectToApply"]:
            try:
                actor.set_editor_property(prop_name, effect_asset)
                log(f"  SUCCESS: {prop_name}")
                modified += 1
                break
            except Exception as e:
                log(f"  {prop_name}: {e}")

    log(f"\n\nModified: {modified}")

    # Save level if modified
    if modified > 0 and level_subsystem:
        result = level_subsystem.save_current_level()
        log(f"Level save: {result}")

# Run
fix()

# Save log
with open(LOG_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog: {LOG_FILE}")
