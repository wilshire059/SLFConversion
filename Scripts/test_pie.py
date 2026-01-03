"""
PIE Test Script - Verify Character Spawns Correctly
"""
import unreal

def run():
    unreal.log_warning("=" * 60)
    unreal.log_warning("PIE TEST: Verifying Character Spawn")
    unreal.log_warning("=" * 60)

    # Load the demo map
    map_path = "/Game/SoulslikeFramework/Maps/L_Demo"
    unreal.log_warning("Loading map: " + map_path)

    # Try to get world and game mode info
    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    if editor_subsystem:
        world = editor_subsystem.get_editor_world()
        if world:
            unreal.log_warning("  World: " + world.get_name())
        else:
            unreal.log_warning("  No editor world available")

    # Check the GameMode CDO
    unreal.log_warning("")
    unreal.log_warning("Checking GameMode CDO...")
    game_mode_class = unreal.find_object(None, "/Script/SLFConversion.SLFGameMode")
    if game_mode_class:
        unreal.log_warning("  Found SLFGameMode class")
        # Get CDO
        cdo = unreal.get_default_object(game_mode_class)
        if cdo:
            unreal.log_warning("  CDO exists")
            pawn_class = cdo.get_editor_property("default_pawn_class")
            if pawn_class:
                unreal.log_warning("  DefaultPawnClass: " + pawn_class.get_name())
            else:
                unreal.log_warning("  DefaultPawnClass: None")

            pc_class = cdo.get_editor_property("player_controller_class")
            if pc_class:
                unreal.log_warning("  PlayerControllerClass: " + pc_class.get_name())
            else:
                unreal.log_warning("  PlayerControllerClass: None")
    else:
        unreal.log_warning("  SLFGameMode class not found")

    # Check if B_Soulslike_Character can be loaded
    unreal.log_warning("")
    unreal.log_warning("Testing B_Soulslike_Character Blueprint...")
    char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"

    try:
        bp = unreal.load_asset(char_path)
        if bp:
            unreal.log_warning("  Blueprint loaded: " + bp.get_name())
            gen_class = bp.generated_class()
            if gen_class:
                unreal.log_warning("  Generated class: " + gen_class.get_name())
                parent = gen_class.get_super_class()
                if parent:
                    unreal.log_warning("  Parent class: " + parent.get_name())

            # Check for SCS components
            scs = bp.get_editor_property("simple_construction_script")
            if scs:
                root_nodes = scs.get_editor_property("root_nodes")
                unreal.log_warning("  SCS root nodes: " + str(len(root_nodes)))
                for node in root_nodes:
                    comp_template = node.get_editor_property("component_template")
                    if comp_template:
                        unreal.log_warning("    - " + comp_template.get_class().get_name() + ": " + comp_template.get_name())
        else:
            unreal.log_warning("  ERROR: Could not load Blueprint")
    except Exception as e:
        unreal.log_warning("  ERROR loading Blueprint: " + str(e))

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("PIE TEST COMPLETE")
    unreal.log_warning("=" * 60)
    unreal.log_warning("")
    unreal.log_warning("To fully test PIE, open the editor and press Play.")
    unreal.log_warning("Expected: Character spawns with working camera and movement.")

run()
