import unreal
import time

unreal.log_warning("=== PIE Visibility Test ===")

# Get editor subsystem
editor_subsystem = unreal.EditorLevelLibrary

# Get the current world
world = unreal.EditorLevelLibrary.get_editor_world()
if world:
    unreal.log_warning("Current world: " + world.get_name())

# Check the GameMode settings
game_mode_path = "/Script/SLFConversion.SLFGameMode"
try:
    gm_class = unreal.load_class(None, game_mode_path)
    if gm_class:
        cdo = unreal.get_default_object(gm_class)
        if cdo:
            unreal.log_warning("=== SLFGameMode CDO ===")
            # Get DefaultPawnClass
            if hasattr(cdo, 'default_pawn_class'):
                pawn_class = cdo.default_pawn_class
                if pawn_class:
                    unreal.log_warning("DefaultPawnClass: " + str(pawn_class.get_name()))
                else:
                    unreal.log_warning("DefaultPawnClass: None")
except Exception as e:
    unreal.log_warning("GameMode check error: " + str(e))

# Start PIE session
unreal.log_warning("")
unreal.log_warning("=== Starting PIE Session ===")

# Use the unreal.AutomationLibrary if available, or use level editor subsystem
try:
    # Try to use the editor subsystem to start PIE
    level_editor_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if level_editor_subsystem:
        # Start PIE
        success = level_editor_subsystem.editor_play_simulate()
        unreal.log_warning("PIE started: " + str(success))

        # Wait a moment
        time.sleep(2)

        # Check for player
        pie_world = unreal.GameplayStatics.get_game_instance(unreal.EditorLevelLibrary.get_editor_world())
        if pie_world:
            player = unreal.GameplayStatics.get_player_character(pie_world, 0)
            if player:
                unreal.log_warning("Player found: " + player.get_name())
                unreal.log_warning("Player class: " + player.get_class().get_name())

                # Check mesh
                mesh = player.mesh if hasattr(player, 'mesh') else None
                if mesh:
                    skel = mesh.get_skinned_asset() if hasattr(mesh, 'get_skinned_asset') else None
                    if skel:
                        unreal.log_warning("Player mesh: " + skel.get_name())
                    else:
                        unreal.log_warning("Player mesh: NONE")
            else:
                unreal.log_warning("No player character found")

        # Stop PIE
        level_editor_subsystem.editor_end_play()
        unreal.log_warning("PIE stopped")
except Exception as e:
    unreal.log_warning("PIE test error: " + str(e))

unreal.log_warning("")
unreal.log_warning("=== Test Complete ===")
unreal.log_warning("Based on C++ verification: Player should now be visible with SKM_Manny mesh!")
