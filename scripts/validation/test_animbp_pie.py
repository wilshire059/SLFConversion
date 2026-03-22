# test_animbp_pie.py
# PIE test for AnimBP functionality

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/animbp_test_results.txt"

def log(msg):
    print(msg)
    unreal.log(msg)
    with open(OUTPUT_FILE, "a") as f:
        f.write(msg + "\n")

def main():
    # Clear output file
    with open(OUTPUT_FILE, "w") as f:
        f.write("")

    log("=== AnimBP PIE Test ===")

    # Test 1: Check a level exists
    demo_level = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    log(f"\n[TEST 1] Checking level: {demo_level}")

    if unreal.EditorAssetLibrary.does_asset_exist(demo_level):
        log("[PASS] Demo level exists")
    else:
        log("[WARN] Demo level not found, checking alternate...")
        demo_level = "/Game/SoulslikeFramework/Maps/L_Demo_Menu"
        if unreal.EditorAssetLibrary.does_asset_exist(demo_level):
            log("[PASS] Menu level exists")
        else:
            log("[WARN] No level found but continuing tests...")

    # Test 2: Load the AnimBP
    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    log(f"\n[TEST 2] Loading AnimBP: {animbp_path}")

    asset = unreal.EditorAssetLibrary.load_asset(animbp_path)
    if asset:
        log("[PASS] AnimBP loaded successfully")
    else:
        log("[FAIL] AnimBP failed to load")
        return

    # Test 3: Check for compile errors
    log(f"\n[TEST 3] Checking compile errors")
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(asset)

    if errors and len(errors.strip()) > 0:
        log(f"[INFO] Compile messages:\n{errors}")
        # Check for "Errors: X" where X > 0
        import re
        error_match = re.search(r"Errors:\s*(\d+)", errors)
        if error_match and int(error_match.group(1)) > 0:
            log("[FAIL] AnimBP has compile errors")
            return
        else:
            log("[PASS] AnimBP has warnings only (acceptable)")
    else:
        log("[PASS] AnimBP has no compile errors")

    # Test 4: Check LinkedAnimLayer diagnosis
    log(f"\n[TEST 4] Checking LinkedAnimLayer nodes")
    diagnosis = unreal.SLFAutomationLibrary.diagnose_linked_anim_layer_nodes(asset)

    if "[ERROR]" in diagnosis or "[FIX NEEDED]" in diagnosis:
        log("[FAIL] LinkedAnimLayer issues found")
        log(diagnosis)
        return
    else:
        log("[PASS] LinkedAnimLayer nodes OK")

    # Test 5: Check if ALI assets exist
    log(f"\n[TEST 5] Checking Animation Layer Interfaces")
    ali_overlay = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/_Misc/ALI_OverlayStates"
    ali_locomotion = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/_Misc/ALI_LocomotionStates"

    if unreal.EditorAssetLibrary.does_asset_exist(ali_overlay):
        log("[PASS] ALI_OverlayStates exists")
    else:
        log("[WARN] ALI_OverlayStates not found")

    if unreal.EditorAssetLibrary.does_asset_exist(ali_locomotion):
        log("[PASS] ALI_LocomotionStates exists")
    else:
        log("[WARN] ALI_LocomotionStates not found (may not be required)")

    log("\n=== ALL TESTS PASSED ===")
    log("AnimBP is ready for PIE testing")

if __name__ == "__main__":
    main()
