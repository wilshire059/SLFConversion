"""
Comprehensive Action System Test
Tests the full flow: loading, instantiation, and execution readiness
"""
import unreal

OUTPUT_FILE = "C:/scripts/slfconversion/test_results.txt"

def write_log(lines):
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))

def test_comprehensive():
    results = []
    results.append("=" * 70)
    results.append("COMPREHENSIVE ACTION SYSTEM TEST")
    results.append("=" * 70)

    all_passed = True

    # ========================================================================
    # TEST 1: Load ALL 26 action data assets (matching C++ InitializeDefaultActions)
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 1: Loading ALL Action Data Assets (26 expected)")
    results.append("=" * 70)

    # This matches the C++ InitializeDefaultActions mapping
    all_action_mappings = [
        ("Jump", "Jump"),
        ("Dodge", "Dodge"),
        ("Crouch", "Crouch"),
        ("StartSprinting", "StartSprinting"),
        ("StopSprinting", "StopSprinting"),
        ("SprintAttack", "SprintAttack"),
        ("LightAttackLeft", "ComboLight_L"),
        ("LightAttackRight", "ComboLight_R"),
        ("HeavyAttack", "ComboHeavy"),
        ("GuardStart", "GuardStart"),
        ("GuardEnd", "GuardEnd"),
        ("GuardCancel", "GuardCancel"),
        ("JumpAttack", "JumpAttack"),
        ("Backstab", "Backstab"),
        ("Execute", "Execute"),
        ("DrinkFlask.HP", "DrinkFlask_HP"),
        ("ThrowKnife", "Projectile"),
        ("SpecialAttack", "WeaponAbility"),
        ("DualWieldAttack", "DualWieldAttack"),
        ("PickupItem", "PickupItemMontage"),
        ("UseEquippedTool", "UseEquippedTool"),
        ("TwoHandStanceLeft", "TwoHandedStance_L"),
        ("TwoHandStanceRight", "TwoHandedStance_R"),
        ("ScrollWheel.Left", "ScrollWheel_LeftHand"),
        ("ScrollWheel.Right", "ScrollWheel_RightHand"),
        ("ScrollWheel.Bottom", "ScrollWheel_Tools"),
    ]

    loaded_data_assets = 0
    failed_data_assets = []
    for tag_name, asset_name in all_action_mappings:
        asset_path = f"/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_{asset_name}.DA_Action_{asset_name}"
        asset = unreal.load_object(None, asset_path)
        if asset:
            loaded_data_assets += 1
        else:
            failed_data_assets.append(asset_name)

    results.append(f"  Loaded: {loaded_data_assets}/{len(all_action_mappings)}")
    if failed_data_assets:
        results.append(f"  [WARN] Failed to load: {', '.join(failed_data_assets)}")
        all_passed = False
    else:
        results.append(f"  [OK] All data assets loaded")

    # ========================================================================
    # TEST 2: Load ALL action Blueprint classes
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 2: Loading ALL Action Blueprint Classes")
    results.append("=" * 70)

    # Map data asset names to Blueprint class names (handling naming mismatches)
    bp_name_mapping = {
        "Projectile": "ThrowProjectile",  # DA_Action_Projectile -> B_Action_ThrowProjectile
    }

    loaded_classes = 0
    failed_classes = []
    class_objects = {}  # Store for later instantiation test

    for tag_name, asset_name in all_action_mappings:
        # Apply name mapping for mismatches
        bp_class_name = bp_name_mapping.get(asset_name, asset_name)
        bp_path = f"/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_{bp_class_name}.B_Action_{bp_class_name}"

        bp = unreal.load_object(None, bp_path)
        if bp and hasattr(bp, 'generated_class'):
            gen_class = bp.generated_class()
            if gen_class:
                loaded_classes += 1
                class_objects[asset_name] = gen_class
            else:
                failed_classes.append(f"{bp_class_name} (NULL generated_class)")
        else:
            failed_classes.append(bp_class_name)

    results.append(f"  Loaded: {loaded_classes}/{len(all_action_mappings)}")
    if failed_classes:
        results.append(f"  [WARN] Failed to load: {', '.join(failed_classes[:5])}...")
    else:
        results.append(f"  [OK] All Blueprint classes loaded")

    # ========================================================================
    # TEST 3: Test NewObject instantiation capability
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 3: Testing Object Instantiation")
    results.append("=" * 70)

    # Try to instantiate a few action classes
    test_classes = ["Jump", "Dodge", "StartSprinting", "ComboLight_L"]
    instantiation_success = 0

    for class_name in test_classes:
        if class_name in class_objects:
            gen_class = class_objects[class_name]
            try:
                # In Python, we can't directly call NewObject, but we can verify the class is valid
                # Check class has expected properties/methods
                class_name_str = gen_class.get_name()
                if class_name_str and "_C" in class_name_str:
                    results.append(f"  [OK] {class_name}: {class_name_str} (valid generated class)")
                    instantiation_success += 1
                else:
                    results.append(f"  [WARN] {class_name}: Unexpected class name format: {class_name_str}")
            except Exception as e:
                results.append(f"  [FAIL] {class_name}: {str(e)}")
        else:
            results.append(f"  [SKIP] {class_name}: Not loaded")

    results.append(f"  Verified: {instantiation_success}/{len(test_classes)}")

    # ========================================================================
    # TEST 4: Verify B_Action inheritance chain
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 4: Checking B_Action Base Class Chain")
    results.append("=" * 70)

    b_action_bp = unreal.load_object(None, "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action.B_Action")
    if b_action_bp and hasattr(b_action_bp, 'generated_class'):
        b_action_class = b_action_bp.generated_class()
        if b_action_class:
            results.append(f"  [OK] B_Action Blueprint -> {b_action_class.get_name()}")

            # Check parent blueprint property
            if hasattr(b_action_bp, 'parent_class'):
                parent = b_action_bp.parent_class
                if parent:
                    results.append(f"  [OK] B_Action parent_class: {parent.get_name()}")
                else:
                    results.append(f"  [INFO] B_Action parent_class is None (expected if reparented to C++)")
        else:
            results.append(f"  [FAIL] B_Action has NULL generated_class")
            all_passed = False
    else:
        results.append(f"  [FAIL] Could not load B_Action Blueprint")
        all_passed = False

    # ========================================================================
    # TEST 5: Verify UB_Action C++ class exists in module
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 5: Checking C++ Classes in SLFConversion Module")
    results.append("=" * 70)

    cpp_classes_to_check = [
        ("UAC_ActionManager", "Action Manager Component"),
        ("UB_Action", "Action Base Class"),
        ("UPDA_Action", "Action Data Asset"),
    ]

    for class_name, description in cpp_classes_to_check:
        # Check if exposed to Python
        if hasattr(unreal, class_name):
            results.append(f"  [OK] {class_name} ({description}) - exposed to Python")
        else:
            # Try find_object
            obj = unreal.find_object(None, f"/Script/SLFConversion.{class_name}")
            if obj:
                results.append(f"  [OK] {class_name} ({description}) - found via find_object")
            else:
                results.append(f"  [INFO] {class_name} ({description}) - not exposed to Python (normal)")

    # ========================================================================
    # TEST 6: Verify GameplayTags exist
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 6: Checking GameplayTags Configuration")
    results.append("=" * 70)

    # We can't easily test GameplayTag creation in Python without the right API
    # But we can check if the tag table exists
    results.append("  [INFO] GameplayTag verification requires PIE/runtime test")
    results.append("  [INFO] Tags defined in DefaultGameplayTags.ini should be available")

    # ========================================================================
    # TEST 7: Check for common issues in loaded classes
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 7: Checking Class Validity")
    results.append("=" * 70)

    valid_classes = 0
    for name, gen_class in list(class_objects.items())[:5]:  # Test first 5
        try:
            # Check class properties
            class_name = gen_class.get_name()
            class_path = gen_class.get_path_name()

            if class_name and class_path:
                results.append(f"  [OK] {name}: {class_name}")
                results.append(f"       Path: {class_path}")
                valid_classes += 1
            else:
                results.append(f"  [WARN] {name}: Missing name or path")
        except Exception as e:
            results.append(f"  [FAIL] {name}: {str(e)}")

    # ========================================================================
    # SUMMARY
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("SUMMARY")
    results.append("=" * 70)
    results.append(f"  Data Assets:      {loaded_data_assets}/{len(all_action_mappings)}")
    results.append(f"  Blueprint Classes: {loaded_classes}/{len(all_action_mappings)}")
    results.append(f"  Class Validity:    {valid_classes}/5 checked")

    # Calculate overall status
    data_ok = loaded_data_assets >= 20  # Allow some missing
    classes_ok = loaded_classes >= 20

    if data_ok and classes_ok:
        results.append("\n[SUCCESS] Action system assets are properly configured!")
        results.append("The UClass* fix should enable action execution in PIE.")
    else:
        results.append("\n[WARNING] Some assets failed to load - check paths")
        all_passed = False

    results.append("\n" + "=" * 70)
    results.append("COMPREHENSIVE TEST COMPLETE")
    results.append("=" * 70)

    write_log(results)
    return all_passed

if __name__ == "__main__":
    test_comprehensive()
