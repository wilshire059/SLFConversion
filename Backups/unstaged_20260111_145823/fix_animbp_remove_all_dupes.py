# fix_animbp_remove_all_dupes.py
# Remove ALL Blueprint variables that duplicate C++ UPROPERTY

import unreal

def log(msg):
    unreal.log_warning(f"[FixDupes] {msg}")

def main():
    log("=" * 60)
    log("Removing ALL Duplicate Blueprint Variables from AnimBP")
    log("=" * 60)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log(f"[ERROR] Failed to load AnimBP")
        return

    # These variables exist in C++ and must be removed from Blueprint
    # All 23 variables from ABP_SoulslikeCharacter_Additive.h
    vars_to_remove = [
        "WorldLocation",
        "WorldRotation",
        "Acceleration",
        "Acceleration2D",
        "bIsAccelerating",
        "Velocity",
        "Velocity2D",
        "Direction",
        "Speed",
        "AnimDataAsset",
        "GrantedTags",
        "bIsBlocking",
        "IsCrouched",
        "bIsFalling",
        "IsResting",
        "LeftHandOverlayState",
        "RightHandOverlayState",
        "ActiveOverlayState",
        "EquipmentManager",
        "CombatManager",
        "ActionManager",
        "ActiveGuardSequence",
        "IkWeight",
        "ActiveHitNormal",
    ]

    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"AnimBP Parent: {parent}")

    # Get current Blueprint variables
    bp_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Blueprint has {len(bp_vars)} variables currently")
    for v in bp_vars:
        log(f"  - {v}")

    # Remove each variable
    log("")
    log("Removing duplicate variables...")
    removed_count = 0
    for var_name in vars_to_remove:
        success = unreal.SLFAutomationLibrary.remove_variable(bp, var_name)
        if success:
            log(f"  [REMOVED] {var_name}")
            removed_count += 1
        else:
            # Variable might not exist or failed
            pass

    log(f"")
    log(f"Removed {removed_count} variables")

    # Save the Blueprint
    if removed_count > 0:
        unreal.EditorAssetLibrary.save_asset(animbp_path)
        log("[OK] Saved AnimBP")

    # Reload and check compile status
    bp = unreal.load_asset(animbp_path)

    # Get remaining variables
    bp_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Blueprint now has {len(bp_vars)} variables")
    for v in bp_vars:
        log(f"  - {v}")

    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log("")
    log("Compile Status:")
    for line in errors.split('\n')[:30]:
        if line.strip():
            log(f"  {line}")

    log("=" * 60)

if __name__ == "__main__":
    main()
