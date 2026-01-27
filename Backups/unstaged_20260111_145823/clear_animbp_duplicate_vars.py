# clear_animbp_duplicate_vars.py
# Clear Blueprint variables that now exist in C++ parent class
# This resolves "Tried to create a property X but another object already exists" errors

import unreal

def log(msg):
    unreal.log_warning(f"[ClearVars] {msg}")

# Variables that exist in C++ UABP_SoulslikeCharacter_Additive
# These need to be removed from the Blueprint to avoid duplicates
CPP_VARIABLES = [
    "LeftHandOverlayState",
    "RightHandOverlayState",
    "ActiveOverlayState",
    "AnimDataAsset",
    "EquipmentManager",
    "CombatManager",
    "ActionManager",
    "WorldLocation",
    "WorldRotation",
    "Acceleration",
    "Acceleration2D",
    "bIsAccelerating",
    "Velocity",
    "Velocity2D",
    "Direction",
    "Speed",
    "GrantedTags",
    "bIsBlocking",
    "IsCrouched",
    "bIsFalling",
    "IsResting",
    "ActiveGuardSequence",
    "IkWeight",
    "ActiveHitNormal",
]

def main():
    log("=" * 60)
    log("Clearing Duplicate Blueprint Variables")
    log("=" * 60)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log(f"[ERROR] Failed to load: {animbp_path}")
        return

    # Use automation library to clear variables
    for var_name in CPP_VARIABLES:
        result = unreal.SLFAutomationLibrary.remove_blueprint_variable(bp, var_name)
        if result:
            log(f"  [OK] Removed variable: {var_name}")
        else:
            log(f"  [SKIP] Variable not found or couldn't remove: {var_name}")

    # Save the asset
    unreal.EditorAssetLibrary.save_asset(animbp_path)
    log(f"[OK] Saved AnimBP")

    # Verify compile status
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log(f"Compile status: {errors}")

    log("=" * 60)
    log("Done!")
    log("=" * 60)

if __name__ == "__main__":
    main()
