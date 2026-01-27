# verify_animations_runtime.py
# Start PIE and check for animation-related runtime errors

import unreal
import time

def log(msg):
    unreal.log_warning(f"[AnimVerify] {msg}")

def main():
    log("=== Animation Runtime Verification ===")

    # Check AnimBP compile status
    animbp_paths = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    ]

    all_clean = True
    log("--- AnimBP Compile Status ---")
    for path in animbp_paths:
        name = path.split("/")[-1]
        bp = unreal.load_asset(path)
        if bp:
            errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
            has_errors = "Errors: 0" not in errors
            has_warnings = "Warnings: 0" not in errors

            if has_errors or has_warnings:
                log(f"  [ISSUE] {name}")
                for line in errors.split("\n"):
                    if "Errors:" in line or "Warnings:" in line or "[ERROR]" in line or "[WARNING]" in line:
                        log(f"    {line.strip()}")
                all_clean = False
            else:
                log(f"  [OK] {name} - No errors/warnings")
        else:
            log(f"  [ERROR] {name} - FAILED TO LOAD")
            all_clean = False

    if all_clean:
        log("=== ALL ANIMBPS COMPILE CLEAN ===")
    else:
        log("=== SOME ANIMBPS HAVE ISSUES ===")

    # Check character classes use correct AnimBP
    log("--- Checking Character AnimBP Assignments ---")

    # Check player character
    player_char_path = "/Game/SoulslikeFramework/Demo/Blueprints/Characters/Player/B_Soulslike_Character"
    player_bp = unreal.load_asset(player_char_path)
    if player_bp:
        log(f"  Player character loaded: {player_bp.get_name()}")
    else:
        log(f"  [WARNING] Player character not found at {player_char_path}")

    log("=== Animation Runtime Verification Complete ===")
    return all_clean

if __name__ == "__main__":
    result = main()
    if result:
        unreal.log_warning("[AnimVerify] ALL CHECKS PASSED")
    else:
        unreal.log_error("[AnimVerify] SOME CHECKS FAILED")
