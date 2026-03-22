# detailed_character_diagnostic.py
# Comprehensive diagnostic test for character movement and systems

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/detailed_diagnostic_output.txt"

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

    section("DETAILED CHARACTER DIAGNOSTIC")
    log("Testing all systems required for character movement in PIE")

    # =========================================================================
    # 1. CHARACTER BLUEPRINT ANALYSIS
    # =========================================================================
    section("1. CHARACTER BLUEPRINT ANALYSIS")

    char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    char_bp = unreal.EditorAssetLibrary.load_asset(char_path)

    if not char_bp:
        errors.append("CRITICAL: Could not load B_Soulslike_Character")
        log("  FAILED: Could not load character Blueprint")
    else:
        log("  Loaded: B_Soulslike_Character")

        gen_class = char_bp.generated_class()
        if gen_class:
            log(f"  Generated Class: {gen_class.get_name()}")

            # Parent class chain
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(char_bp)
            log(f"  Parent Class: {parent}")

            if "SLFSoulslikeCharacter" in str(parent):
                log("  OK: Correctly parented to C++ SLFSoulslikeCharacter")
            else:
                errors.append(f"Parent class is {parent}, expected SLFSoulslikeCharacter")

    # =========================================================================
    # 2. INPUT ACTION CONFIGURATION (ALL ACTIONS)
    # =========================================================================
    section("2. INPUT ACTION CONFIGURATION")

    if char_bp:
        gen_class = char_bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                input_actions = [
                    ("PlayerMappingContext", "Input Mapping Context"),
                    ("IA_Move", "Movement Input"),
                    ("IA_Look", "Camera Look Input"),
                    ("IA_Jump", "Jump Input"),
                    ("IA_Sprint", "Sprint Input"),
                    ("IA_Dodge", "Dodge Input"),
                    ("IA_Attack", "Attack Input"),
                    ("IA_Guard", "Guard/Block Input"),
                    ("IA_Interact", "Interact Input"),
                    ("IA_TargetLock", "Target Lock Input"),
                ]

                for prop_name, desc in input_actions:
                    try:
                        val = cdo.get_editor_property(prop_name)
                        if val:
                            log(f"  OK: {prop_name} = {val.get_name()}")
                        else:
                            if prop_name in ["IA_Move", "IA_Look", "PlayerMappingContext"]:
                                errors.append(f"{prop_name} ({desc}) is None - CRITICAL for movement!")
                            else:
                                warnings.append(f"{prop_name} ({desc}) is None")
                            log(f"  MISSING: {prop_name} ({desc})")
                    except Exception as e:
                        log(f"  ERROR: {prop_name} - {e}")

    # =========================================================================
    # 3. COMPONENT ANALYSIS
    # =========================================================================
    section("3. COMPONENT ANALYSIS")

    if char_bp:
        gen_class = char_bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Get all components
                try:
                    all_comps = cdo.get_components_by_class(unreal.ActorComponent)
                    log(f"  Total Components: {len(all_comps)}")
                    log("")

                    # Categorize components
                    movement_comps = []
                    input_comps = []
                    combat_comps = []
                    camera_comps = []
                    mesh_comps = []
                    other_comps = []

                    for comp in all_comps:
                        name = comp.get_name()
                        class_name = comp.get_class().get_name()

                        if "Movement" in class_name or "Movement" in name:
                            movement_comps.append((name, class_name))
                        elif "Input" in class_name or "Input" in name or "Buffer" in name:
                            input_comps.append((name, class_name))
                        elif "Combat" in class_name or "Combat" in name or "Action" in name:
                            combat_comps.append((name, class_name))
                        elif "Camera" in class_name or "Camera" in name or "Spring" in name:
                            camera_comps.append((name, class_name))
                        elif "Mesh" in class_name or "Skeletal" in class_name:
                            mesh_comps.append((name, class_name))
                        else:
                            other_comps.append((name, class_name))

                    log("  [Movement Components]")
                    if movement_comps:
                        for name, cls in movement_comps:
                            log(f"    - {name} ({cls})")
                    else:
                        log("    (inherited from ACharacter)")

                    log("")
                    log("  [Input/Buffer Components]")
                    if input_comps:
                        for name, cls in input_comps:
                            log(f"    - {name} ({cls})")
                    else:
                        warnings.append("No InputBuffer component found in hierarchy")
                        log("    None found - may be in parent Blueprint")

                    log("")
                    log("  [Combat Components]")
                    if combat_comps:
                        for name, cls in combat_comps:
                            log(f"    - {name} ({cls})")
                    else:
                        log("    None found - may be in parent Blueprint")

                    log("")
                    log("  [Camera Components]")
                    if camera_comps:
                        for name, cls in camera_comps:
                            log(f"    - {name} ({cls})")
                    else:
                        warnings.append("No camera components found")
                        log("    None found - camera may not work")

                    log("")
                    log("  [Mesh Components]")
                    if mesh_comps:
                        for name, cls in mesh_comps:
                            log(f"    - {name} ({cls})")

                    log("")
                    log("  [Other Components]")
                    for name, cls in other_comps:
                        log(f"    - {name} ({cls})")

                except Exception as e:
                    log(f"  ERROR getting components: {e}")

    # =========================================================================
    # 4. BASE CHARACTER CHECK
    # =========================================================================
    section("4. BASE CHARACTER CHECK")

    base_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter"
    base_bp = unreal.EditorAssetLibrary.load_asset(base_path)

    if base_bp:
        log("  Loaded: B_BaseCharacter")
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(base_bp)
        log(f"  Parent Class: {parent}")

        if "SLFBaseCharacter" in str(parent):
            log("  OK: Correctly parented to C++ SLFBaseCharacter")
        else:
            errors.append(f"B_BaseCharacter parent is {parent}, expected SLFBaseCharacter")

        # Check components in base
        gen_class = base_bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    comps = cdo.get_components_by_class(unreal.ActorComponent)
                    log(f"  Components in B_BaseCharacter: {len(comps)}")
                    for comp in comps:
                        log(f"    - {comp.get_name()} ({comp.get_class().get_name()})")
                except:
                    pass
    else:
        errors.append("Could not load B_BaseCharacter")

    # =========================================================================
    # 5. GAME MODE CONFIGURATION
    # =========================================================================
    section("5. GAME MODE CONFIGURATION")

    gm_path = "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework"
    gm = unreal.EditorAssetLibrary.load_asset(gm_path)

    if gm:
        log("  Loaded: GM_SoulslikeFramework")
        gen_class = gm.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                props = [
                    "DefaultPawnClass",
                    "PlayerControllerClass",
                    "HUDClass",
                    "PlayerStateClass",
                    "GameStateClass",
                ]
                for prop in props:
                    try:
                        val = cdo.get_editor_property(prop)
                        if val:
                            log(f"  {prop}: {val.get_name()}")
                        else:
                            if prop in ["DefaultPawnClass", "PlayerControllerClass"]:
                                errors.append(f"GameMode {prop} is None")
                            log(f"  {prop}: None")
                    except Exception as e:
                        log(f"  {prop}: Error - {e}")
    else:
        errors.append("Could not load GameMode")

    # =========================================================================
    # 6. PLAYER CONTROLLER CONFIGURATION
    # =========================================================================
    section("6. PLAYER CONTROLLER CONFIGURATION")

    pc_path = "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework"
    pc = unreal.EditorAssetLibrary.load_asset(pc_path)

    if pc:
        log("  Loaded: PC_SoulslikeFramework")
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(pc)
        log(f"  Parent Class: {parent}")

        gen_class = pc.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    mapping_ctx = cdo.get_editor_property("GameplayMappingContext")
                    if mapping_ctx:
                        log(f"  GameplayMappingContext: {mapping_ctx.get_name()}")
                    else:
                        warnings.append("PlayerController GameplayMappingContext is None")
                except:
                    pass
    else:
        errors.append("Could not load PlayerController")

    # =========================================================================
    # 7. ANIMATION BLUEPRINT CHECK
    # =========================================================================
    section("7. ANIMATION BLUEPRINT CHECK")

    if char_bp:
        gen_class = char_bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    mesh = cdo.get_editor_property("Mesh")
                    if mesh:
                        log(f"  SkeletalMesh Component: {mesh.get_name()}")

                        skeletal_mesh = mesh.get_editor_property("SkeletalMesh")
                        if skeletal_mesh:
                            log(f"  Skeletal Mesh Asset: {skeletal_mesh.get_name()}")
                        else:
                            warnings.append("No skeletal mesh assigned")
                            log("  Skeletal Mesh Asset: None")

                        anim_class = mesh.get_editor_property("AnimClass")
                        if anim_class:
                            log(f"  Animation Blueprint: {anim_class.get_name()}")
                        else:
                            warnings.append("No AnimBlueprint assigned - character won't animate")
                            log("  Animation Blueprint: None")
                    else:
                        errors.append("No Mesh component found")
                except Exception as e:
                    log(f"  Error checking mesh: {e}")

    # =========================================================================
    # 8. INPUT MAPPING CONTEXT CONTENTS
    # =========================================================================
    section("8. INPUT MAPPING CONTEXT ANALYSIS")

    imc_path = "/Game/SoulslikeFramework/Input/IMC_Gameplay"
    imc = unreal.EditorAssetLibrary.load_asset(imc_path)

    if imc:
        log(f"  Loaded: {imc.get_name()}")
        log(f"  Class: {imc.get_class().get_name()}")

        # Try to get mappings
        try:
            mappings = imc.get_editor_property("Mappings")
            if mappings:
                log(f"  Mappings count: {len(mappings)}")
                for i, mapping in enumerate(mappings[:10]):  # First 10
                    try:
                        action = mapping.get_editor_property("Action")
                        if action:
                            log(f"    [{i}] {action.get_name()}")
                    except:
                        pass
                if len(mappings) > 10:
                    log(f"    ... and {len(mappings) - 10} more")
        except Exception as e:
            log(f"  Could not read mappings: {e}")
    else:
        errors.append("Could not load IMC_Gameplay")

    # =========================================================================
    # 9. LEVEL CHECK
    # =========================================================================
    section("9. AVAILABLE LEVELS")

    # Find gameplay levels
    level_paths = [
        "/Game/SoulslikeFramework/Demo/Maps/L_Showcase",
        "/Game/SoulslikeFramework/Demo/Maps/L_ShowcaseRoom",
        "/Game/SoulslikeFramework/Maps/L_Showcase",
    ]

    found_levels = []
    for path in level_paths:
        if unreal.EditorAssetLibrary.does_asset_exist(path):
            found_levels.append(path)
            log(f"  Found: {path}")

    if not found_levels:
        # Search for levels
        log("  Searching for levels...")
        try:
            all_assets = unreal.EditorAssetLibrary.list_assets("/Game/SoulslikeFramework/", recursive=True)
            maps = [a for a in all_assets if "Map" in a or "Level" in a or a.endswith("_L")]
            for m in maps[:10]:
                log(f"    {m}")
        except:
            pass

    # =========================================================================
    # 10. C++ CLASS VERIFICATION
    # =========================================================================
    section("10. C++ CLASS VERIFICATION")

    cpp_classes = [
        "/Script/SLFConversion.SLFBaseCharacter",
        "/Script/SLFConversion.SLFSoulslikeCharacter",
        "/Script/SLFConversion.InputBufferComponent",
        "/Script/SLFConversion.ActionManagerComponent",
        "/Script/SLFConversion.CombatManagerComponent",
        "/Script/SLFConversion.StatManagerComponent",
    ]

    for class_path in cpp_classes:
        try:
            cls = unreal.find_class(class_path.split(".")[-1])
            if cls:
                log(f"  OK: {class_path.split('.')[-1]}")
            else:
                log(f"  MISSING: {class_path.split('.')[-1]}")
        except:
            # Try loading via path
            short_name = class_path.split(".")[-1]
            log(f"  Checking: {short_name}")

    # =========================================================================
    # FINAL SUMMARY
    # =========================================================================
    section("DIAGNOSTIC SUMMARY")

    log("")
    log(f"Errors: {len(errors)}")
    log(f"Warnings: {len(warnings)}")
    log("")

    if errors:
        log("CRITICAL ERRORS (must fix before PIE):")
        for err in errors:
            log(f"  X {err}")
        log("")

    if warnings:
        log("WARNINGS (may affect gameplay):")
        for warn in warnings:
            log(f"  ! {warn}")
        log("")

    if not errors:
        log("=" * 70)
        log("VERDICT: READY FOR PIE TESTING")
        log("=" * 70)
        log("")
        log("The character should be able to:")
        log("  - Spawn in the level")
        log("  - Move with WASD (via C++ HandleMove)")
        log("  - Look with mouse (via C++ HandleLook)")
        log("  - Jump with Space (via C++ HandleJump)")
        log("  - Sprint with Shift (via C++ HandleSprintStarted/Completed)")
        log("  - Attack with left mouse (via C++ HandleAttack)")
        log("  - Guard with right mouse (via C++ HandleGuardStarted/Completed)")
        log("")
        log("To test:")
        log("  1. Open the project in Unreal Editor")
        log("  2. Open L_Showcase or similar level")
        log("  3. Press Play (Alt+P)")
        log("  4. Test WASD movement and mouse look")
    else:
        log("=" * 70)
        log("VERDICT: NOT READY - FIX ERRORS FIRST")
        log("=" * 70)

    # Write output
    output = "\n".join(results)
    with open(OUTPUT_FILE, 'w') as f:
        f.write(output)

    unreal.log_warning(output)

run()
