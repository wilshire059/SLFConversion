"""
Action System Setup Verification Test
Verifies action blueprints, data assets, and C++ classes are correctly configured.
"""

import unreal

def log(msg):
    """Log to both Python print and UE log."""
    print(msg)
    unreal.log(msg)

def run_test():
    """Verify action system is correctly set up."""

    log("=" * 70)
    log("ACTION SYSTEM SETUP VERIFICATION")
    log("=" * 70)

    errors = []
    warnings = []
    successes = []

    # Test 1: Check Action Logic Blueprints
    log("")
    log("[TEST 1] Action Logic Blueprints")
    log("-" * 40)

    action_logic_paths = [
        "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action",
        "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_PickupItemMontage",
        "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Dodge",
        "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Jump",
        "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_ComboLight_L",
        "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_ComboLight_R",
        "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_ComboHeavy",
    ]

    for path in action_logic_paths:
        bp = unreal.EditorAssetLibrary.load_asset(path)
        name = path.split("/")[-1]
        if bp:
            gen_class = bp.generated_class()
            if gen_class:
                log(f"  [OK] {name} - Class: {gen_class.get_name()}")
                successes.append(f"Action logic: {name}")
            else:
                log(f"  [WARN] {name} - no generated class")
                warnings.append(f"No generated class for {name}")
        else:
            log(f"  [ERROR] {name} - failed to load")
            errors.append(f"Failed to load action: {name}")

    # Test 2: Check Action Data Assets
    log("")
    log("[TEST 2] Action Data Assets (PDA_Action)")
    log("-" * 40)

    data_asset_paths = [
        "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_PickupItemMontage",
        "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge",
        "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Jump",
    ]

    for path in data_asset_paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        name = path.split("/")[-1]
        if asset:
            log(f"  [OK] {name}")
            successes.append(f"Data asset: {name}")
        else:
            log(f"  [WARN] {name} - not found")
            warnings.append(f"Data asset not found: {name}")

    # Test 3: Check ActionManager Component
    log("")
    log("[TEST 3] ActionManager Component")
    log("-" * 40)

    am_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager"
    am_bp = unreal.EditorAssetLibrary.load_asset(am_path)
    if am_bp:
        gen_class = am_bp.generated_class()
        if gen_class:
            log(f"  [OK] AC_ActionManager - Class: {gen_class.get_name()}")
            successes.append("ActionManager component")
        else:
            log(f"  [WARN] AC_ActionManager - no generated class")
            warnings.append("ActionManager has no generated class")
    else:
        log(f"  [ERROR] AC_ActionManager - failed to load")
        errors.append("Failed to load ActionManager")

    # Test 4: Check Character Blueprints
    log("")
    log("[TEST 4] Character Blueprints")
    log("-" * 40)

    char_paths = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
    ]

    for path in char_paths:
        bp = unreal.EditorAssetLibrary.load_asset(path)
        name = path.split("/")[-1]
        if bp:
            gen_class = bp.generated_class()
            if gen_class:
                log(f"  [OK] {name}")
                successes.append(f"Character: {name}")
            else:
                log(f"  [WARN] {name} - no generated class")
                warnings.append(f"No generated class for {name}")
        else:
            log(f"  [ERROR] {name} - failed to load")
            errors.append(f"Failed to load character: {name}")

    # Summary
    log("")
    log("=" * 70)
    log("TEST SUMMARY")
    log("=" * 70)
    log(f"  Successes: {len(successes)}")
    log(f"  Warnings:  {len(warnings)}")
    log(f"  Errors:    {len(errors)}")

    if errors:
        log("")
        log("Errors:")
        for e in errors:
            log(f"  - {e}")

    if warnings:
        log("")
        log("Warnings:")
        for w in warnings:
            log(f"  - {w}")

    log("")
    log("=" * 70)
    if errors:
        log("[RESULT] TEST FAILED - Fix errors above")
        return False
    elif warnings:
        log("[RESULT] TEST PASSED with warnings")
        return True
    else:
        log("[RESULT] TEST PASSED")
        return True

if __name__ == "__main__":
    run_test()
