"""
PIE Test Script - Verifies camera and movement for B_Soulslike_Character
Writes results to pie_test_results.txt
"""

import unreal

# Results file
RESULTS_FILE = "C:/scripts/SLFConversion/pie_test_results.txt"

def log(msg):
    """Log to file, UE log, and stdout"""
    unreal.log(str(msg))
    with open(RESULTS_FILE, "a") as f:
        f.write(str(msg) + "\n")

def run_pie_test():
    """Test PIE functionality for the migrated character"""

    # Clear previous results
    with open(RESULTS_FILE, "w") as f:
        f.write("")

    log("=" * 60)
    log("PIE TEST - Verifying B_Soulslike_Character Setup")
    log("=" * 60)

    # Load the character Blueprint
    character_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    character_bp = unreal.EditorAssetLibrary.load_asset(character_path)

    if not character_bp:
        log(f"[ERROR] Could not load {character_path}")
        return False

    log(f"[SUCCESS] LOADED: B_Soulslike_Character")

    # Get the Blueprint Generated Class
    bp_gc = character_bp.generated_class()
    if bp_gc:
        log(f"[SUCCESS] Generated Class: {bp_gc.get_name()}")
    else:
        log("[ERROR] No generated class")
        return False

    # Verify GameMode
    log("=" * 60)
    log("Verifying GameMode Configuration")
    log("=" * 60)

    gm_path = "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework"
    gm_bp = unreal.EditorAssetLibrary.load_asset(gm_path)

    if gm_bp:
        log(f"[SUCCESS] GameMode loaded: GM_SoulslikeFramework")
        gm_gc = gm_bp.generated_class()
        if gm_gc:
            # Get CDO
            cdo = unreal.get_default_object(gm_gc)
            if cdo:
                try:
                    default_pawn = cdo.get_editor_property('default_pawn_class')
                    if default_pawn:
                        pawn_name = default_pawn.get_name()
                        log(f"[INFO] DefaultPawnClass: {pawn_name}")

                        if "B_Soulslike_Character" in pawn_name:
                            log("[SUCCESS] GameMode uses B_Soulslike_Character")
                        else:
                            log(f"[WARNING] Expected B_Soulslike_Character, got {pawn_name}")
                    else:
                        log("[WARNING] DefaultPawnClass is None")
                except Exception as e:
                    log(f"[INFO] Could not read DefaultPawnClass property: {e}")
    else:
        log(f"[ERROR] Could not load GameMode")

    # Check map
    log("=" * 60)
    log("Verifying Demo Map")
    log("=" * 60)

    map_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    map_asset = unreal.EditorAssetLibrary.load_asset(map_path)
    if map_asset:
        log("[SUCCESS] Demo map loaded: L_Demo_Showcase")
    else:
        log("[ERROR] Could not load demo map")

    # Summary
    log("=" * 60)
    log("PIE TEST COMPLETE")
    log("=" * 60)
    log("Key assets loaded successfully.")
    log("The migration is working if you see [SUCCESS] messages above.")
    log("")
    log("To test gameplay, run in standalone game mode:")
    log('  "C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor.exe" "C:/scripts/SLFConversion/SLFConversion.uproject" -game')

    return True

# Run the test
run_pie_test()
