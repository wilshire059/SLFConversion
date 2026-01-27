"""
Complete diagnosis and fix for B_StatusEffectArea.

This script:
1. Checks the actual Blueprint parent class
2. Forces reparent to C++ class
3. Uses SLFAutomationLibrary to clear Blueprint variables
4. Resaves everything
5. Applies status effect configuration to level instances
"""

import unreal

LOG_FILE = "C:/scripts/SLFConversion/migration_cache/statuseffect_diagnosis.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

def diagnose_and_fix():
    """Complete diagnosis and fix."""

    bp_path = "/Game/SoulslikeFramework/Data/StatusEffects/B_StatusEffectArea"
    cpp_class_path = "/Script/SLFConversion.B_StatusEffectArea"
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    log("=" * 60)
    log("B_StatusEffectArea - Complete Diagnosis and Fix")
    log("=" * 60)

    # =========================================================================
    # Step 1: Check Blueprint's current state
    # =========================================================================
    log("\n=== STEP 1: Current Blueprint State ===")

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"FATAL: Cannot load Blueprint: {bp_path}")
        return False

    log(f"Blueprint loaded: {bp.get_name()}")
    log(f"Blueprint type: {type(bp)}")

    # Get the generated class
    gen_class = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
    if gen_class:
        log(f"Generated class: {gen_class.get_name()}")
        log(f"Generated class path: {gen_class.get_path_name()}")

        # Walk the class hierarchy
        log("\nClass hierarchy:")
        current = gen_class
        depth = 0
        while current:
            prefix = "  " * depth
            log(f"{prefix}- {current.get_name()} ({current.get_path_name()})")
            # Try different methods to get parent class
            try:
                current = current.static_class().get_super_class()
            except:
                try:
                    # Alternative: use parent_class property for Blueprints
                    current = getattr(current, 'parent_class', None)
                except:
                    current = None
            depth += 1
            if depth > 10:
                break
    else:
        log("ERROR: Could not get generated class!")

    # =========================================================================
    # Step 2: Check C++ class exists
    # =========================================================================
    log("\n=== STEP 2: C++ Class Check ===")

    cpp_class = unreal.load_class(None, cpp_class_path)
    if cpp_class:
        log(f"C++ class found: {cpp_class.get_name()}")
        log(f"C++ class path: {cpp_class.get_path_name()}")

        # Get CDO
        cdo = unreal.get_default_object(cpp_class)
        if cdo:
            log(f"C++ CDO: {cdo.get_name()}")

            # Check CDO properties
            for prop in ["status_effect_to_apply", "effect_rank"]:
                try:
                    val = cdo.get_editor_property(prop)
                    log(f"  CDO.{prop} = {val}")
                except Exception as e:
                    log(f"  CDO.{prop} = FAILED ({e})")
    else:
        log(f"ERROR: C++ class NOT found: {cpp_class_path}")

    # =========================================================================
    # Step 3: Force reparent Blueprint to C++
    # =========================================================================
    log("\n=== STEP 3: Force Reparent to C++ ===")

    if cpp_class:
        try:
            # Use BlueprintEditorLibrary to reparent
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            log(f"Reparent result: {result}")

            # Compile the Blueprint
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            log("Blueprint compiled")

            # Save the Blueprint
            unreal.EditorAssetLibrary.save_asset(bp_path)
            log("Blueprint saved")

        except Exception as e:
            log(f"Reparent error: {e}")
    else:
        log("Skipping reparent - no C++ class")

    # =========================================================================
    # Step 4: Reload and verify
    # =========================================================================
    log("\n=== STEP 4: Verify After Reparent ===")

    # Reload the Blueprint
    unreal.EditorAssetLibrary.unload_asset(bp_path)
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    gen_class = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
    if gen_class:
        log(f"Generated class after reparent: {gen_class.get_name()}")

        # Walk hierarchy again
        log("\nClass hierarchy after reparent:")
        current = gen_class
        depth = 0
        while current:
            prefix = "  " * depth
            log(f"{prefix}- {current.get_name()} ({current.get_path_name()})")
            current = current.get_super_class()
            depth += 1
            if depth > 10:
                break

        # Test CDO property access
        gen_cdo = unreal.get_default_object(gen_class)
        if gen_cdo:
            log("\nBlueprint CDO property access:")
            for prop in ["status_effect_to_apply", "effect_rank"]:
                try:
                    val = gen_cdo.get_editor_property(prop)
                    log(f"  CDO.{prop} = {val}")
                except Exception as e:
                    log(f"  CDO.{prop} = FAILED ({e})")

    # =========================================================================
    # Step 5: Load level and check instances
    # =========================================================================
    log("\n=== STEP 5: Level Instance Check ===")

    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if level_subsystem:
        level_subsystem.load_level(level_path)
        log(f"Loaded level: {level_path}")

    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()

    status_effect_actors = []
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "StatusEffectArea" in class_name:
            status_effect_actors.append(actor)

    log(f"\nFound {len(status_effect_actors)} B_StatusEffectArea instances")

    for i, actor in enumerate(status_effect_actors[:3]):  # Check first 3
        log(f"\nInstance {i+1}: {actor.get_name()}")
        log(f"  Class: {actor.get_class().get_name()}")
        log(f"  Location: X={actor.get_actor_location().x:.0f}")

        # Test property access
        for prop in ["status_effect_to_apply", "effect_rank"]:
            try:
                val = actor.get_editor_property(prop)
                log(f"  {prop}: {val}")
            except Exception as e:
                log(f"  {prop}: FAILED - {e}")

    # =========================================================================
    # Step 6: If properties work, apply configuration
    # =========================================================================
    log("\n=== STEP 6: Apply Status Effect Configuration ===")

    # Status effects to assign (sorted by X position)
    STATUS_EFFECTS = [
        ("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison", 10091),
        ("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed", 10391),
        ("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn", 10691),
        ("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Frostbite", 10991),
        ("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Corruption", 11291),
        ("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Madness", 11591),
        ("/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Plague", 11891),
    ]

    # Sort instances by X position
    sorted_actors = sorted(status_effect_actors, key=lambda a: a.get_actor_location().x)

    modified = 0
    for idx, actor in enumerate(sorted_actors):
        if idx >= len(STATUS_EFFECTS):
            break

        effect_path, expected_x = STATUS_EFFECTS[idx]
        actor_x = actor.get_actor_location().x

        log(f"\n{actor.get_name()} (X={actor_x:.0f}):")

        # Load status effect asset
        effect_asset = unreal.EditorAssetLibrary.load_asset(effect_path)
        if not effect_asset:
            log(f"  ERROR: Could not load {effect_path}")
            continue

        # Try to set the property
        try:
            actor.set_editor_property("status_effect_to_apply", effect_asset)
            actor.set_editor_property("effect_rank", 1)
            log(f"  Set status_effect_to_apply = {effect_asset.get_name()}")
            log(f"  Set effect_rank = 1")
            modified += 1
        except Exception as e:
            log(f"  ERROR: {e}")

    log(f"\nModified {modified}/{len(sorted_actors)} instances")

    # Save the level
    if modified > 0 and level_subsystem:
        result = level_subsystem.save_current_level()
        log(f"Level save result: {result}")

    return True

# Run
diagnose_and_fix()

# Write log
with open(LOG_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nLog saved to: {LOG_FILE}")
