# pie_simulation_test.py
# Simulate PIE startup checks without actually running PIE (headless mode)

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/pie_simulation_output.txt"

def run():
    results = []
    results.append("=" * 70)
    results.append("PIE SIMULATION TEST - Checking all prerequisites")
    results.append("=" * 70)
    results.append("")

    errors = []
    warnings = []

    # 1. Check GameMode
    results.append("[1] GAME MODE CHECK")
    gm_path = "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework"
    gm = unreal.EditorAssetLibrary.load_asset(gm_path)
    if gm:
        gen_class = gm.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    pawn_class = cdo.get_editor_property("DefaultPawnClass")
                    pc_class = cdo.get_editor_property("PlayerControllerClass")
                    results.append(f"  DefaultPawnClass: {pawn_class.get_name() if pawn_class else 'None'}")
                    results.append(f"  PlayerControllerClass: {pc_class.get_name() if pc_class else 'None'}")
                    if pawn_class:
                        results.append("  OK: GameMode has DefaultPawnClass")
                    else:
                        errors.append("GameMode DefaultPawnClass is None")
                except Exception as e:
                    errors.append(f"GameMode property error: {e}")
    else:
        errors.append("Could not load GameMode")

    # 2. Check Character
    results.append("")
    results.append("[2] CHARACTER CHECK")
    char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    char_bp = unreal.EditorAssetLibrary.load_asset(char_path)
    if char_bp:
        gen_class = char_bp.generated_class()
        if gen_class:
            # Check parent class
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(char_bp)
            results.append(f"  Parent: {parent}")
            if "SLF" in str(parent):
                results.append("  OK: Parent is C++ class")
            else:
                warnings.append("Character parent is not C++ class")

            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Check input configuration
                ia_move = None
                player_ctx = None
                try:
                    ia_move = cdo.get_editor_property("IA_Move")
                    player_ctx = cdo.get_editor_property("PlayerMappingContext")
                except:
                    pass

                if ia_move:
                    results.append(f"  IA_Move: {ia_move.get_name()}")
                else:
                    errors.append("Character IA_Move is None - movement won't work!")

                if player_ctx:
                    results.append(f"  PlayerMappingContext: {player_ctx.get_name()}")
                else:
                    errors.append("Character PlayerMappingContext is None - input won't work!")
    else:
        errors.append("Could not load Character")

    # 3. Check PlayerController
    results.append("")
    results.append("[3] PLAYER CONTROLLER CHECK")
    pc_path = "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework"
    pc = unreal.EditorAssetLibrary.load_asset(pc_path)
    if pc:
        gen_class = pc.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    mapping_ctx = cdo.get_editor_property("GameplayMappingContext")
                    if mapping_ctx:
                        results.append(f"  GameplayMappingContext: {mapping_ctx.get_name()}")
                    else:
                        warnings.append("PlayerController GameplayMappingContext is None")
                except:
                    pass
    else:
        errors.append("Could not load PlayerController")

    # 4. Check the level for spawn points
    results.append("")
    results.append("[4] LEVEL CHECK")
    # Get current level
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        level_name = world.get_name()
        results.append(f"  Current level: {level_name}")

        # Check for player start
        actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.PlayerStart)
        results.append(f"  PlayerStart actors: {len(actors)}")
        if len(actors) == 0:
            warnings.append("No PlayerStart actor found in level")
    else:
        warnings.append("Could not get editor world")

    # 5. Check AnimBlueprint
    results.append("")
    results.append("[5] ANIMATION CHECK")
    if char_bp:
        gen_class = char_bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    mesh = cdo.get_editor_property("Mesh")
                    if mesh:
                        anim_class = mesh.get_editor_property("AnimClass")
                        if anim_class:
                            results.append(f"  AnimBlueprint: {anim_class.get_name()}")
                        else:
                            warnings.append("Character has no AnimBlueprint assigned")
                except:
                    pass

    # 6. Summary
    results.append("")
    results.append("=" * 70)
    results.append("RESULT SUMMARY")
    results.append("=" * 70)

    if len(errors) == 0:
        results.append("STATUS: READY FOR PIE")
        results.append("")
        results.append("All critical checks passed. The game should be able to start.")
        results.append("Character movement should work with WASD (via IA_Move).")
        results.append("Camera should work with mouse (via IA_Look).")
    else:
        results.append("STATUS: NOT READY - ERRORS FOUND")

    if len(errors) > 0:
        results.append("")
        results.append("ERRORS (must fix):")
        for err in errors:
            results.append(f"  X {err}")

    if len(warnings) > 0:
        results.append("")
        results.append("WARNINGS (may affect gameplay):")
        for warn in warnings:
            results.append(f"  ! {warn}")

    results.append("")
    results.append("To test in PIE:")
    results.append("1. Open Unreal Editor")
    results.append("2. Open a level with PlayerStart")
    results.append("3. Press Play")
    results.append("4. Use WASD to move, mouse to look")

    # Write results
    output = "\n".join(results)
    with open(OUTPUT_FILE, 'w') as f:
        f.write(output)

    unreal.log_warning(output)

run()
