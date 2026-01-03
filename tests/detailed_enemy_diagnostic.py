# detailed_enemy_diagnostic.py
# Comprehensive diagnostic test for enemy characters and AI systems

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/detailed_enemy_diagnostic_output.txt"

def run():
    results = []
    errors = []
    warnings = []

    def log(msg):
        results.append(msg)

    def section(title):
        log("")
        log("=" * 70)
        log(title)
        log("=" * 70)

    section("DETAILED ENEMY DIAGNOSTIC")
    log("Testing all enemy characters, AI, and combat systems")

    # =========================================================================
    # 1. ENEMY CHARACTER BLUEPRINTS
    # =========================================================================
    section("1. ENEMY CHARACTER BLUEPRINTS")

    enemy_blueprints = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
    ]

    for bp_path in enemy_blueprints:
        log("")
        bp_name = bp_path.split("/")[-1]
        log(f"  [{bp_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            errors.append(f"Could not load {bp_name}")
            log(f"    FAILED: Could not load")
            continue

        log(f"    Loaded: OK")

        # Check parent class
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"    Parent: {parent}")

        # Check if parent is C++
        if "/Script/SLFConversion" in str(parent):
            log(f"    OK: Has C++ parent")
        else:
            warnings.append(f"{bp_name} parent is not C++: {parent}")
            log(f"    NOTE: Parent is Blueprint, not C++")

        # Get CDO and check components
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    comps = cdo.get_components_by_class(unreal.ActorComponent)
                    log(f"    Components: {len(comps)}")

                    # Look for key AI/Combat components
                    comp_names = [c.get_class().get_name() for c in comps]
                    ai_comps = [n for n in comp_names if "AI" in n or "Combat" in n or "Behavior" in n]
                    if ai_comps:
                        for c in ai_comps:
                            log(f"      - {c}")
                except Exception as e:
                    log(f"    Component error: {e}")

    # =========================================================================
    # 2. ENEMY BASE CLASS CHECK
    # =========================================================================
    section("2. ENEMY BASE CLASS (B_Soulslike_Enemy)")

    enemy_base_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Enemy"
    enemy_base = unreal.EditorAssetLibrary.load_asset(enemy_base_path)

    if enemy_base:
        log("  Loaded: B_Soulslike_Enemy")
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(enemy_base)
        log(f"  Parent: {parent}")

        gen_class = enemy_base.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    comps = cdo.get_components_by_class(unreal.ActorComponent)
                    log(f"  Components: {len(comps)}")
                    for comp in comps:
                        log(f"    - {comp.get_name()} ({comp.get_class().get_name()})")
                except:
                    pass
    else:
        log("  Not found - checking alternate paths...")
        # Try alternate paths
        alt_paths = [
            "/Game/SoulslikeFramework/Blueprints/_BaseClasses/B_Soulslike_Enemy",
            "/Game/SoulslikeFramework/Blueprints/_Characters/B_Enemy",
        ]
        for alt in alt_paths:
            if unreal.EditorAssetLibrary.does_asset_exist(alt):
                log(f"  Found at: {alt}")

    # =========================================================================
    # 3. NPC CHARACTER CHECK
    # =========================================================================
    section("3. NPC CHARACTERS")

    npc_blueprints = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
    ]

    for bp_path in npc_blueprints:
        bp_name = bp_path.split("/")[-1]
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if bp:
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
            log(f"  {bp_name}")
            log(f"    Parent: {parent}")
        else:
            log(f"  {bp_name}: NOT FOUND")

    # =========================================================================
    # 4. AI CONTROLLER CHECK
    # =========================================================================
    section("4. AI CONTROLLER")

    aic_path = "/Game/SoulslikeFramework/Blueprints/_AI/Misc/AIC_SoulslikeFramework"
    aic = unreal.EditorAssetLibrary.load_asset(aic_path)

    if aic:
        log("  Loaded: AIC_SoulslikeFramework")
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(aic)
        log(f"  Parent: {parent}")

        gen_class = aic.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Check for blackboard and behavior tree
                try:
                    bb = cdo.get_editor_property("Blackboard")
                    if bb:
                        log(f"  Blackboard: {bb.get_name()}")
                except:
                    pass
    else:
        errors.append("Could not load AI Controller")
        log("  FAILED: Could not load")

    # =========================================================================
    # 5. BEHAVIOR TREE CHECK
    # =========================================================================
    section("5. BEHAVIOR TREE")

    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"
    bt = unreal.EditorAssetLibrary.load_asset(bt_path)

    if bt:
        log("  Loaded: BT_Enemy")
        log(f"  Class: {bt.get_class().get_name()}")
    else:
        errors.append("Could not load Behavior Tree")
        log("  FAILED: Could not load")

    # Check subtrees
    log("")
    log("  Subtrees:")
    subtree_paths = [
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Investigating",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PatrolPath",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_PoiseBroken",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam",
    ]

    for st_path in subtree_paths:
        st_name = st_path.split("/")[-1]
        if unreal.EditorAssetLibrary.does_asset_exist(st_path):
            log(f"    OK: {st_name}")
        else:
            log(f"    MISSING: {st_name}")

    # =========================================================================
    # 6. BLACKBOARD CHECK
    # =========================================================================
    section("6. BLACKBOARD")

    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"
    bb = unreal.EditorAssetLibrary.load_asset(bb_path)

    if bb:
        log("  Loaded: BB_Standard")
        log(f"  Class: {bb.get_class().get_name()}")

        # Try to get keys
        try:
            keys = bb.get_editor_property("Keys")
            if keys:
                log(f"  Keys: {len(keys)}")
                for key in keys[:10]:
                    log(f"    - {key.get_editor_property('EntryName')}")
                if len(keys) > 10:
                    log(f"    ... and {len(keys) - 10} more")
        except Exception as e:
            log(f"  Could not read keys: {e}")
    else:
        errors.append("Could not load Blackboard")
        log("  FAILED: Could not load")

    # =========================================================================
    # 7. AI COMPONENT CHECK
    # =========================================================================
    section("7. AI COMPONENTS")

    ai_components = [
        ("/Game/SoulslikeFramework/Blueprints/Components/AC_AI_BehaviorManager", "AI Behavior Manager"),
        ("/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager", "AI Combat Manager"),
        ("/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager", "AI Interaction Manager"),
        ("/Game/SoulslikeFramework/Blueprints/Components/AC_AI_Boss", "AI Boss"),
    ]

    for comp_path, desc in ai_components:
        comp_name = comp_path.split("/")[-1]
        comp = unreal.EditorAssetLibrary.load_asset(comp_path)

        if comp:
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(comp)
            log(f"  {comp_name}")
            log(f"    Parent: {parent}")

            if "/Script/SLFConversion" in str(parent):
                log(f"    OK: Has C++ parent")
            else:
                warnings.append(f"{comp_name} not parented to C++")
        else:
            log(f"  {comp_name}: NOT FOUND")

    # =========================================================================
    # 8. AI ABILITIES CHECK
    # =========================================================================
    section("8. AI ABILITIES (Attack Data)")

    ability_folder = "/Game/SoulslikeFramework/Data/AI_Abilities"

    # Check for abilities
    log("  Enemy Abilities:")
    enemy_abilities = [
        "/Game/SoulslikeFramework/Data/AI_Abilities/DA_AI_Ability_Enemy_Attack01",
        "/Game/SoulslikeFramework/Data/AI_Abilities/DA_AI_Ability_Enemy_Attack02",
        "/Game/SoulslikeFramework/Data/AI_Abilities/DA_AI_Ability_Enemy_Attack03_Fast",
    ]

    for ab_path in enemy_abilities:
        ab_name = ab_path.split("/")[-1]
        if unreal.EditorAssetLibrary.does_asset_exist(ab_path):
            log(f"    OK: {ab_name}")
        else:
            log(f"    MISSING: {ab_name}")

    log("")
    log("  Boss Abilities:")
    boss_abilities = [
        "/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_FourSwings",
        "/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_JumpAttackClose",
        "/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_Ranged_Fireball",
    ]

    for ab_path in boss_abilities:
        ab_name = ab_path.split("/")[-1]
        if unreal.EditorAssetLibrary.does_asset_exist(ab_path):
            log(f"    OK: {ab_name}")
        else:
            log(f"    MISSING: {ab_name}")

    # =========================================================================
    # 9. ENEMY ANIMATION BLUEPRINTS
    # =========================================================================
    section("9. ENEMY ANIMATION BLUEPRINTS")

    enemy_abps = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    ]

    for abp_path in enemy_abps:
        abp_name = abp_path.split("/")[-1]
        abp = unreal.EditorAssetLibrary.load_asset(abp_path)

        if abp:
            log(f"  {abp_name}")
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(abp)
            log(f"    Parent: {parent}")
        else:
            log(f"  {abp_name}: NOT FOUND")

    # =========================================================================
    # 10. LEVEL ENEMY INSTANCES
    # =========================================================================
    section("10. ENEMIES IN L_Demo_Showcase")

    # Load the level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    try:
        unreal.EditorLevelLibrary.load_level(level_path)
        subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
        world = subsystem.get_editor_world()

        if world:
            log(f"  Level: {world.get_name()}")

            # Find all characters
            characters = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Character)
            log(f"  Total Characters: {len(characters)}")

            for char in characters:
                char_name = char.get_name()
                class_name = char.get_class().get_name()
                loc = char.get_actor_location()

                log(f"")
                log(f"  [{char_name}]")
                log(f"    Class: {class_name}")
                log(f"    Location: ({loc.x:.0f}, {loc.y:.0f}, {loc.z:.0f})")

                # Check AI Controller
                try:
                    controller = char.get_controller()
                    if controller:
                        log(f"    Controller: {controller.get_class().get_name()}")
                    else:
                        log(f"    Controller: None")
                except:
                    pass

                # Check for combat/AI components
                try:
                    comps = char.get_components_by_class(unreal.ActorComponent)
                    ai_comps = [c for c in comps if "AI" in c.get_class().get_name() or "Combat" in c.get_class().get_name()]
                    if ai_comps:
                        log(f"    AI/Combat Components:")
                        for c in ai_comps:
                            log(f"      - {c.get_class().get_name()}")
                except:
                    pass

    except Exception as e:
        log(f"  Error loading level: {e}")

    # =========================================================================
    # 11. C++ AI CLASSES
    # =========================================================================
    section("11. C++ AI CLASSES")

    cpp_ai_classes = [
        "AIBehaviorManagerComponent",
        "AICombatManagerComponent",
        "AIInteractionManagerComponent",
        "AIBossComponent",
        "SLFAIController",
    ]

    for cls_name in cpp_ai_classes:
        try:
            # Try to find class
            cls = unreal.find_class(cls_name)
            if cls:
                log(f"  OK: {cls_name}")
            else:
                log(f"  Checking: {cls_name}")
        except:
            log(f"  Checking: {cls_name}")

    # =========================================================================
    # SUMMARY
    # =========================================================================
    section("DIAGNOSTIC SUMMARY")

    log("")
    log(f"Errors: {len(errors)}")
    log(f"Warnings: {len(warnings)}")

    if errors:
        log("")
        log("ERRORS:")
        for err in errors:
            log(f"  X {err}")

    if warnings:
        log("")
        log("WARNINGS:")
        for warn in warnings:
            log(f"  ! {warn}")

    log("")
    if not errors:
        log("VERDICT: Enemy systems appear to be configured")
        log("")
        log("Enemy AI Flow:")
        log("  1. Enemy spawns in level")
        log("  2. AI Controller (AIC_SoulslikeFramework) possesses enemy")
        log("  3. Behavior Tree (BT_Enemy) starts running")
        log("  4. Blackboard (BB_Standard) stores AI state")
        log("  5. AI Combat Manager handles attack selection")
        log("  6. AI Behavior Manager handles state transitions")
        log("")
        log("To test enemy AI:")
        log("  1. Open Unreal Editor")
        log("  2. Open L_Demo_Showcase")
        log("  3. Press Play")
        log("  4. Approach enemies - they should detect and attack")
    else:
        log("VERDICT: Some enemy systems have issues")

    # Write output
    output = "\n".join(results)
    with open(OUTPUT_FILE, 'w') as f:
        f.write(output)

    unreal.log_warning(output)

run()
