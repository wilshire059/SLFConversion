"""
Test player spawning and game framework in L_Demo_Showcase
"""
import unreal

def log(msg):
    print(f"[SPAWN-TEST] {msg}")
    unreal.log(msg)

def test_game_framework():
    """Test that game framework classes are properly set up."""
    log("=" * 60)
    log("TESTING GAME FRAMEWORK CLASSES")
    log("=" * 60)

    # Check Game Mode
    gm_class = unreal.load_class(None, "/Script/SLFConversion.GM_SoulslikeFramework")
    if gm_class:
        log(f"✓ GM_SoulslikeFramework loaded")
        # Check parent class
        parent = gm_class.get_super_class()
        log(f"  Parent: {parent.get_name() if parent else 'None'}")
    else:
        log("✗ GM_SoulslikeFramework NOT FOUND")

    # Check Player Controller
    pc_class = unreal.load_class(None, "/Script/SLFConversion.PC_SoulslikeFramework")
    if pc_class:
        log(f"✓ PC_SoulslikeFramework loaded")
        parent = pc_class.get_super_class()
        log(f"  Parent: {parent.get_name() if parent else 'None'}")
    else:
        log("✗ PC_SoulslikeFramework NOT FOUND")

    # Check Game State
    gs_class = unreal.load_class(None, "/Script/SLFConversion.GS_SoulslikeFramework")
    if gs_class:
        log(f"✓ GS_SoulslikeFramework loaded")
        parent = gs_class.get_super_class()
        log(f"  Parent: {parent.get_name() if parent else 'None'}")
    else:
        log("✗ GS_SoulslikeFramework NOT FOUND")

    # Check Player State
    ps_class = unreal.load_class(None, "/Script/SLFConversion.PS_SoulslikeFramework")
    if ps_class:
        log(f"✓ PS_SoulslikeFramework loaded")
        parent = ps_class.get_super_class()
        log(f"  Parent: {parent.get_name() if parent else 'None'}")
    else:
        log("✗ PS_SoulslikeFramework NOT FOUND")

    # Check Character
    char_class = unreal.load_class(None, "/Script/SLFConversion.B_Soulslike_Character")
    if char_class:
        log(f"✓ B_Soulslike_Character loaded")
        parent = char_class.get_super_class()
        log(f"  Parent: {parent.get_name() if parent else 'None'}")
    else:
        log("✗ B_Soulslike_Character NOT FOUND")

def test_level_info():
    """Test level loading and world settings."""
    log("")
    log("=" * 60)
    log("TESTING LEVEL: L_Demo_Showcase")
    log("=" * 60)

    # Try to load the level asset
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

    try:
        # Check if the asset exists
        if unreal.EditorAssetLibrary.does_asset_exist(level_path):
            log(f"✓ Level asset exists: {level_path}")
        else:
            log(f"✗ Level asset NOT found: {level_path}")
            return
    except Exception as e:
        log(f"✗ Error checking level: {e}")

def test_blueprint_game_mode():
    """Test loading the Blueprint game mode."""
    log("")
    log("=" * 60)
    log("TESTING BLUEPRINT GAME MODE")
    log("=" * 60)

    # The Blueprint GM
    bp_gm_path = "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework"

    try:
        bp_gm = unreal.load_asset(bp_gm_path)
        if bp_gm:
            log(f"✓ Blueprint GM loaded: {bp_gm.get_name()}")

            # Get the generated class
            gen_class = bp_gm.generated_class()
            if gen_class:
                log(f"  Generated class: {gen_class.get_name()}")
                parent = gen_class.get_super_class()
                log(f"  Parent class: {parent.get_name() if parent else 'None'}")

                # Check if parent is correct (should be AGameModeBase now)
                parent_name = parent.get_name() if parent else ""
                if "GameMode" in parent_name:
                    log(f"  ✓ Correctly inherits from GameMode family")
                elif "Actor" in parent_name and "GameMode" not in parent_name:
                    log(f"  ✗ WRONG: Inherits from Actor, not GameMode!")
        else:
            log(f"✗ Could not load Blueprint GM")
    except Exception as e:
        log(f"✗ Error: {e}")

def test_player_start():
    """Check for Player Start in level."""
    log("")
    log("=" * 60)
    log("CHECKING PLAYER START")
    log("=" * 60)

    # This is harder to test in commandlet mode
    # We can check if the PlayerStart class is accessible
    player_start_class = unreal.load_class(None, "/Script/Engine.PlayerStart")
    if player_start_class:
        log("✓ PlayerStart class available")
    else:
        log("✗ PlayerStart class NOT found")

def run_tests():
    log("╔════════════════════════════════════════════════════════════╗")
    log("║       PLAYER SPAWN & GAME FRAMEWORK TEST                   ║")
    log("╚════════════════════════════════════════════════════════════╝")

    test_game_framework()
    test_level_info()
    test_blueprint_game_mode()
    test_player_start()

    log("")
    log("=" * 60)
    log("TEST COMPLETE")
    log("=" * 60)

if __name__ == "__main__":
    run_tests()
else:
    run_tests()
